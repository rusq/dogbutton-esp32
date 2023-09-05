#pragma once
#include <Arduino.h>

namespace m5pow {
void init_power_save(const gpio_num_t gpio_pin) {
    // sleep configuration
    esp_err_t err;
    err = esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    if (err != ESP_OK) {
        USBSerial.printf("esp_sleep_pd_config: %s\n", esp_err_to_name(err));
        return;
    }
    err = gpio_wakeup_enable(gpio_pin, GPIO_INTR_LOW_LEVEL);
    if (err != ESP_OK) {
        USBSerial.printf("gpio_wakeup_enable: %s\n", esp_err_to_name(err));
        return;
    }
    err = esp_sleep_enable_gpio_wakeup();
    if (err != ESP_OK) {
        USBSerial.printf("esp_sleep_enable_gpio_wakeup: %s\n",
                         esp_err_to_name(err));
        return;
    }
    /*
    // this fragment returns ERR_NOT_SUPPORTED, probably because
    // the compiler directive that allows configuring not set?
    esp_pm_config_esp32s3_t cfg = {
        .max_freq_mhz = 40,
        .min_freq_mhz = 40,
        .light_sleep_enable = true,
    };

    err = esp_pm_configure(&cfg);
    if (err != ESP_OK) {
        USBSerial.printf("esp_pm_configure: %s\n", esp_err_to_name(err));
        return;
    }
    */
}
}