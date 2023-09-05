#include "cycles.h"
#include "easym5.h"
#include "power.h"
#include "secrets.h"
#include <M5AtomS3.h>
#include <UniversalTelegramBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <esp_wifi.h>


const char *godaddy_root =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEADCCAuigAwIBAgIBADANBgkqhkiG9w0BAQUFADBjMQswCQYDVQQGEwJVUzEh\n"
    "MB8GA1UEChMYVGhlIEdvIERhZGR5IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBE\n"
    "YWRkeSBDbGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTA0MDYyOTE3\n"
    "MDYyMFoXDTM0MDYyOTE3MDYyMFowYzELMAkGA1UEBhMCVVMxITAfBgNVBAoTGFRo\n"
    "ZSBHbyBEYWRkeSBHcm91cCwgSW5jLjExMC8GA1UECxMoR28gRGFkZHkgQ2xhc3Mg\n"
    "MiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eTCCASAwDQYJKoZIhvcNAQEBBQADggEN\n"
    "ADCCAQgCggEBAN6d1+pXGEmhW+vXX0iG6r7d/+TvZxz0ZWizV3GgXne77ZtJ6XCA\n"
    "PVYYYwhv2vLM0D9/AlQiVBDYsoHUwHU9S3/Hd8M+eKsaA7Ugay9qK7HFiH7Eux6w\n"
    "wdhFJ2+qN1j3hybX2C32qRe3H3I2TqYXP2WYktsqbl2i/ojgC95/5Y0V4evLOtXi\n"
    "EqITLdiOr18SPaAIBQi2XKVlOARFmR6jYGB0xUGlcmIbYsUfb18aQr4CUWWoriMY\n"
    "avx4A6lNf4DD+qta/KFApMoZFv6yyO9ecw3ud72a9nmYvLEHZ6IVDd2gWMZEewo+\n"
    "YihfukEHU1jPEX44dMX4/7VpkI+EdOqXG68CAQOjgcAwgb0wHQYDVR0OBBYEFNLE\n"
    "sNKR1EwRcbNhyz2h/t2oatTjMIGNBgNVHSMEgYUwgYKAFNLEsNKR1EwRcbNhyz2h\n"
    "/t2oatTjoWekZTBjMQswCQYDVQQGEwJVUzEhMB8GA1UEChMYVGhlIEdvIERhZGR5\n"
    "IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBEYWRkeSBDbGFzcyAyIENlcnRpZmlj\n"
    "YXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQAD\n"
    "ggEBADJL87LKPpH8EsahB4yOd6AzBhRckB4Y9wimPQoZ+YeAEW5p5JYXMP80kWNy\n"
    "OO7MHAGjHZQopDH2esRU1/blMVgDoszOYtuURXO1v0XJJLXVggKtI3lpjbi2Tc7P\n"
    "TMozI+gciKqdi0FuFskg5YmezTvacPd+mSYgFFQlq25zheabIZ0KbIIOqPjCDPoQ\n"
    "HmyW74cNxA9hi63ugyuV+I6ShHI56yDqg+2DzZduCLzrTia2cyvk0/ZM/iZx4mER\n"
    "dEr/VxqHD3VILs9RaRegAhJhldXRQLIQTO7ErBBDpqWeCtWVYpoNz4iCxTIM5Cuf\n"
    "ReYNnyicsbkqWletNw+vHX/bvZ8=\n"
    "-----END CERTIFICATE-----\n";

// sleepTimeout is the time between keypresses after which the device goes to
// sleep. It is expressed in milliseconds.
const uint32_t SLEEP_TIMEOUT = 60000; // 60 sec
uint32_t last_pressed = 0;

// wifi creds:
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;
bool connected = false;
WiFiClientSecure client;

UniversalTelegramBot bot(BOT_TOKEN, client);
RTC_NOINIT_ATTR const String chat_id = ADMIN_ID;

// function declarations
void report();
void wakeup();

// interrupt/event handlers
void on_connected(WiFiEvent_t event, WiFiEventInfo_t info);

void setup() {
    M5.begin(false, true, false, true);
    // put your setup code here, to run once:
    USBSerial.println("initialising...");
    delay(50);

    em5::led(MRED);

    M5.Btn.begin();
    pinMode(em5::BUTTON_PIN, INPUT_PULLUP);

    WiFi.onEvent(on_connected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);

    // HTTP client gets the root certificate from godaddy
    // to verify the telegram server
    client.setCACert(godaddy_root);

    m5pow::init_power_save(em5::BUTTON_PIN);
    cycles::init();
}

void loop() {
    if (M5.Btn.wasPressed()) {
        report();
        last_pressed = millis();
    }
    // sleep if there was no activity for a while
    if (connected && millis() - last_pressed > SLEEP_TIMEOUT) {
        USBSerial.println("no activity for a while, going to sleep");
        em5::led(MGREEN, em5::sleepBrightness);

        connected = false;
        esp_err_t err;
        err = esp_wifi_stop(); // https://esp32.com/viewtopic.php?t=20960
        if (err != ESP_OK) {
            USBSerial.printf("esp_wifi_stop: %s\n", esp_err_to_name(err));
        }

        err = esp_light_sleep_start();
        if (err != ESP_OK) {
            USBSerial.printf("esp_light_sleep_start: %s\n",
                             esp_err_to_name(err));
        } else {
            // woken up
            wakeup();
            report();
        }
    }
    M5.Btn.read(); // or M5.update(); but this is more explicit.
    cycles::wait();
}

void report() {
    if (!connected) {
        USBSerial.println("Not connected to wifi");
        em5::led(MRED | MBLUE);
        return;
    }
    em5::led(MBLUE);
    if (!bot.sendMessage(chat_id, "dog bark")) {
        USBSerial.println("send error");
        em5::led(MYELLOW);
    } else {
        em5::ready();
    }
}

void wakeup() {
    em5::led(MRED | MBLUE);

    USBSerial.println("woke up, reestablish wifi connection");

    esp_err_t err = esp_wifi_start();
    if (err != ESP_OK) {
        USBSerial.printf("esp_wifi_start: %s\n", esp_err_to_name(err));
        em5::led(MYELLOW);
        return;
    }
    if (!WiFi.reconnect()) {
        USBSerial.println("WiFi.reconnect failed");
        em5::led(MYELLOW);
        return;
    }
    USBSerial.println("waiting for connection");
    while (WiFi.status() != WL_CONNECTED) {
        cycles::wait();
    }
    USBSerial.println("reconnected");
    last_pressed = millis();
    // connected = true?
    em5::ready();
}

void on_connected(WiFiEvent_t event, WiFiEventInfo_t info) {
    USBSerial.println("Connected to AP");
    USBSerial.println(WiFi.localIP());
    connected = true;
    em5::ready();
}