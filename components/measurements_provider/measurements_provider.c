#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "measurements_provider.h"
#include "data_types.h"
#include "sensor_data_provider.h"

#define TAG "MEASUREMENTS"

static TimerHandle_t measurements_timer = NULL;
static QueueHandle_t measurements_queue = NULL;

static void measurements_timer_cb(TimerHandle_t xTimer)
{
    measurements_t data;

    if (sensor_get_temperature(&data.temperature) != ESP_OK)
    {
        data.temperature = 0.0f;
    }

    if (sensor_get_humidity(&data.humidity) != ESP_OK)
    {
        data.humidity = 0.0f;
    }

    if (sensor_get_pressure(&data.pressure) != ESP_OK)
    {
        data.pressure = 0.0f;
    }

    if (xQueueSend(measurements_queue, &data, 0) != pdPASS)
    {
        ESP_LOGW(TAG, "Failed to send measurements to queue");
    }
    else
    {
        ESP_LOGI(TAG, "Parameters measured: Temperature = %.2f °C, Humidity = %.2f %%, Pressure = %.2f hPa",
                 data.temperature, data.humidity, data.pressure);
    }
}

void start_measurements(const measurements_provider_config_t *config, QueueHandle_t queue)
{
    if (!config || !queue)
    {
        ESP_LOGE(TAG, "Invalid config or queue");
        return;
    }

    measurements_queue = queue;

    sensor_config_t sensor_cfg = {
        .scl_io = config->scl_io,
        .sda_io = config->sda_io};

    if (sensor_init(&sensor_cfg) != ESP_OK)
    {
        ESP_LOGE(TAG, "Sensor initialization failed");
        return;
    }

    measurements_timer = xTimerCreate(
        "MeasurementsTimer",
        pdMS_TO_TICKS(config->measurements_period_ms),
        pdTRUE,
        NULL,
        measurements_timer_cb);

    if (!measurements_timer)
    {
        ESP_LOGE(TAG, "Failed to create measurements timer");
        return;
    }

    if (xTimerStart(measurements_timer, 0) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to start measurements timer");
        xTimerDelete(measurements_timer, 0);
        measurements_timer = NULL;
        return;
    }

    ESP_LOGI(TAG, "Timer started with period %d ms", config->measurements_period_ms);
}