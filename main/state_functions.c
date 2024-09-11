#include "state_functions.h"

// Assuming these devices and pin configurations are globally or externally declared
static const char *TAG = "State Machine";

// State functions directly implement hardware interaction logic
 void start_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Start recirculation");
  state_dto->state = STATE_START;
  state_dto->error = ESP_OK;
}

 void turn_on_main_tank_flow_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning on Main Tank Flow");
  state_dto->state = STATE_TURN_ON_MAIN_TANK_FLOW;
  state_dto->error =  xl9535_set_pin_output(&xl9535_dev, MAIN_TANK_PUMP, PUMP_ON);
}

 void check_main_tank_max_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Checking Main Tank Max Level");
   state_dto->state = STATE_CHECK_MAIN_TANK_MAX;
   state_dto->error = pcf8575_read_pin(&pcf_dev, MAIN_TANK_LEVEL_MAX_PIN, &(state_dto->arg_in));
}

 void turn_off_main_tank_flow_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning off Main Tank Flow");
   state_dto->state = STATE_TURN_OFF_MAIN_TANK_FLOW;
   state_dto->error = xl9535_set_pin_output(&xl9535_dev, MAIN_TANK_PUMP, PUMP_OFF);

}

 void turn_on_tower_flow_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning on Tower Flow");
   state_dto->state = STATE_TURN_ON_TOWER_FLOW;
   state_dto->error =  xl9535_set_pin_output(&xl9535_dev, TOWER_VALVE, VALVE_ON);
   vTaskDelay(pdMS_TO_TICKS(3000));
   if(state_dto->error!=ESP_OK)
     return ;
   state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_PUMP, PUMP_ON);
}

 void check_all_towers_filled_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Checking All Towers Filled");
   state_dto->state = STATE_CHECK_ALL_TOWERS_FILLED;
   state_dto->error = pcf8575_read_pin(&pcf_dev, TOWER_LEVEL_PIN, &(state_dto->arg_in));
}

 void turn_off_tower_flow_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning off Tower Flow");
   state_dto->state = STATE_TURN_OFF_TOWER_FLOW;
   state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_PUMP, PUMP_OFF);
   vTaskDelay(pdMS_TO_TICKS(3000));
   if(state_dto->error!=ESP_OK)
     return ;
   state_dto->error =  xl9535_set_pin_output(&xl9535_dev, TOWER_VALVE, VALVE_OFF);

}

 void check_main_tank_min_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Checking Main Tank Min Level");
   state_dto->state = STATE_CHECK_MAIN_TANK_MIN;
   state_dto->error = pcf8575_read_pin(&pcf_dev, MAIN_TANK_LEVEL_MIN_PIN, &(state_dto->arg_in));
}

 void start_refill_main_tank_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Starting to Fill Main Tank");
   state_dto->state = STATE_START_REFILL_MAIN_TANK;
   state_dto->error = xl9535_set_pin_output(&xl9535_dev, MAIN_TANK_REFILL_PUMP, PUMP_ON);
}

 void check_main_tank_mid_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Checking Main Tank Mid Level");
   state_dto->state = STATE_CHECK_MAIN_TANK_MID;
   state_dto->error = pcf8575_read_pin(&pcf_dev, MAIN_TANK_LEVEL_MID_DOWN_PIN, &(state_dto->arg_in));
}

 void stop_refill_main_tank_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Stopping Fill Main Tank");
   state_dto->state = STATE_STOP_REFILL_MAIN_TANK;
   state_dto->error = xl9535_set_pin_output(&xl9535_dev, MAIN_TANK_REFILL_PUMP, PUMP_OFF);
}

 void correction_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "In Correction Stage");
   state_dto->state = STATE_CORRECTION;
   state_dto->error = ESP_OK;
}

 void end_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Process Completed");
   state_dto->state = STATE_END;
   state_dto->error = ESP_OK;
}

void error_state(state_dto_t *state_dto) {
  ESP_LOGE(TAG, "Error occurred. error at state ID : %d, Error : %s", state_dto->state, esp_err_to_name(state_dto->error));
  state_dto->state = STATE_ERROR;
  state_dto->error = ESP_OK;
}