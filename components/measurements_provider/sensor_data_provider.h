#ifndef SENSOR_DATA_PROVIDER_H
#define SENSOR_DATA_PROVIDER_H

#include <stdint.h>
#include "esp_err.h"

typedef struct {
    int scl_io;  // I2C clock pin
    int sda_io;  // I2C data pin
} sensor_config_t;

esp_err_t sensor_init(const sensor_config_t *config);

esp_err_t sensor_get_temperature(float *temperature_c);
esp_err_t sensor_get_humidity(float *humidity_percent);
esp_err_t sensor_get_pressure(float *pressure_hpa);

#endif