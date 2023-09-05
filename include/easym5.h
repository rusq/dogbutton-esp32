#pragma once
#include <M5AtomS3.h>

// for some reason it's GRB, not RGB?
#define MBLACK 0x000000
#define MRED 0x00ff00
#define MGREEN 0xff0000
#define MBLUE 0x0000ff
#define MYELLOW 0xffff00

namespace em5 {

const int activeBrightness = 50;
const int sleepBrightness = 5;
const gpio_num_t BUTTON_PIN = GPIO_NUM_41;

// led sets the led to a given colour.
void IRAM_ATTR led(CRGB col, int brightness = activeBrightness) {
    M5.dis.setBrightness(brightness);
    M5.dis.drawpix(col);
    M5.dis.show();
}

// ready switches the led into the "ready" mode
// (light green, low brightness)
void ready() {
    M5.dis.setBrightness(activeBrightness);
    led(MGREEN);
}

} // namespace em5