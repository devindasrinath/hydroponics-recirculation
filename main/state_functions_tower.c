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

static session_t session = {.count = 0, .session_state = false, .on_count = 0, .off_count = 0};
// State functions directly implement hardware interaction logic
void start_tower_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Initiating tower inner circulation process.");
  is_first_time(STATE_START_TOWER, 1)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Initiating tower inner circulation process", 0, 0, 0):0;
  state_dto->state = STATE_START_TOWER;
  state_dto->error = ESP_OK;
}

void session_check_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Verifying current session status.");
  is_first_time(STATE_SESSION_CHECK, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Verifying current session status", 0, 0, 0):0;
  state_dto->state = STATE_SESSION_CHECK;
  state_dto->arg_in = session.session_state;
  state_dto->error = ESP_OK;
}

void start_new_session_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Starting new session. Turning on the Tower inner Flow. ");
  is_first_time(STATE_START_NEW_SESSION, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Starting new session. Turning on the Tower inner Flow", 0, 0, 0):0;
  state_dto->state = STATE_START_NEW_SESSION;
  session.session_state = true;
  session.count = 1;
  session.on_count = 1;
  session.off_count = 1;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_INNER_PUMP, PUMP_ON);
}

void on_period_checking_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Verifying if the on period is complete.");
  is_first_time(STATE_ON_PERIOD_CHECKING_STATE, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Verifying if the on period is complete", 0, 0, 0):0;
  state_dto->state = STATE_ON_PERIOD_CHECKING_STATE;
  state_dto->arg_in = session.on_count > session.count;
  state_dto->error = ESP_OK;
}

void no_op_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Proceeding with no-operation state.");
  is_first_time(STATE_NO_OP, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Proceeding with no-operation state", 0, 0, 0):0;
  state_dto->state = STATE_NO_OP;
  session.count++;
  state_dto->error = ESP_OK;
}

void tower_off_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning off the Tower inner Flow.");
  is_first_time(STATE_TURN_OFF_TOWER, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Turning off the Tower inner Flow", 0, 0, 0):0;
  state_dto->state = STATE_TURN_OFF_TOWER;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_INNER_PUMP, PUMP_OFF);
}

void off_period_checking_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Verifying if the off period is complete.");
  is_first_time(STATE_OFF_PERIOD_CHECKING_STATE, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Verifying if the off period is complete", 0, 0, 0):0;
  state_dto->state = STATE_OFF_PERIOD_CHECKING_STATE;
  state_dto->arg_in = (session.on_count + session.off_count) > (session.count + 1);
  state_dto->error = ESP_OK;
}

void session_clear_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Clearing session");
  is_first_time(STATE_SESSION_CLEAR, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Clearing session", 0, 0, 0):0;
  state_dto->state = STATE_SESSION_CLEAR;
  session.session_state = false;
}

void end_tower_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Period processing completed successfully.");
  is_first_time(STATE_END_TOWER, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "Period processing completed successfully", 0, 0, 0):0;
  state_dto->state = STATE_END_TOWER;
  state_dto->error = ESP_OK;
}

void error_tower_state(state_dto_t *state_dto) {
  ESP_LOGE(TAG, "An error occurred. State ID: %d, Error: %s", state_dto->state, esp_err_to_name(state_dto->error));
  is_first_time(STATE_ERROR_TOWER, 0)==1?esp_mqtt_client_publish(client, "/topic/tower_recirculation", "An error occurred", 0, 0, 0):0;
  state_dto->state = STATE_ERROR_TOWER;
  state_dto->error = ESP_OK;
}
