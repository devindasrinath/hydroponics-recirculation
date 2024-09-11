#ifndef __PCF8575_H__
#define __PCF8575_H__

#include <stdint.h>
#include <esp_err.h>
#include <driver/i2c.h>

// I2C frequency for PCF8575 communication
#define PCF8575_I2C_FREQ_HZ 100000
#define PCF8575_I2C_ADDRESS 0x20
// PCF8575 device descriptor
typedef struct {
  i2c_port_t port;   // I2C port number
  uint8_t addr;      // I2C address
} pcf8575_t;

// Initialize the PCF8575 device
esp_err_t pcf8575_init(pcf8575_t *dev, uint8_t addr, i2c_port_t port);

// Read the state of a single pin
esp_err_t pcf8575_read_pin(pcf8575_t *dev, uint8_t pin, uint8_t *val);

// Write a value to a single pin
esp_err_t pcf8575_write_pin(pcf8575_t *dev, uint8_t pin, uint8_t val);

// Read the state of both ports (16 bits)
esp_err_t pcf8575_read_ports(pcf8575_t *dev, uint16_t *val);

// Write a value to both ports (16 bits)
esp_err_t pcf8575_write_ports(pcf8575_t *dev, uint16_t val);

#endif // __PCF8575_H__
