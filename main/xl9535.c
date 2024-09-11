/**
 * @file xl9535.c
 * @brief Driver for the XL9535 GPIO expander.
 *
 * This file contains the implementation of functions to interface with the
 * XL9535 GPIO expander over I2C. The functions allow for configuring pin
 * direction, reading inputs, writing outputs, and configuring polarity inversion.
 *
 * @note The XL9535 expander has 16 GPIO pins split into two 8-bit registers.
 *       These registers control pin states, direction, and polarity.
 *
 * @author Devinda srinath (devinda.w@fidet.net)
 * @date 26/08/2024
 */

#include "xl9535.h"
#include "esp_log.h"

// Register Definitions
#define XL9535_REGISTER_INPUT_PORT_0  0x00
#define XL9535_REGISTER_INPUT_PORT_1  0x01
#define XL9535_REGISTER_OUTPUT_PORT_0 0x02
#define XL9535_REGISTER_OUTPUT_PORT_1 0x03
#define XL9535_REGISTER_POLARITY_INV_0 0x04
#define XL9535_REGISTER_POLARITY_INV_1 0x05
#define XL9535_REGISTER_CONFIG_0      0x06
#define XL9535_REGISTER_CONFIG_1      0x07

static const char *TAG = "XL9535";

/**
 * @brief Initialize the XL9535 device
 *
 * This function initializes the XL9535 GPIO expander by setting up the I2C port,
 * setting the I2C address, and performing any necessary initialization routines
 * for the device.
 *
 * @param dev Pointer to xl9535_t structure representing the device.
 * @param i2c_port I2C port number to be used for communication.
 * @param i2c_address I2C address of the XL9535 device.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t xl9535_init(xl9535_t *dev, i2c_port_t i2c_port, uint8_t i2c_address) {
  if (!dev) {
    ESP_LOGE(TAG, "Device pointer is NULL");
    return ESP_ERR_INVALID_ARG;
  }

  // Initialize the xl9535_t structure with the I2C port and address
  dev->i2c_port = i2c_port;
  dev->i2c_address = i2c_address;

  // Optionally, check the connection to the device by reading a known register
  uint8_t reg_data = 0;
  esp_err_t ret = xl9535_read_register(dev, XL9535_REGISTER_CONFIG_0, &reg_data);

  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "XL9535 device initialized successfully");
  } else {
    ESP_LOGE(TAG, "Failed to initialize XL9535 device");
  }

  return ret;
}

/**
 * @brief Write a value to a specific XL9535 register.
 *
 * This function sends a value to a specific register on the XL9535 GPIO expander.
 * It uses I2C communication to send the register address and the value to be written.
 *
 * @param dev Pointer to xl9535_t structure representing the device.
 * @param reg_addr Register address to write to.
 * @param value Value to write to the register.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t xl9535_write_register(xl9535_t *dev, uint8_t reg_addr, uint8_t value) {
  uint8_t data[2] = {reg_addr, value};  // First byte is the register address, second byte is the value
  esp_err_t ret = i2c_master_write_to_device(dev->i2c_port, dev->i2c_address, data, sizeof(data), pdMS_TO_TICKS(1000));

  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "Wrote 0x%02X to register 0x%02X", value, reg_addr);
  } else {
    ESP_LOGE(TAG, "Failed to write to register 0x%02X", reg_addr);
  }

  return ret;
}

/**
 * @brief Read a value from a specific XL9535 register.
 *
 * This function reads a value from a specific register on the XL9535 GPIO expander.
 * It uses I2C communication to send the register address and read back the register value.
 *
 * @param dev Pointer to xl9535_t structure representing the device.
 * @param reg_addr Register address to read from.
 * @param value Pointer to a variable where the read value will be stored.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t xl9535_read_register(xl9535_t *dev, uint8_t reg_addr, uint8_t *value) {
  esp_err_t ret = i2c_master_write_read_device(dev->i2c_port, dev->i2c_address, &reg_addr, 1, value, 1, pdMS_TO_TICKS(1000));

  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "Read 0x%02X from register 0x%02X", *value, reg_addr);
  } else {
    ESP_LOGE(TAG, "Failed to read from register 0x%02X", reg_addr);
  }

  return ret;
}


/**
 * @brief Set the output value of a specific pin.
 *
 * This function writes a specific value (high or low) to a given output pin.
 * The function determines the appropriate output register based on the pin number,
 * reads the current value of the register, modifies the specific bit corresponding to the pin,
 * and writes the updated value back to the register.
 *
 * @param dev Pointer to xl9535_t structure representing the device.
 * @param pin Pin number (0-15). Pins 0-7 correspond to PORT_0 and pins 8-15 correspond to PORT_1.
 * @param value Boolean value for output (true for high, false for low).
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t xl9535_set_pin_output(xl9535_t *dev, uint8_t pin, bool value) {
  uint8_t reg_addr;
  uint8_t reg_data;

  // Determine the appropriate register based on the pin number
  if (pin < 8) {
    reg_addr = XL9535_REGISTER_OUTPUT_PORT_0;
  } else {
    reg_addr = XL9535_REGISTER_OUTPUT_PORT_1;
    pin -= 8;
  }

  // Read current state of the output register
  esp_err_t ret = xl9535_read_register(dev, reg_addr, &reg_data);
  if (ret != ESP_OK) {
    return ret;
  }

  // Set or clear the bit corresponding to the pin
  if (value) {
    reg_data |= (1 << pin);
  } else {
    reg_data &= ~(1 << pin);
  }

  // Write back the updated register value
  return xl9535_write_register(dev, reg_addr, reg_data);
}

/**
 * @brief Read the input value of a specific pin.
 *
 * This function reads the current state (high or low) of a specific input pin.
 * The function determines the appropriate input register based on the pin number,
 * reads the register, and extracts the specific bit corresponding to the pin.
 *
 * @param dev Pointer to xl9535_t structure representing the device.
 * @param pin Pin number (0-15). Pins 0-7 correspond to PORT_0 and pins 8-15 correspond to PORT_1.
 * @param value Pointer to a boolean where the input state will be stored (true for high, false for low).
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t xl9535_read_pin_input(xl9535_t *dev, uint8_t pin, bool *value) {
  uint8_t reg_addr;
  uint8_t reg_data;

  // Determine the appropriate register based on the pin number
  if (pin < 8) {
    reg_addr = XL9535_REGISTER_INPUT_PORT_0;
  } else {
    reg_addr = XL9535_REGISTER_INPUT_PORT_1;
    pin -= 8;
  }

  // Read the input register
  esp_err_t ret = xl9535_read_register(dev, reg_addr, &reg_data);
  if (ret != ESP_OK) {
    return ret;
  }

  // Get the state of the specific pin
  *value = (reg_data >> pin) & 0x01;
  return ESP_OK;
}

/**
 * @brief Configure the direction of a specific pin (input or output).
 *
 * This function configures the direction of a specific pin by determining the appropriate
 * configuration register based on the pin number, reading the current value of the register,
 * modifying the specific bit corresponding to the pin to set it as input or output,
 * and writing the updated value back to the register.
 *
 * @param dev Pointer to xl9535_t structure representing the device.
 * @param pin Pin number (0-15). Pins 0-7 correspond to PORT_0 and pins 8-15 correspond to PORT_1.
 * @param is_input Boolean value (true for input, false for output).
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t xl9535_configure_pin_direction(xl9535_t *dev, uint8_t pin, bool is_input) {
  uint8_t reg_addr;
  uint8_t reg_data;

  // Determine the appropriate register based on the pin number
  if (pin < 8) {
    reg_addr = XL9535_REGISTER_CONFIG_0;
  } else {
    reg_addr = XL9535_REGISTER_CONFIG_1;
    pin -= 8;
  }

  // Read current configuration register
  esp_err_t ret = xl9535_read_register(dev, reg_addr, &reg_data);
  if (ret != ESP_OK) {
    return ret;
  }

  // Set or clear the bit to configure as input or output
  if (is_input) {
    reg_data |= (1 << pin);  // Set bit to configure as input
  } else {
    reg_data &= ~(1 << pin);  // Clear bit to configure as output
  }

  // Write back the updated configuration register
  return xl9535_write_register(dev, reg_addr, reg_data);
}

/**
 * @brief Set the polarity inversion of a specific pin.
 *
 * This function configures the polarity inversion of a specific input pin.
 * It determines the appropriate polarity inversion register based on the pin number,
 * reads the current value of the register, modifies the bit corresponding to the pin
 * to either invert or retain the polarity, and writes the updated value back to the register.
 *
 * @param dev Pointer to xl9535_t structure representing the device.
 * @param pin Pin number (0-15). Pins 0-7 correspond to PORT_0 and pins 8-15 correspond to PORT_1.
 * @param invert Boolean value (true to invert polarity, false to retain polarity).
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t xl9535_set_pin_polarity(xl9535_t *dev, uint8_t pin, bool invert) {
  uint8_t reg_addr;
  uint8_t reg_data;

  // Determine the appropriate register based on the pin number
  if (pin < 8) {
    reg_addr = XL9535_REGISTER_POLARITY_INV_0;
  } else {
    reg_addr = XL9535_REGISTER_POLARITY_INV_1;
    pin -= 8;
  }

  // Read current polarity inversion register
  esp_err_t ret = xl9535_read_register(dev, reg_addr, &reg_data);
  if (ret != ESP_OK) {
    return ret;
  }

  // Set or clear the bit to invert or retain polarity
  if (invert) {
    reg_data |= (1 << pin);  // Set bit to invert polarity
  } else {
    reg_data &= ~(1 << pin);  // Clear bit to retain polarity
  }

  // Write back the updated polarity inversion register
  return xl9535_write_register(dev, reg_addr, reg_data);
}
