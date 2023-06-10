#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

struct DataPoint
{
    uint32_t ts;
    float temp;
    float hmdty;
    uint16_t co2;
    uint16_t tvoc;
};

#define FEED_WDT ESP.wdtFeed()
#define PAUSE_WDT ESP.wdtDisable()
#define CONT_WDT ESP.wdtEnable((WDTO_t) WDTO_1S)
#endif