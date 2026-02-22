#ifndef MEASUREMENTS_PROVIDER_H
#define MEASUREMENTS_PROVIDER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef struct {
    int sda_io;                   // I2C data pin for sensor
    int scl_io;                   // I2C clock pin for sensor
    int measurements_period_ms;   // Measurement period in milliseconds
} measurements_provider_config_t;

void start_measurements(const measurements_provider_config_t *config, QueueHandle_t queue);

#endif