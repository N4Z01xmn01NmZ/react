#include <stdlib.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "freertos/FreeRTOS.h"

static const char *s_TAG = "RE-NOW";

// Sender MAC address
static uint8_t s_markedAddr[ESP_NOW_ETH_ALEN] = {0xF0, 0x08, 0xD1, 0xD4, 0x09, 0xD8};
// Reciever MAC address
static uint8_t s_unmarkedAddr[ESP_NOW_ETH_ALEN] = {0xF0, 0x08, 0xD1, 0xD4, 0xEC, 0x04};

static void react_on_send_cb(void);
static void react_espnow_task(void *pvParameter);
static void react_espnow_deinit(void);

void app_main()
{
    /* Initialize Non Volatile Storage */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize WiFi */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t wifiInitConfig = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifiInitConfig));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Initialize ESPNow */
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(react_on_send_cb));

    /* Adding peer info to peer list */
    esp_now_peer_info_t *peerInfo = malloc(sizeof(esp_now_peer_info_t));
    if (peerInfo == NULL) {
        ESP_LOGE(s_TAG, "Peer info heap allocation failed");
        return ESP_FAIL;
    }
    memset(peerInfo, 0, sizeof(esp_now_peer_info_t));
    memcpy(peerInfo->peer_addr, s_unmarkedAddr, ESP_NOW_ETH_ALEN);
    peerInfo->channel = 0;
    peerInfo->encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(peerInfo));
    free(peerInfo);

    /* List of task to be run */
    xTaskCreate(react_espnow_task, "react_espnow_task", 2048, NULL, 4, NULL);
}

void on_send_cb(void)
{}

void react_espnow_task(void *pvParameter)
{
    int message = 10;
    esp_err_t ret = esp_now_send(s_unmarkedAddr, (uint8_t*)&message, sizeof(message));
    if (ret != ESP_OK) {
        ESP_LOGE(s_TAG, "Error sending message");
        react_espnow_deinit();
        vTaskDelete(NULL);
    }
}

void react_espnow_deinit(void)
{
    /* Free used memory and shutdown ESPNow */
    esp_now_deinit();
}