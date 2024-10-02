#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include <stdint.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_log.h"

// Struct to hold configuration parameters for the flow sensor
typedef struct {
  uint8_t flow_sensor_pin;         // GPIO pin for sensor input
  float pulse_to_liters_factor;    // Conversion factor: pulses per liter/min
} flow_sensor_config_t;

// Function to initialize the flow sensor (sets up GPIO, ISR, and FreeRTOS timer)
void flow_sensor_init(flow_sensor_config_t *config);

// Function to get the current flow frequency in pulses per second (Hz)
uint32_t get_flow_frequency(void);

#endif  // FLOW_SENSOR_H

