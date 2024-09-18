#include "wifi_manager.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_manager.h"

static const char *TAG = "WIFI_EXAMPLE";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  switch (event->event_id)
  {
    case SYSTEM_EVENT_STA_START:
      esp_wifi_connect();
      ESP_LOGI(TAG, "Trying to connect with Wi-Fi\n");
      break;

    case SYSTEM_EVENT_STA_CONNECTED:
      ESP_LOGI(TAG, "Wi-Fi connected\n");
      break;

    case SYSTEM_EVENT_STA_GOT_IP:
      ESP_LOGI(TAG, "got ip: starting MQTT Client\n");
      mqtt_app_start();
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      ESP_LOGI(TAG, "disconnected: Retrying Wi-Fi\n");
      esp_wifi_connect();
      break;

    default:
      break;
  }
  return ESP_OK;
}

void wifi_init(void)
{
  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

  wifi_config_t wifi_config = {
      .sta = {
          .ssid = "Dialog 4G 617",
          .password = "Ef5A93DF",
          .threshold.authmode = WIFI_AUTH_WPA2_PSK,
      },
  };
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}
