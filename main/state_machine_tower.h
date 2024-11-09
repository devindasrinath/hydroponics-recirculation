#ifndef STATE_MACHINE_TOWER_H
#define STATE_MACHINE_TOWER_H

#include <esp_err.h>
#include <stdint.h>
#include "state_functions_tower.h"

#define START_NEW_TIMER_SESSION 0
#define ON_PERIOD_NEED_TO_CONTINUE 1
#define OFF_PERIOD_NEED_TO_CONTINUE 1
#define ON_PERIOD_SESSION 1

// Define the states for the state machine
typedef enum {
  STATE_START_TOWER,
  STATE_SESSION_CHECK,
  STATE_START_NEW_SESSION,
  STATE_NO_OP,
  STATE_TURN_OFF_TOWER,
  STATE_UPDATE_TOWER,
  STATE_SESSION_CLEAR,
  STATE_END_TOWER,
  STATE_ERROR_TOWER  // Additional state to handle errors
} state_tower_t;


// Function prototype for the state machine task
_Noreturn void tower_inner_circulation_task(void *param);

#endif  // STATE_MACHINE_TOWER_H
