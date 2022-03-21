#include <Arduino.h>
#include <WiFi.h>
#include "esp_now.h"

// Sender MAC address
uint8_t markedAddress[ESP_NOW_ETH_ALEN] {0xF0, 0x08, 0xD1, 0xD4, 0x09, 0xD8};
// Reciever MAC address
uint8_t unmarkedAddress[ESP_NOW_ETH_ALEN] {0xF0, 0x08, 0xD1, 0xD4, 0xEC, 0x04};

void OnDataRecieved(const uint8_t *mac_addr, const uint8_t *incoming_data, int len)
{
    Serial.print("Bytes recieved: ");
    Serial.println(len);
    Serial.print("Message: (");
    Serial.print(((int*)incoming_data)[0]);
    Serial.print(", ");
    Serial.print(((int*)incoming_data)[1]);
    Serial.println(")");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Enabling WiFi mode STA");
    WiFi.mode(WIFI_STA);
    Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());

    delay(5000);

    pinMode(LED_BUILTIN, OUTPUT);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Failed to initialize ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecieved);
}

void loop()
{
}