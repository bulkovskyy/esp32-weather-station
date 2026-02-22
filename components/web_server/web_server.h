#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define WEB_SERVER_PATH_MAX 128

typedef struct {
    char measurements_path[WEB_SERVER_PATH_MAX]; // Path to measurements endpoint
} web_server_config_t;

void web_server_start(const web_server_config_t *config, QueueHandle_t queue);

#endif