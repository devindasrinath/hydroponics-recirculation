#ifndef STATE_FUNCTIONS_H
#define STATE_FUNCTIONS_H

#include <esp_err.h>
#include "esp_log.h"
#include "constants.h"
#include "state_machine.h"

// Declare the function pointers for state functions
typedef void (*state_function_t)(state_dto_t *state_dto);

// External device structs
extern xl9535_t xl9535_dev;
extern pcf8575_t pcf_dev;

// State function declarations
void start_state(state_dto_t *state_dto);
void turn_on_main_tank_flow_state(state_dto_t *state_dto);
void check_main_tank_max_state(state_dto_t *state_dto);
void turn_off_main_tank_flow_state(state_dto_t *state_dto);
void turn_on_tower_flow_state(state_dto_t *state_dto);
void check_all_towers_filled_state(state_dto_t *state_dto);
void turn_off_tower_flow_state(state_dto_t *state_dto);
void check_main_tank_min_state(state_dto_t *state_dto);
void start_refill_main_tank_state(state_dto_t *state_dto);
void check_main_tank_mid_state(state_dto_t *state_dto);
void stop_refill_main_tank_state(state_dto_t *state_dto);
void correction_state(state_dto_t *state_dto);
void end_state(state_dto_t *state_dto);
void error_state(state_dto_t *state_dto);

#endif  // STATE_FUNCTIONS_H
