#include "state_machine_tower.h"
#include "esp_log.h"

// Array of state functions
static state_function_t state_functions[] = {
    start_tower_state,
    session_check_state,
    start_new_session_state,
    on_period_checking_state,
    no_op_state,
    tower_off_state,
    off_period_checking_state,
    session_clear_state,
    end_tower_state,
    error_tower_state
};

// State machine task that runs the state transitions
_Noreturn void state_machine_tower_task(void *param) {
  uint8_t current_state = STATE_START_TOWER;
  state_dto_t state_dto;
  state_dto.state = STATE_START_TOWER;
  state_dto.arg_in = 0;
  state_dto.error = ESP_OK;

  while (1) {
    state_functions[current_state](&state_dto);
    if (state_dto.error != ESP_OK) {
      current_state = STATE_ERROR_TOWER;
    } else {
      switch (current_state) {
        case STATE_START_TOWER:
          current_state = STATE_SESSION_CHECK;
          break;
        case STATE_SESSION_CHECK:
          current_state = (state_dto.arg_in) ? STATE_START_NEW_SESSION : STATE_ON_PERIOD_CHECKING_STATE;
          break;
        case STATE_START_NEW_SESSION:
          current_state = STATE_END_TOWER;
          break;
        case STATE_ON_PERIOD_CHECKING_STATE:
          current_state = (state_dto.arg_in) ? STATE_NO_OP : STATE_TURN_OFF_TOWER;
          break;
        case STATE_TURN_OFF_TOWER:
          current_state = STATE_OFF_PERIOD_CHECKING_STATE;
          break;
        case STATE_OFF_PERIOD_CHECKING_STATE:
          current_state = (!state_dto.arg_in) ? STATE_NO_OP :STATE_SESSION_CLEAR ;
          break;
        case STATE_SESSION_CLEAR | STATE_NO_OP | STATE_ERROR_TOWER:
          current_state = STATE_END_TOWER;
          break;
        case STATE_END_TOWER:
          vTaskDelete(NULL);  // End the task
          break;
        default:
          current_state = STATE_END_TOWER;
          break;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));  // Delay between transitions
  }
}
