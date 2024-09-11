#include "state_functions_tower.h"

// Assuming these devices and pin configurations are globally or externally declared
static const char *TAG = "State Machine";
static session_t session = {.count = 0, .session_state = false, .on_count = 0, .off_count = 0};
// State functions directly implement hardware interaction logic
void start_tower_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Initiating tower inner circulation process.");
  state_dto->state = STATE_START_TOWER;
  state_dto->error = ESP_OK;
}

void session_check_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Verifying current session status.");
  state_dto->state = STATE_SESSION_CHECK;
  state_dto->arg_in = session.session_state;
  state_dto->error = ESP_OK;
}

void start_new_session_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Starting new session. Turning on the Tower inner Flow. ");
  state_dto->state = STATE_START_NEW_SESSION;
  session.session_state = true;
  session.count = 1;
  session.on_count = 1;
  session.off_count = 1;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_INNER_PUMP, PUMP_ON);
}

void on_period_checking_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Verifying if the on period is complete.");
  state_dto->state = STATE_ON_PERIOD_CHECKING_STATE;
  state_dto->arg_in = session.on_count > session.count;
  state_dto->error = ESP_OK;
}

void no_op_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Proceeding with no-operation state.");
  state_dto->state = STATE_NO_OP;
  session.count++;
  state_dto->error = ESP_OK;
}

void tower_off_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Turning off the Tower inner Flow.");
  state_dto->state = STATE_TURN_OFF_TOWER;
  state_dto->error = xl9535_set_pin_output(&xl9535_dev, TOWER_INNER_PUMP, PUMP_OFF);
}

void off_period_checking_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Verifying if the off period is complete.");
  state_dto->state = STATE_OFF_PERIOD_CHECKING_STATE;
  state_dto->arg_in = (session.on_count + session.off_count) > (session.count + 1);
  state_dto->error = ESP_OK;
}

void session_clear_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Clearing session");
  state_dto->state = STATE_SESSION_CLEAR;
  session.session_state = false;
}

void end_tower_state(state_dto_t *state_dto) {
  ESP_LOGI(TAG, "Period processing completed successfully.");
  state_dto->state = STATE_END_TOWER;
  state_dto->error = ESP_OK;
}

void error_tower_state(state_dto_t *state_dto) {
  ESP_LOGE(TAG, "An error occurred. State ID: %d, Error: %s", state_dto->state, esp_err_to_name(state_dto->error));
  state_dto->state = STATE_ERROR_TOWER;
  state_dto->error = ESP_OK;
}
