#ifndef STATE_MACHINE_TOWER_H
#define STATE_MACHINE_TOWER_H

#include <esp_err.h>
#include <stdint.h>
#include "state_functions_tower.h"

// Define the states for the state machine
typedef enum {
  STATE_START_TOWER,
  STATE_SESSION_CHECK,
  STATE_START_NEW_SESSION,
  STATE_ON_PERIOD_CHECKING_STATE,
  STATE_NO_OP,
  STATE_TURN_OFF_TOWER,
  STATE_OFF_PERIOD_CHECKING_STATE,
  STATE_SESSION_CLEAR,
  STATE_END_TOWER,
  STATE_ERROR_TOWER  // Additional state to handle errors
} state_tower_t;


// Function prototype for the state machine task
_Noreturn void state_machine_tower_task(void *param);

#endif  // STATE_MACHINE_TOWER_H
