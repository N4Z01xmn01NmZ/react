#include <Arduino.h>
#include <WiFi.h>
#include "esp_now.h"

namespace react
{

    class ESPNowPeer
    {
    public:
        ESPNowPeer() = default;
        ~ESPNowPeer() = default;

        void setPeerInfo(const uint8_t *peer_addr);
        esp_now_peer_info_t getPeerInfo() const { return m_peerInfo; };
    private:
        esp_now_peer_info_t m_peerInfo;
    };

    void ESPNowPeer::setPeerInfo(const uint8_t *peer_addr)
    {
        memcpy(m_peerInfo.peer_addr, peer_addr, ESP_NOW_ETH_ALEN);
        m_peerInfo.channel = 0;
        m_peerInfo.encrypt = false;
    }

} // namespace react

// Sender MAC address
uint8_t markedAddress[ESP_NOW_ETH_ALEN] {0xF0, 0x08, 0xD1, 0xD4, 0x09, 0xD8};
// Reciever MAC address
uint8_t unmarkedAddress[ESP_NOW_ETH_ALEN] {0xF0, 0x08, 0xD1, 0xD4, 0xEC, 0x04};

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{

}

void setup()
{
    Serial.begin(115200);
    Serial.println("Enabling WiFi mode STA");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());

    pinMode(GPIO_NUM_34, INPUT);
    pinMode(GPIO_NUM_33, INPUT);

    delay(5000);

    pinMode(LED_BUILTIN, OUTPUT);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Failed to initialize ESP-NOW");
        return;
    }
    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo;
    memcpy(&peerInfo, unmarkedAddress, ESP_NOW_ETH_ALEN);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop()
{
    int joy[2];
    joy[0] = analogRead(GPIO_NUM_33);
    joy[1] = analogRead(GPIO_NUM_34);

    Serial.print("(");
    Serial.print(joy[0]);
    Serial.print(", ");
    Serial.print(joy[1]);
    Serial.println(")");

    int message {10};
    esp_err_t result = esp_now_send(unmarkedAddress, (uint8_t*)&joy, sizeof(int)*2);

    if (result != ESP_OK) {
        Serial.println("Failed to send message");
        delay(1000);
    }
    else {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1800);
    }
}