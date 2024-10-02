#include <mqtt_client.h>
#include "state_functions.h"
#include "mqtt_manager.h"

// Assuming these devices and pin configurations are globally or externally declared
static const char *TAG = "State Machine";

static bool is_first_time(const uint8_t current_state, bool reset){
  static int pre_state  = -1;
  if(reset){
    pre_state = -1;
  }
  if(pre_state!=current_state){
    pre_state=current_state;
    return true;
  }
  return false;
}

// State functions directly implement hardware interaction logic
void start_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Start recirculation");
  is_first_time(STATE_START, 1)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Start recirculation", 0, 0, 0):0;
  state_dto->state = STATE_START;
  state_dto->error = ESP_OK;
}

void turn_on_main_tank_flow_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning on Main Tank Flow");
  is_first_time(STATE_TURN_ON_MAIN_TANK_FLOW, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Turning on Main Tank Flow", 0, 0, 0):0;
  state_dto->state = STATE_TURN_ON_MAIN_TANK_FLOW;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, MAIN_TANK_PUMP, PUMP_ON);
}

void check_main_tank_max_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Checking Main Tank Max Level");
  is_first_time(STATE_CHECK_MAIN_TANK_MAX, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Checking Main Tank Max Level", 0, 0, 0):0;
  state_dto->state = STATE_CHECK_MAIN_TANK_MAX;
  state_dto->error = pcf8575_read_pin(&pcf_dev, MAIN_TANK_LEVEL_MAX_PIN, &(state_dto->arg_in));
}

void turn_off_main_tank_flow_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning off Main Tank Flow");
  is_first_time(STATE_TURN_OFF_MAIN_TANK_FLOW, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Turning off Main Tank Flow", 0, 0, 0):0;
  state_dto->state = STATE_TURN_OFF_MAIN_TANK_FLOW;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, MAIN_TANK_PUMP, PUMP_OFF);

}

void turn_on_tower_flow_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning on Tower Flow");
  is_first_time(STATE_TURN_ON_TOWER_FLOW, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Turning on Tower Flow", 0, 0, 0):0;
  state_dto->state = STATE_TURN_ON_TOWER_FLOW;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_VALVE, VALVE_ON);
  vTaskDelay(pdMS_TO_TICKS(3000));
  if (state_dto->error != ESP_OK)
    return;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_PUMP, PUMP_ON);

  vTaskDelay(pdMS_TO_TICKS(60000));
}

void check_all_towers_filled_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Checking All Towers Filled");
  is_first_time(STATE_CHECK_ALL_TOWERS_FILLED, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Checking All Towers Filled", 0, 0, 0):0;
  state_dto->state = STATE_CHECK_ALL_TOWERS_FILLED;
  uint32_t flow_freq = get_flow_frequency();
  state_dto->error = (flow_freq<10);
  ESP_LOGI(TAG, "Flow frequency: %d Hz", flow_freq);

}

void turn_off_tower_flow_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning off Tower Flow");
  is_first_time(STATE_TURN_OFF_TOWER_FLOW, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Turning off Tower Flow", 0, 0, 0):0;
  state_dto->state = STATE_TURN_OFF_TOWER_FLOW;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_PUMP, PUMP_OFF);
  vTaskDelay(pdMS_TO_TICKS(3000));
  if (state_dto->error != ESP_OK)
    return;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_VALVE, VALVE_OFF);

}

void check_main_tank_min_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Checking Main Tank Min Level");
  is_first_time(STATE_CHECK_MAIN_TANK_MIN, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Checking Main Tank Min Level", 0, 0, 0):0;
  state_dto->state = STATE_CHECK_MAIN_TANK_MIN;
  state_dto->error = pcf8575_read_pin(&pcf_dev, MAIN_TANK_LEVEL_MIN_PIN, &(state_dto->arg_in));
}

void start_refill_main_tank_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Starting to Fill Main Tank");
  is_first_time(STATE_START_REFILL_MAIN_TANK, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Starting to Fill Main Tank", 0, 0, 0):0;
  state_dto->state = STATE_START_REFILL_MAIN_TANK;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, MAIN_TANK_REFILL_PUMP, PUMP_ON);
}

void check_main_tank_mid_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Checking Main Tank Mid Level");
  is_first_time(STATE_CHECK_MAIN_TANK_MID, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Checking Main Tank Mid Level", 0, 0, 0):0;
  state_dto->state = STATE_CHECK_MAIN_TANK_MID;
  state_dto->error = pcf8575_read_pin(&pcf_dev, MAIN_TANK_LEVEL_MID_DOWN_PIN, &(state_dto->arg_in));
}

void stop_refill_main_tank_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Stopping Fill Main Tank");
  is_first_time(STATE_STOP_REFILL_MAIN_TANK, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Stopping Fill Main Tank", 0, 0, 0):0;
  state_dto->state = STATE_STOP_REFILL_MAIN_TANK;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, MAIN_TANK_REFILL_PUMP, PUMP_OFF);
}

void correction_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "In Correction Stage");
  is_first_time(STATE_CORRECTION, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "In Correction Stage", 0, 0, 0):0;
  state_dto->state = STATE_CORRECTION;
  state_dto->error = ESP_OK;
}

void end_state(state_dto_t *state_dto) {
  xSemaphoreGive(binarySemaphore);
  ESP_LOGI(TAG, "Process Completed");
  is_first_time(STATE_END, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Process Completed", 0, 0, 0):0;
  state_dto->state = STATE_END;
  state_dto->error = ESP_OK;
}

void error_state(state_dto_t *state_dto) {
  ESP_LOGE(TAG,
           "Error occurred. error at state ID : %d, Error : %s",
           state_dto->state,
           esp_err_to_name(state_dto->error));
  is_first_time(STATE_ERROR, 0)==1?esp_mqtt_client_publish(client, "/topic/main_recirculation", "Error occurred", 0, 0, 0):0;
  state_dto->state = STATE_ERROR;
  state_dto->error = ESP_OK;
}



