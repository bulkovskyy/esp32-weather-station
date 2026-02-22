#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bme280.h"
#include "i2c_bus.h"
#include "sensor_data_provider.h"

#define TAG "SENSOR"

static i2c_bus_handle_t i2c_bus = NULL;
static bme280_handle_t bme280 = NULL;

esp_err_t sensor_init(const sensor_config_t *config)
{
    if (!config)
    {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = config->sda_io,
        .scl_io_num = config->scl_io,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_bus = i2c_bus_create(I2C_NUM_0, &i2c_config);
    if (!i2c_bus)
    {
        ESP_LOGE(TAG, "Failed to create I2C bus");
        return ESP_FAIL;
    }

    bme280 = bme280_create(i2c_bus, BME280_I2C_ADDRESS_DEFAULT);
    if (!bme280)
    {
        ESP_LOGE(TAG, "Failed to initialize BME280 sensor");
        return ESP_FAIL;
    }

    if (bme280_default_init(bme280) != ESP_OK)
    {
        ESP_LOGE(TAG, "BME280 default initialization failed");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Initialized successfully");
    return ESP_OK;
}

esp_err_t sensor_check_ready(void)
{
    if (!i2c_bus)
    {
        return ESP_ERR_INVALID_STATE;
    }
    if (!bme280)
    {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

esp_err_t sensor_get_temperature(float *temperature_c)
{
    if (sensor_check_ready() != ESP_OK || temperature_c == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (bme280_read_temperature(bme280, temperature_c) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read temperature");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t sensor_get_humidity(float *humidity_percent)
{
    if (sensor_check_ready() != ESP_OK || humidity_percent == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (bme280_read_humidity(bme280, humidity_percent) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read humidity");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t sensor_get_pressure(float *pressure_hpa)
{
    if (sensor_check_ready() != ESP_OK || pressure_hpa == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (bme280_read_pressure(bme280, pressure_hpa) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read pressure");
        return ESP_FAIL;
    }

    return ESP_OK;
}