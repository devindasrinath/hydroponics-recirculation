#include "pcf8575.h"

#define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)
#define BV(x) (1 << (x))

// Initialize the PCF8575 device
esp_err_t pcf8575_init(pcf8575_t *dev, uint8_t addr, i2c_port_t port)
{
  CHECK_ARG(dev);

  dev->port = port;
  dev->addr = addr;
  return ESP_OK;
}

// Read the state of a single pin
esp_err_t pcf8575_read_pin(pcf8575_t *dev, uint8_t pin, uint8_t *val)
{
  CHECK_ARG(dev && val);
  if (pin > 15) return ESP_ERR_INVALID_ARG;

  uint16_t port_val;
  esp_err_t err = pcf8575_read_ports(dev, &port_val);
  if (err != ESP_OK) return err;

  *val = (port_val >> pin) & 0x01;

  return ESP_OK;
}

// Write a value to a single pin
esp_err_t pcf8575_write_pin(pcf8575_t *dev, uint8_t pin, uint8_t val)
{
  CHECK_ARG(dev);
  if (pin > 15) return ESP_ERR_INVALID_ARG;

  uint16_t port_val;
  esp_err_t err = pcf8575_read_ports(dev, &port_val);
  if (err != ESP_OK) return err;

  if (val)
    port_val |= BV(pin);  // Set the pin
  else
    port_val &= ~BV(pin);  // Clear the pin

  return pcf8575_write_ports(dev, port_val);
}

// Read the state of both ports (16 bits)
esp_err_t pcf8575_read_ports(pcf8575_t *dev, uint16_t *val)
{
  CHECK_ARG(dev && val);

  uint8_t data[2];
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev->addr << 1) | I2C_MASTER_READ, true);
  i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);
  esp_err_t err = i2c_master_cmd_begin(dev->port, cmd, pdMS_TO_TICKS(1000));
  i2c_cmd_link_delete(cmd);

  if (err == ESP_OK) {
    *val = (data[1] << 8) | data[0];
  }

  return err;
}

// Write a value to both ports (16 bits)
esp_err_t pcf8575_write_ports(pcf8575_t *dev, uint16_t val)
{
  CHECK_ARG(dev);

  uint8_t data[2] = { val & 0xFF, (val >> 8) & 0xFF };

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev->addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write(cmd, data, 2, true);
  i2c_master_stop(cmd);
  esp_err_t err = i2c_master_cmd_begin(dev->port, cmd, pdMS_TO_TICKS(1000));
  i2c_cmd_link_delete(cmd);

  return err;
}
