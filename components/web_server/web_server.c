#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_http_server.h"
#include "web_server.h"
#include "data_types.h"

#define TAG "WEB"

typedef struct
{
    httpd_handle_t server;
    measurements_t measurements_data;
    QueueHandle_t queue;
    char measurements_path[WEB_SERVER_PATH_MAX];
} web_server_context_t;

static web_server_context_t web_ctx;

static void receive_measurements(void *params)
{
    web_server_context_t *ctx = (web_server_context_t *)params;

    while (true)
    {
        xQueueReceive(ctx->queue, &ctx->measurements_data, portMAX_DELAY);
    }
}

static esp_err_t measurements_url_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "New request received: %s", req->uri);

    cJSON *root = cJSON_CreateObject();
    if (!root)
    {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    char temp_str[16];
    char hum_str[16];
    char pres_str[16];

    snprintf(temp_str, sizeof(temp_str), "%.2f", web_ctx.measurements_data.temperature);
    snprintf(hum_str, sizeof(hum_str), "%.2f", web_ctx.measurements_data.humidity);
    snprintf(pres_str, sizeof(pres_str), "%.2f", web_ctx.measurements_data.pressure);

    cJSON_AddStringToObject(root, "temperature", temp_str);
    cJSON_AddStringToObject(root, "humidity", hum_str);
    cJSON_AddStringToObject(root, "pressure", pres_str);

    char *response = cJSON_PrintUnformatted(root);
    if (!response)
    {
        cJSON_Delete(root);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    esp_err_t res = httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    cJSON_free(response);
    cJSON_Delete(root);

    return res;
}

static esp_err_t default_url_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "New request received: %s", req->uri);
    return httpd_resp_sendstr(req, "OK");
}

static void init_server(const char *measurements_path)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(httpd_start(&web_ctx.server, &config));

    strncpy(web_ctx.measurements_path, measurements_path, WEB_SERVER_PATH_MAX - 1);
    web_ctx.measurements_path[WEB_SERVER_PATH_MAX - 1] = '\0';

    httpd_uri_t measurement_url = {
        .uri = web_ctx.measurements_path,
        .method = HTTP_GET,
        .handler = measurements_url_handler,
        .user_ctx = NULL};

    httpd_uri_t default_url = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = default_url_handler,
        .user_ctx = NULL};

    httpd_register_uri_handler(web_ctx.server, &measurement_url);
    httpd_register_uri_handler(web_ctx.server, &default_url);
}

void web_server_start(const web_server_config_t *config, QueueHandle_t queue)
{
    memset(&web_ctx, 0, sizeof(web_ctx));
    web_ctx.queue = queue;

    init_server(config->measurements_path);

    xTaskCreate(receive_measurements, "ReceiveMeasurements", 2048, &web_ctx, 1, NULL);

    ESP_LOGI(TAG, "Server started");
}