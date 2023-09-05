#pragma once

#include <Arduino.h>
// https://m1cr0lab-esp32.github.io/sleep-modes/time-control/

namespace cycles {

const uint16_t LOOP_FREQUENCY = 25; // Hz

// WAIT_PERIOD corresponds to the period of a cycle, i.e. its duration,
// expressed here in milliseconds. Thus, for a frequency of 25 Hz, we obtain a
// period of 1000 / 25 = 40 ms.
const uint16_t WAIT_PERIOD = 1000 / LOOP_FREQUENCY; // ms

struct Timer {
    uint32_t laptime;
    uint32_t ticks;
};

Timer timer;

void wait() {
    uint32_t now;
    do {
        now = millis();
    } while (now - timer.laptime < WAIT_PERIOD);
    timer.laptime = now;
    timer.ticks++;
}

void init() { timer = {millis(), 0}; }

} // namespace cycles