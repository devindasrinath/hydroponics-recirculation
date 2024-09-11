/**
 * @file xl9535.h
 * @brief Driver for the XL9535 GPIO expander.
 *
 * This file contains the prototypes to interface with the
 * XL9535 GPIO expander over I2C. The functions allow for configuring pin
 * direction, reading inputs, writing outputs, and configuring polarity inversion.
 *
 * @note The XL9535 expander has 16 GPIO pins split into two 8-bit registers.
 *       These registers control pin states, direction, and polarity.
 *
 * @author Devinda srinath (devinda.w@fidet.net)
 * @date 26/08/2024
 */

#ifndef XL9535_H
#define XL9535_H

#include "driver/i2c.h"
#include "esp_err.h"

// I2C address of XL9535 (7-bit address)
#define XL9535_I2C_ADDRESS 0x21  // Change this based on your hardware configuration

// XL9535 Device Structure
typedef struct {
  i2c_port_t i2c_port;
  uint8_t i2c_address;
} xl9535_t;

/**
 * @brief Initialize the XL9535 device
 *
 * @param dev Pointer to xl9535_t structure
 * @param i2c_port I2C port number
 * @param i2c_address I2C address of the XL9535 device
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t xl9535_init(xl9535_t *dev, i2c_port_t i2c_port, uint8_t i2c_address);

/**
 * @brief Set the output value of a specific pin
 *
 * @param dev Pointer to xl9535_t structure
 * @param pin Pin number (0-15)
 * @param value Boolean value for output (true for high, false for low)
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t xl9535_set_pin_output(xl9535_t *dev, uint8_t pin, bool value);

/**
 * @brief Read the input value of a specific pin
 *
 * @param dev Pointer to xl9535_t structure
 * @param pin Pin number (0-15)
 * @param value Pointer to store the read boolean value (true for high, false for low)
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t xl9535_read_pin_input(xl9535_t *dev, uint8_t pin, bool *value);

/**
 * @brief Configure the direction of a specific pin
 *
 * @param dev Pointer to xl9535_t structure
 * @param pin Pin number (0-15)
 * @param is_input Boolean value (true for input, false for output)
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t xl9535_configure_pin_direction(xl9535_t *dev, uint8_t pin, bool is_input);

/**
 * @brief Change the polarity inversion of a specific pin
 *
 * @param dev Pointer to xl9535_t structure
 * @param pin Pin number (0-15)
 * @param invert Boolean value (true to invert polarity, false to retain polarity)
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t xl9535_set_pin_polarity(xl9535_t *dev, uint8_t pin, bool invert);

/**
 * @brief Write a value to an XL9535 register
 *
 * @param dev Pointer to xl9535_t structure
 * @param reg_addr Address of the register to write to
 * @param reg_data Data byte to write to the register
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t xl9535_write_register(xl9535_t *dev, uint8_t reg_addr, uint8_t reg_data);

/**
 * @brief Read a value from an XL9535 register
 *
 * @param dev Pointer to xl9535_t structure
 * @param reg_addr Address of the register to read from
 * @param reg_data Pointer to store the read data byte
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t xl9535_read_register(xl9535_t *dev, uint8_t reg_addr, uint8_t *reg_data);

#endif // XL9535_H
