#include <mqtt_client.h>
#include "state_functions_tower.h"
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

static session_t session = {.count = 0, .on_count = 2, .off_count = 6};

// State functions directly implement hardware interaction logic
void start_tower_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Initiating tower inner circulation process.........");
  ESP_LOGI(TAG, "On time : %d min, Off time : %d min",session.on_count,session.off_count );
 // is_first_time(STATE_START_TOWER, 1)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Initiating tower inner circulation process", 0, 0, 0):0;
  state_dto->state = STATE_START_TOWER;
  state_dto->error = ESP_OK;
}


void get_session_type_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Verifying current session status.");
 // is_first_time(STATE_SESSION_CHECK, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Verifying current session status", 0, 0, 0):0;
  state_dto->state = STATE_SESSION_CHECK;

  if (session.count == 0) {
    // New session
    state_dto->arg_in = STATE_START_NEW_SESSION;
  } else if (session.count < session.on_count) {
    // No operation (before reaching `on_count`)
    state_dto->arg_in = STATE_NO_OP;
  } else if (session.count < session.on_count + session.off_count) {
    // Either turn off motor (at exactly `on_count`) or no operation (during `off_count` period)
    state_dto->arg_in = (session.count == session.on_count) ? STATE_TURN_OFF_TOWER : STATE_NO_OP;
  } else {
    // End of session
    state_dto->arg_in = STATE_SESSION_CLEAR;
  }
  state_dto->error = ESP_OK;
}

void start_new_session_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Starting new session. Turning on the Tower inner Flow.");
 // is_first_time(STATE_START_NEW_SESSION, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Starting new session. Turning on the Tower inner Flow", 0, 0, 0):0;
  state_dto->state = STATE_START_NEW_SESSION;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_INNER_PUMP, PUMP_ON);
}

void no_op_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Proceeding with no-operation state.");
  //is_first_time(STATE_NO_OP, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Proceeding with no-operation state", 0, 0, 0):0;
  state_dto->state = STATE_NO_OP;
  state_dto->error = ESP_OK;
}

void tower_off_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning off the Tower inner Flow.");
  //is_first_time(STATE_TURN_OFF_TOWER, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Turning off the Tower inner Flow", 0, 0, 0):0;
  state_dto->state = STATE_TURN_OFF_TOWER;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_INNER_PUMP, PUMP_OFF);
}

void tower_update_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Proceeding with update state.");
  //is_first_time(STATE_NO_OP, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Proceeding with update state", 0, 0, 0):0;
  session.count++;
  state_dto->state = STATE_UPDATE_TOWER;
  state_dto->error = ESP_OK;
}

void session_clear_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Clearing session");
  //is_first_time(STATE_SESSION_CLEAR, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Clearing session", 0, 0, 0):0;
  session.count = 0;
  state_dto->state = STATE_SESSION_CLEAR;
  state_dto->error = ESP_OK;
}

void end_tower_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Period processing completed successfully.");
  //is_first_time(STATE_END_TOWER, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Period processing completed successfully", 0, 0, 0):0;
  state_dto->state = STATE_END_TOWER;
  state_dto->error = ESP_OK;
}

void error_tower_state(state_dto_t *state_dto) {
  ESP_LOGE(TAG, "An error occurred. State ID: %d, Error: %s", state_dto->state, esp_err_to_name(state_dto->error));
//  is_first_time(STATE_ERROR_TOWER, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "An error occurred", 0, 0, 0):0;
  state_dto->state = STATE_ERROR_TOWER;
  state_dto->error = ESP_OK;
}
