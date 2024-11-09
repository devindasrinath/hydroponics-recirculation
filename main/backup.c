// State machine logic
_Noreturn void main_recircultaion_task(void *pvParameters) {
  current_state= STATE_WAIT_1_MINUTE;
  while (1) {
    switch (current_state) {
      case STATE_START:
        current_state = STATE_TURN_ON_GRAVITY_VALVE;
        break;

      case STATE_TURN_ON_GRAVITY_VALVE:
        printf("Turning on gravity valve...\n");
        turn_on_off_gravity_valve(true);
        current_state = STATE_WAIT_3_MINUTES;
        break;

      case STATE_WAIT_3_MINUTES:
        printf("Waiting for 3 minutes...\n");
        wait_n_minutes(10);
        current_state = STATE_TURN_OFF_GRAVITY_VALVE;
        break;

      case STATE_TURN_OFF_GRAVITY_VALVE:
        printf("Turning off gravity valve...\n");
        turn_on_off_gravity_valve(false);
        current_state = STATE_WAIT_1_MINUTE;
        break;

      case STATE_WAIT_1_MINUTE:
        printf("Waiting for 1 minute...\n");
        wait_n_minutes(0.1);
        current_state = STATE_TURN_ON_ALL_TOWER_VALVE;
        break;

      case STATE_TURN_ON_ALL_TOWER_VALVE:
        printf("Turning on all tower valve...\n");
        turn_on_off_tower_valve(true);
        current_state = STATE_TURN_ON_TOWER_MOTOR;
        break;

      case STATE_TURN_ON_TOWER_MOTOR:
        printf("Turning on tower motor...\n");
        turn_on_off_tower_motor(true);
        current_state = STATE_WAIT_UNTIL_MAX_LEVEL;
        break;

      case STATE_WAIT_UNTIL_MAX_LEVEL:
        printf("Waiting until max level is reached...\n");
        wait_until_max_level();
        current_state = STATE_TURN_OFF_TOWER_MOTOR;
        break;

      case STATE_TURN_OFF_TOWER_MOTOR:
        printf("Turning off tower motor...\n");
        turn_on_off_tower_motor(false);
        current_state = STATE_TURN_OFF_ALL_TOWER_VALVE;
        break;

      case STATE_TURN_OFF_ALL_TOWER_VALVE:
        printf("Turning off all tower valve...\n");
        turn_on_off_tower_valve(false);
        current_state = STATE_WAIT_FINAL_1_MINUTE;
        break;

      case STATE_WAIT_FINAL_1_MINUTE:
        printf("Waiting for final 1 minute...\n");
        wait_n_minutes(0.1);
        current_state = STATE_DONE;
        break;

      case STATE_DONE:
        printf("Process complete!\n");
        // vTaskDelete(NULL);
        current_state = STATE_TURN_ON_GRAVITY_VALVE;
        break;

      default:
        break;
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Polling delay
  }
}

// Function implementations
void turn_on_off_gravity_valve(bool state) {
  // Placeholder for GPIO logic
  xl9535_set_pin_output(&xl9535_dev, GRAVITY_VALVE_RELAY_INDEX, state);
}

// Function implementations
void turn_on_off_tower_valve(bool state) {
  // Placeholder for GPIO logic
  xl9535_set_pin_output(&xl9535_dev, TOWER_VALVE_RELAY_INDEX, state);
}

// Function implementations
void turn_on_off_tower_motor(bool state) {
  // Placeholder for GPIO logic
  xl9535_set_pin_output(&xl9535_dev, TOWER_MOTOR_RELAY_INDEX, state);
}

void wait_until_max_level() {

  // Placeholder for sensor logic
  uint8_t pin_val;
  while(1) {
    pcf8575_read_pin(&pcf_dev, MID_DOWN_PIN, &pin_val);
    if(!pin_val){
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }

}


void wait_n_minutes(double n) {
  vTaskDelay(pdMS_TO_TICKS(n*60 * 1000)); // 1 minute delay
}