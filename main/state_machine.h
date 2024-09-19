#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <esp_err.h>
#include <stdint.h>
#include <freertos/semphr.h>

// Define the states for the state machine
typedef enum {
  STATE_START,
  STATE_TURN_ON_MAIN_TANK_FLOW,
  STATE_CHECK_MAIN_TANK_MAX,
  STATE_TURN_OFF_MAIN_TANK_FLOW,
  STATE_TURN_ON_TOWER_FLOW,
  STATE_CHECK_ALL_TOWERS_FILLED,
  STATE_TURN_OFF_TOWER_FLOW,
  STATE_CHECK_MAIN_TANK_MIN,
  STATE_START_REFILL_MAIN_TANK,
  STATE_CHECK_MAIN_TANK_MID,
  STATE_STOP_REFILL_MAIN_TANK,
  STATE_CORRECTION,
  STATE_END,
  STATE_ERROR  // Additional state to handle errors
} state_t;

extern SemaphoreHandle_t binarySemaphore;

// Function prototype for the state machine task
_Noreturn void state_machine_task(void *param);

#endif  // STATE_MACHINE_H
