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

// HARDWARE CONFIG
#define LED_G D3
#define LED_Y D8
#define LED_R D0

#define DHTPIN D4
#define DHTTYPE DHT22

// OBJECTS
DHT dht(DHTPIN, DHTTYPE);
Adafruit_CCS811 ccs;
RTC_DS3231 rtc;
SdFat32 sd;

// SOFTWARE CONFIG

// CMD_TYPES
#define TSTAMP_PUSH 'T'
#define PING 'P'

// times in seconds
const unsigned long initial_poll = 10;
const unsigned long poll_interval = 3;
const unsigned long i2c_cooldown = 1;

// times in milliseconds
const unsigned long led_on_interval = .1e3;
const unsigned long led_off_interval = 1e3;

// co2 threshold
const unsigned int good_co2_threshold = 600;
const unsigned int medium_co2_threshold = 800;

// METHOD DECLARATIONS
void readDHT(float *temperature, float *humidity);
void readCCS811(uint16_t *co2, uint16_t *tvoc);
bool pollScheduled();
void handleLEDs();
void writeLineToSDCard(char* line);
void checkForIncoming();

#endif