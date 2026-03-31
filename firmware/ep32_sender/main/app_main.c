#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "app_config.h"

static const char *TAG = "NODEWATCH";

static void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {0};
    strcpy((char *)wifi_config.sta.ssid, NODEWATCH_WIFI_SSID);
    strcpy((char *)wifi_config.sta.password, NODEWATCH_WIFI_PASSWORD);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi baslatildi, baglaniliyor...");
    ESP_ERROR_CHECK(esp_wifi_connect());
}

static void send_telemetry(void)
{
    char post_data[256];

    int temp = 20 + (esp_random() % 100) / 10;
    int hum = 40 + (esp_random() % 100) / 10;

    snprintf(post_data, sizeof(post_data),
             "{"
             "\"device_id\":\"%s\","
             "\"timestamp\":%lld,"
             "\"temperature\":%d,"
             "\"humidity\":%d,"
             "\"status\":\"ok\""
             "}",
             NODEWATCH_DEVICE_ID,
             esp_timer_get_time() / 1000000,
             temp,
             hum);

    esp_http_client_config_t config = {0};
    config.url = NODEWATCH_SERVER_URL;
    config.method = HTTP_METHOD_POST;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "HTTP client olusturulamadi");
        return;
    }

    ESP_ERROR_CHECK(esp_http_client_set_header(client, "Content-Type", "application/json"));
    ESP_ERROR_CHECK(esp_http_client_set_post_field(client, post_data, strlen(post_data)));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);

        ESP_LOGI(TAG,
                 "POST basarili | status=%d content_length=%d payload=%s",
                 status_code,
                 content_length,
                 post_data);
    } else {
        ESP_LOGE(TAG, "POST hata: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "NodeWatch sender basliyor...");
    ESP_LOGI(TAG, "Device ID: %s", NODEWATCH_DEVICE_ID);
    ESP_LOGI(TAG, "Server URL: %s", NODEWATCH_SERVER_URL);
    ESP_LOGI(TAG, "Send interval: %d ms", NODEWATCH_SEND_INTERVAL_MS);

    wifi_init();

    vTaskDelay(pdMS_TO_TICKS(5000));

    while (1) {
        send_telemetry();
        vTaskDelay(pdMS_TO_TICKS(NODEWATCH_SEND_INTERVAL_MS));
    }
}