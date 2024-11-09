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
#include "flow_sensor.h"
#include "esp_log.h"

xl9535_t xl9535_dev;
pcf8575_t pcf_dev;

SemaphoreHandle_t binarySemaphore;
TaskHandle_t main_recirculation_task_handle = NULL;

#define TASK_STACK_SIZE 4096
#define TASK_PRIORITY 5


// Timer callback function to create new tasks
void timer_callback_main_recirculation(TimerHandle_t xTimer) {

  //if previous main task is not completed yet : shut down the system
  if(xSemaphoreTake(binarySemaphore, 0) == pdFALSE){

    vTaskDelete(main_recirculation_task_handle);
    main_recirculation_task_handle = NULL;
    //down the system
    xl9535_set_pin_output(&xl9535_dev, 0, false);
    xl9535_set_pin_output(&xl9535_dev, 1, false);
    xl9535_set_pin_output(&xl9535_dev, 2, false);
    xl9535_set_pin_output(&xl9535_dev, 3, false);
   // xl9535_set_pin_output(&xl9535_dev, 4, false);
    xl9535_set_pin_output(&xl9535_dev, 5, false);
    xl9535_set_pin_output(&xl9535_dev, 6, false);
    xl9535_set_pin_output(&xl9535_dev, 7, false);

    return;

  } else{
    main_recirculation_task_handle = NULL;
  }

  // Create a new task with a unique ID
  BaseType_t result = xTaskCreate(
      main_recirculation_task,               // Task function
      "created_task",            // Task name
      TASK_STACK_SIZE,           // Stack size
      NULL, // Parameter (task ID)
      TASK_PRIORITY,             // Task priority
      &main_recirculation_task_handle       // Task handle (optional)
  );

  if (result == pdPASS) {
    printf("Created new main recirculation task\n");
  } else {
    printf("Failed to create new main recirculation task\n");
  }
}


// Timer callback function to create new tasks
void timer_callback_tower_recirculation(TimerHandle_t xTimer) {

  // Create a new task with a unique ID
  BaseType_t result = xTaskCreate(
      tower_inner_circulation_task,               // Task function
      "tower_inner_recirculation_task",            // Task name
      TASK_STACK_SIZE,           // Stack size
      NULL, // Parameter (task ID)
      TASK_PRIORITY,             // Task priority
      NULL       // Task handle (optional)
  );

  if (result == pdPASS) {
    printf("Created new tower inner circulation task\n");
  } else {
    printf("Failed to create new tower inner circulation task\n");
  }
}

void start_main_recirculation_timer(){
  // Create a software timer for main recirculation
  TimerHandle_t task_creation_timer = xTimerCreate(
      "TaskCreationTimerForMainRecirculation",        // Timer name
      pdMS_TO_TICKS(1800000),       // Timer period (20 minute)
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
  TimerHandle_t tower_timer = xTimerCreate(
      "TaskCreationTimerForTowerRecirculation",        // Timer name
      pdMS_TO_TICKS(60000),       // Timer period (2 minute)
      pdTRUE,                     // Auto-reload
      0,                          // Timer ID (not used)
      timer_callback_tower_recirculation              // Timer callback function
  );

  if (tower_timer == NULL) {
    printf("Failed to create tower recirculation timer.\n");
    return;
  }

  // Start the timer
  if (xTimerStart(tower_timer, 0) != pdPASS) {
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

  // Inner circulation pin config
  xl9535_configure_pin_direction(&xl9535_dev, TOWER_INNER_PUMP, false);

  // reset all pins
  xl9535_set_pin_output(&xl9535_dev, 0, false);
  xl9535_set_pin_output(&xl9535_dev, 1, false);
  xl9535_set_pin_output(&xl9535_dev, 2, false);
  xl9535_set_pin_output(&xl9535_dev, 3, false);
  xl9535_set_pin_output(&xl9535_dev, 4, false);
  xl9535_set_pin_output(&xl9535_dev, 5, false);
  xl9535_set_pin_output(&xl9535_dev, 6, false);
  xl9535_set_pin_output(&xl9535_dev, 7, false);

  flow_sensor_config_t config = {
      .flow_sensor_pin = GPIO_NUM_13,        // Set the GPIO pin connected to the sensor
      .pulse_to_liters_factor = 7.5         // Conversion factor for sensor
  };

  // Initialize the flow sensor with the provided configuration
  flow_sensor_init(&config);
}

void app_main(void) {

  binarySemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(binarySemaphore);

//  esp_err_t ret = nvs_flash_init();
//  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//    ESP_ERROR_CHECK(nvs_flash_erase());
//    ret = nvs_flash_init();
//  }
//  ESP_ERROR_CHECK(ret);

//  wifi_init();

  hardware_init();
  start_main_recirculation_timer();
  //start_tower_recirculation_timer();

  while (1){
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


