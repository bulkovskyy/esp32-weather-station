#ifndef WIFI_CONNECTOR_H
#define WIFI_CONNECTOR_H

#include <stdint.h>
#include "esp_err.h"

#define WIFI_MAX_SSID_LEN 32
#define WIFI_MAX_PASS_LEN 64

typedef struct {
    char ssid[WIFI_MAX_SSID_LEN];
    char pass[WIFI_MAX_PASS_LEN];
    uint32_t timeout_ms;
} wifi_network_config_t;

esp_err_t wifi_start(const wifi_network_config_t *config);

#endif