#include "state_machine_tower.h"
#include "esp_log.h"

// Array of state functions
static state_function_t state_functions[] = {
    start_tower_state,
    get_session_type_state,
    start_new_session_state,
    no_op_state,
    tower_off_state,
    tower_update_state,
    session_clear_state,
    end_tower_state,
    error_tower_state
};

// State machine task that runs the state transitions
_Noreturn void tower_inner_circulation_task(void *param) {
  uint8_t current_state = STATE_START_TOWER;
  state_dto_t state_dto;
  state_dto.state = STATE_START_TOWER;
  state_dto.arg_in = START_NEW_TIMER_SESSION;
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
          current_state = state_dto.arg_in; //get session type(new, no op, turn off ,end)
          break;
        case STATE_START_NEW_SESSION :
        case STATE_NO_OP :
        case STATE_TURN_OFF_TOWER :
          current_state = STATE_UPDATE_TOWER;
          break;
        case STATE_SESSION_CLEAR:
          current_state = STATE_SESSION_CHECK;
          break;
        case STATE_UPDATE_TOWER:
        case STATE_ERROR_TOWER:
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
