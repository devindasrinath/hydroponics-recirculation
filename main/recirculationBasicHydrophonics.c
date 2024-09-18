#include <sys/cdefs.h>
#include <stdio.h>
#include "constants.h"
#include "state_machine.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "state_machine_tower.h"
#include "nvs_flash.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

xl9535_t xl9535_dev;
pcf8575_t pcf_dev;



#define TASK_STACK_SIZE 4096
#define TASK_PRIORITY 5

// Counter for created tasks (optional)
static int task_counter = 0;


// Timer callback function to create new tasks
void timer_callback_main_recirculation(TimerHandle_t xTimer) {
  // Increment the task counter
  task_counter++;

  // Create a new task with a unique ID
  BaseType_t result = xTaskCreate(
      state_machine_task,               // Task function
      "created_task",            // Task name
      TASK_STACK_SIZE,           // Stack size
      NULL, // Parameter (task ID)
      TASK_PRIORITY,             // Task priority
      NULL       // Task handle (optional)
  );

  if (result == pdPASS) {
    printf("Created Task %d\n", task_counter);
  } else {
    printf("Failed to create Task %d\n", task_counter);
  }
}


// Timer callback function to create new tasks
void timer_callback_tower_recirculation(TimerHandle_t xTimer) {
  // Increment the task counter
  task_counter++;

  // Create a new task with a unique ID
  BaseType_t result = xTaskCreate(
      state_machine_tower_task,               // Task function
      "created_task1",            // Task name
      TASK_STACK_SIZE,           // Stack size
      NULL, // Parameter (task ID)
      TASK_PRIORITY,             // Task priority
      NULL       // Task handle (optional)
  );

  if (result == pdPASS) {
    printf("Created Task1 %d\n", task_counter);
  } else {
    printf("Failed to create Task1 %d\n", task_counter);
  }
}

void start_main_recirculation_timer(){
  // Create a software timer for main recirculation
  TimerHandle_t task_creation_timer = xTimerCreate(
      "TaskCreationTimerForMainRecirculation",        // Timer name
      pdMS_TO_TICKS(1200000),       // Timer period (20 minute)
      pdTRUE,                     // Auto-reload
      0,                          // Timer ID (not used)
      timer_callback_main_recirculation              // Timer callback function
  );

  if (task_creation_timer == NULL) {
    printf("Failed to create main recirculation timer.\n");
    return;
  }

  // Start the timer
  if (xTimerStart(task_creation_timer, 0) != pdPASS) {
    printf("Failed to start main recirculation timer.\n");
  }

  // Initial creation of a task (optional)
  timer_callback_main_recirculation(NULL);
}

void start_tower_recirculation_timer(){
  // Create a software timer for tower recirculation
  TimerHandle_t task_creation_timer1 = xTimerCreate(
      "TaskCreationTimerForTowerRecirculation",        // Timer name
      pdMS_TO_TICKS(120000),       // Timer period (2 minute)
      pdTRUE,                     // Auto-reload
      0,                          // Timer ID (not used)
      timer_callback_tower_recirculation              // Timer callback function
  );

  if (task_creation_timer1 == NULL) {
    printf("Failed to create tower recirculation timer.\n");
    return;
  }

  // Start the timer
  if (xTimerStart(task_creation_timer1, 0) != pdPASS) {
    printf("Failed to start tower recirculation timer.\n");
  }

  // Initial creation of a task (optional)
  timer_callback_tower_recirculation(NULL);
}

void hardware_init(){
  // Initialize I2C
  i2c_port_t i2c_port = I2C_NUM_0;
  i2c_config_t i2c_conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = 16,
      .scl_io_num = 17,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = 100000
  };
  i2c_param_config(i2c_port, &i2c_conf);
  i2c_driver_install(i2c_port, i2c_conf.mode, 0, 0, 0);


  xl9535_init(&xl9535_dev, I2C_NUM_0, XL9535_I2C_ADDRESS);
  pcf8575_init(&pcf_dev, PCF8575_I2C_ADDRESS, I2C_NUM_0);

  // Configure pin 3 as output
  xl9535_configure_pin_direction(&xl9535_dev, MAIN_TANK_PUMP, false);
  xl9535_configure_pin_direction(&xl9535_dev, TOWER_VALVE, false);
  xl9535_configure_pin_direction(&xl9535_dev, TOWER_PUMP, false);
  xl9535_configure_pin_direction(&xl9535_dev, TOWER_INNER_PUMP, false);

  xl9535_set_pin_output(&xl9535_dev, 0, false);
  xl9535_set_pin_output(&xl9535_dev, 1, false);
  xl9535_set_pin_output(&xl9535_dev, 2, false);
  xl9535_set_pin_output(&xl9535_dev, 3, false);
  xl9535_set_pin_output(&xl9535_dev, 4, false);
  xl9535_set_pin_output(&xl9535_dev, 5, false);
  xl9535_set_pin_output(&xl9535_dev, 6, false);
  xl9535_set_pin_output(&xl9535_dev, 7, false);
}

void app_main(void) {

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  wifi_init();
  hardware_init();
  start_main_recirculation_timer();
  start_tower_recirculation_timer();

  while (1){
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}