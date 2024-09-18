#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "esp_err.h"
#include "mqtt_client.h"

void mqtt_app_start(void);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
extern uint32_t MQTT_CONNECTED;
extern esp_mqtt_client_handle_t client;

#endif // MQTT_MANAGER_H
