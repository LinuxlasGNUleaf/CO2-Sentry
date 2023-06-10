#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_CCS811.h>
#include <Esp.h>
#include "SdFat.h"
#include "sdios.h"

#include "config.h"
#include "utils.h"

// OBJECTS
DHT dht(DHTPIN, DHTTYPE);
Adafruit_CCS811 ccs;
RTC_DS3231 rtc;
SdFat32 sd;

// METHOD DECLARATIONS
void readDHT(float *temperature, float *humidity);
void readCCS811(uint16_t *co2, uint16_t *tvoc);
bool pollScheduled();
void handleLEDs();
void writeBufferToSD();
void checkForIncoming();

#endif