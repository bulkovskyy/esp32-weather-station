#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "data_types.h"
#include "wifi_connector.h"
#include "web_server.h"
#include "measurements_provider.h"

#define TAG "MAIN"

void app_main(void)
{
    ESP_LOGI(TAG, "Starting");

    // Create a queue for sensor measurements
    QueueHandle_t queue = xQueueCreate(5, sizeof(measurements_t));
    if (!queue)
    {
        ESP_LOGE(TAG, "Failed to create measurements queue");
        return;
    }

    // WiFi configuration
    wifi_network_config_t wifi_config = {
        .ssid = "<ssid>",
        .pass = "<password>",
        .timeout_ms = 10000};

    // Web server configuration
    web_server_config_t web_config = {
        .measurements_path = "/measurements"};

    // Measurements provider configuration
    measurements_provider_config_t measurements_provider_config = {
        .sda_io = 25,
        .scl_io = 26,
        .measurements_period_ms = 3000};

    // Connect WiFi
    wifi_start(&wifi_config);

    // Start web server
    web_server_start(&web_config, queue);

    // Start the measurements timer
    start_measurements(&measurements_provider_config, queue);

    ESP_LOGI(TAG, "Initialized successfully");
}