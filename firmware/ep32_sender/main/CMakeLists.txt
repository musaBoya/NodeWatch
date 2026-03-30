
idf_component_register(
    SRCS "app_main.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_wifi
        esp_event
        nvs_flash
        esp_netif
        esp_http_client
        esp_timer
)