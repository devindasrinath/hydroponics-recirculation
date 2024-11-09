#ifndef STATE_FUNCTIONS_TOWER_H
#define STATE_FUNCTIONS_TOWER_H

#include <esp_err.h>
#include "esp_log.h"
#include "constants.h"
#include "state_machine_tower.h"

// Declare the function pointers for state functions
typedef void (*state_function_t)(state_dto_t *state_dto);

// External device structs
extern xl9535_t xl9535_dev;
extern pcf8575_t pcf_dev;

typedef struct {
  uint8_t count;
  uint8_t on_count;
  uint8_t off_count;
}session_t;

// State function declarations
void start_tower_state(state_dto_t *state_dto);
void get_session_type_state(state_dto_t *state_dto);
void start_new_session_state(state_dto_t *state_dto);
void no_op_state(state_dto_t *state_dto);
void tower_off_state(state_dto_t *state_dto);
void tower_update_state(state_dto_t *state_dto);
void session_clear_state(state_dto_t *state_dto);
void end_tower_state(state_dto_t *state_dto);
void error_tower_state(state_dto_t *state_dto);

#endif  // STATE_FUNCTIONS_TOWER_H
