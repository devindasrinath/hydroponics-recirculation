#include "flow_sensor.h"


static uint8_t flow_sensor_pin;          // Store the GPIO pin
static float pulse_to_liters_factor;     // Store the conversion factor

static volatile uint32_t pulse_count = 0;  // To store pulse count from ISR
static uint32_t flow_frequency = 0;        // Flow frequency in Hz (pulses per second)
static TimerHandle_t flow_timer = NULL;    // FreeRTOS timer handle
static const char *TAG = "FlowSensor";     // Tag for logging

// ISR to handle pulses from the flow sensor (called on rising edge of pulse)
static void IRAM_ATTR flow_sensor_isr_handler(void* arg) {
  pulse_count++;  // Increment pulse count on each pulse
}

// Timer callback to calculate flow frequency once every second
static void flow_timer_callback(TimerHandle_t xTimer) {
  flow_frequency = pulse_count;  // Copy pulse count as flow frequency (Hz)
  pulse_count = 0;               // Reset pulse count for the next interval
  ESP_LOGI(TAG, "Flow frequency: %d Hz", flow_frequency);
}

// Initialization function to configure the GPIO, ISR, and FreeRTOS timer
void flow_sensor_init(flow_sensor_config_t *config) {
  // Store the configuration parameters
  flow_sensor_pin = config->flow_sensor_pin;
  pulse_to_liters_factor = config->pulse_to_liters_factor;

  // Configure GPIO for flow sensor input (with pull-up) and interrupt on rising edge
  gpio_config_t io_conf = {
      .intr_type = GPIO_INTR_POSEDGE,      // Interrupt on rising edge
      .mode = GPIO_MODE_INPUT,             // Input mode
      .pin_bit_mask = (1ULL << flow_sensor_pin),  // Bit mask for the pin
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .pull_up_en = GPIO_PULLUP_ENABLE
  };
  gpio_config(&io_conf);

  // Install the ISR service and add the ISR handler for the flow sensor
  gpio_install_isr_service(0);
  gpio_isr_handler_add(flow_sensor_pin, flow_sensor_isr_handler, NULL);

  // Create and start a FreeRTOS timer with a 1-second period to calculate flow rate
  flow_timer = xTimerCreate("flow_rate_timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, flow_timer_callback);

  if (flow_timer != NULL) {
    xTimerStart(flow_timer, 0);  // Start the timer
  } else {
    ESP_LOGE(TAG, "Failed to create flow rate timer");
  }
}

// Function to get the current flow frequency (in pulses per second, Hz)
uint32_t get_flow_frequency(void) {
  return flow_frequency;  // Return the current flow frequency
}
