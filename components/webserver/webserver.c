// Input to HTML Server for ESP32
#include <stdio.h>
#include <sys/param.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "savedata.h"
#define EXAMPLE_ESP_WIFI_SSID "SetupTime"
#define EXAMPLE_ESP_WIFI_PASS "123456789"
#define EXAMPLE_ESP_WIFI_CHANNEL 1
#define EXAMPLE_MAX_STA_CONN 4
httpd_config_t config = HTTPD_DEFAULT_CONFIG();
httpd_handle_t server = NULL;
static const char *TAG = "ESP32 Server";
uint64_t current_timer_4 = 0;
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                .required = true,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}
const char resp[] = "<!DOCTYPE HTML><html><head>\
                            <title>ESP Input Form</title>\
                            <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
                            <style>\
                                body { padding: 20px; }\
                                .container { display: flex; flex-direction: column; gap: 20px; max-width: 800px; margin: auto; }\
                                .input, .history { background: white; border-radius: 8px; padding: 20px; box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1); }\
                                h1 { margin-bottom: 20px; font-size: 24px; }\
                                label { display: block; margin-bottom: 5px; }\
                                input { width: calc(100% - 20px); padding: 10px; border: 1px solid #ccc; border-radius: 4px; margin-bottom: 15px; }\
                                input:focus { border-color: #007bff; outline: none; }\
                                button { padding: 10px 15px; border: none; border-radius: 5px; background-color: #007bff; color: white; cursor: pointer; }\
                                button:hover { background-color: #0056b3; }\
                            </style>\
                            </head><body>\
                            <div class=\"container\">\
                            <h1>ESP Input Form</h1>\
                            <form action=\"/get\">\
                                <label for=\"time\">SET TIME:</label>\
                                <input type=\"text\" id=\"time\" name=\"str\">\
                                <input type=\"submit\" value=\"Submit\">\
                            </form><br>\
                            </div>\
                            </body></html>";
esp_err_t get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_handler_str(httpd_req_t *req)
{
    char *buf;
    size_t buf_len;

    // Read the URI line and get the parameters
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
    {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            ESP_LOGI(TAG, "Found URL query: %s", buf);
            char param[5];
            if (httpd_query_key_value(buf, "str", param, sizeof(param)) == ESP_OK)
            {
                ESP_LOGI(TAG, "The string value = %s", param);
                int time = 0;
                sscanf(param, "%u", &time);
                // string_convert_int(param);
                ESP_ERROR_CHECK(nvs_open("timer_4", NVS_READWRITE, &nvs_handle_timer_4));
                esp_err_t err = nvs_set_u64(nvs_handle_timer_4, "TIMER_4_KEY", time);
                if (err != ESP_OK)
                {
                    ESP_LOGE("NVS", "Error (%s) saving timer_4", esp_err_to_name(err));
                }
                ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_4));
                nvs_close(nvs_handle_timer_4);
                int test = esp_wifi_stop();
                ESP_LOGW("test", "%d", test);
                esp_err_t test_1 = httpd_stop(server);
                ESP_LOGW("test", "%d", test_1);
                // esp_restart();
            }
        }
        free(buf);
    }
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL};

httpd_uri_t uri_get_input = {
    .uri = "/get",
    .method = HTTP_GET,
    .handler = get_handler_str,
    .user_ctx = NULL};

httpd_handle_t start_webserver(void)
{

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_get_input);
    }
    return server;
}

// void init_data_4()
// {
//     esp_err_t err_timer_4 = nvs_open("timer_4", NVS_READONLY, &nvs_handle_timer_4);
//     err_timer_4 = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY", &current_timer_4);
//     if (err_timer_4 != ESP_OK)
//     {
//         ESP_LOGE("NVS", "Error reading timer_4: %s", esp_err_to_name(current_timer_4));
//     }
//     printf("Timer 4: %llu\n", current_timer_4);
//     nvs_close(nvs_handle_timer_4);
// }
void congfi_timer_4()
{
    wifi_init_softap();
    start_webserver();
}
