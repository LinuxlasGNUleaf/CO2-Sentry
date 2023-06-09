#ifndef CONFIG_H
#define CONFIG_H
// ==========> HARDWARE CONFIG
#define LED_G D3
#define LED_Y D8
#define LED_R D0

#define DHTPIN D4
#define DHTTYPE DHT22


// ==========> SOFTWARE CONFIG
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


// buffer options
const unsigned int sd_buffer_size = 5;
const unsigned int sd_buffer_warn_time = 2000;
#endif