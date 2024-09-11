#include "state_machine.h"
#include "esp_log.h"
#include "state_functions.h"


// Array of state functions
static state_function_t state_functions[] = {
    start_state,
    turn_on_main_tank_flow_state,
    check_main_tank_max_state,
    turn_off_main_tank_flow_state,
    turn_on_tower_flow_state,
    check_all_towers_filled_state,
    turn_off_tower_flow_state,
    check_main_tank_min_state,
    start_refill_main_tank_state,
    check_main_tank_mid_state,
    stop_refill_main_tank_state,
    correction_state,
    end_state,
    error_state
};

// State machine task that runs the state transitions
_Noreturn void state_machine_task(void *param) {
  uint8_t current_state = STATE_START;
  state_dto_t state_dto;
  state_dto.state = STATE_START;
  state_dto.arg_in = 0;
  state_dto.error = ESP_OK;

  while (1) {
    state_functions[current_state](&state_dto);
    if (state_dto.error != ESP_OK) {
      current_state = STATE_ERROR;
    } else {
      switch (current_state) {
        case STATE_START:
          current_state = STATE_TURN_ON_MAIN_TANK_FLOW;
          break;
        case STATE_TURN_ON_MAIN_TANK_FLOW:
          current_state = STATE_CHECK_MAIN_TANK_MAX;
          break;
        case STATE_CHECK_MAIN_TANK_MAX:
          current_state = (!state_dto.arg_in) ? STATE_TURN_OFF_MAIN_TANK_FLOW : STATE_CHECK_MAIN_TANK_MAX;
          break;
        case STATE_TURN_OFF_MAIN_TANK_FLOW:
          current_state = STATE_TURN_ON_TOWER_FLOW;
          break;
        case STATE_TURN_ON_TOWER_FLOW:
          current_state = STATE_CHECK_ALL_TOWERS_FILLED;
          break;
        case STATE_CHECK_ALL_TOWERS_FILLED:
          current_state = (state_dto.arg_in) ? STATE_TURN_OFF_TOWER_FLOW : STATE_CHECK_ALL_TOWERS_FILLED;
          break;
        case STATE_TURN_OFF_TOWER_FLOW:
          current_state = STATE_CHECK_MAIN_TANK_MIN;
          break;
        case STATE_CHECK_MAIN_TANK_MIN:
          current_state = (!state_dto.arg_in) ? STATE_CORRECTION :STATE_START_REFILL_MAIN_TANK ;
          break;
        case STATE_START_REFILL_MAIN_TANK:
          current_state = STATE_CHECK_MAIN_TANK_MID;
          break;
        case STATE_CHECK_MAIN_TANK_MID:
          current_state = (!state_dto.arg_in) ? STATE_STOP_REFILL_MAIN_TANK : STATE_CHECK_MAIN_TANK_MID;
          break;
        case STATE_STOP_REFILL_MAIN_TANK:
          current_state = STATE_CORRECTION;
          break;
        case STATE_CORRECTION | STATE_ERROR:
          current_state = STATE_END;
          break;
        case STATE_END:
          vTaskDelete(NULL);  // End the task
          break;
        default:
          current_state = STATE_END;
          break;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));  // Delay between transitions
  }
}
