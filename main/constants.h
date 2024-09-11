#ifndef RECIRCULATION_BASIC_HYDROPHONICS_MAIN_CONSTANTS_H_
#define RECIRCULATION_BASIC_HYDROPHONICS_MAIN_CONSTANTS_H_

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "xl9535.h"
#include "pcf8575.h"

#define MAIN_TANK_PUMP 0
#define TOWER_VALVE 3
#define TOWER_PUMP 2
#define TOWER_INNER_PUMP 4
#define MAIN_TANK_REFILL_PUMP 5


#define MAIN_TANK_LEVEL_MAX_PIN 0
#define MAIN_TANK_LEVEL_MID_UP_PIN 1
#define MAIN_TANK_LEVEL_MID_DOWN_PIN 2
#define MAIN_TANK_LEVEL_MIN_PIN 3

#define TOWER_LEVEL_PIN 15

typedef struct {
  uint8_t state;
  esp_err_t error;
  uint8_t arg_in;
}state_dto_t;

// Define the types for pump state and tank level
typedef enum {
  PUMP_OFF,
  PUMP_ON
} pump_status_t;


typedef enum {
  VALVE_OFF,
  VALVE_ON
} valve_status_t;

//typedef enum {
//  MIN_LEVEL,
//  MID_HIGHER_LEVEL,
//  MID_LOWER_LEVEL,
//  MAX_LEVEL
//} main_tank_level_t;

#endif //RECIRCULATION_BASIC_HYDROPHONICS_MAIN_CONSTANTS_H_
