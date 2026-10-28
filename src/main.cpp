#include <esp_now.h>
#include <WiFi.h>
#include <string.h>

uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t heardMacs[20][6];
int heardCount = 0;

void onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  uint8_t* srcMac = info->src_addr;

  for (int i = 0; i < heardCount; i++) {
    if (memcmp(heardMacs[i], srcMac, 6) == 0) return;
  }

  if (heardCount < 20) {
    memcpy(heardMacs[heardCount], srcMac, 6);
    heardCount++;

    Serial.print("Heard board: ");
    for (int i = 0; i < 6; i++) {
      Serial.print(heardMacs[heardCount - 1][i], HEX);
      Serial.print(i < 5 ? ":" : "");
    }
    Serial.print(" | Total: ");
    Serial.println(heardCount);
  }
}

void onSent(const wifi_tx_info_t* info, esp_now_send_status_t status) {}

void setup() {
  Serial.begin(115200);
  delay(500);
  WiFi.mode(WIFI_STA);
  delay(100);

  uint8_t myMac[6];
  WiFi.macAddress(myMac);

  Serial.print("My MAC: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(myMac[i], HEX);
    Serial.print(i < 5 ? ":" : "");
  }
  Serial.println();
  Serial.println("--- Listening for other boards... ---");

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);
  esp_now_register_send_cb(onSent);

  esp_now_peer_info_t peer;
  memset(&peer, 0, sizeof(peer));
  memcpy(peer.peer_addr, broadcast, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
}

void loop() {
  uint8_t myMac[6];
  WiFi.macAddress(myMac);
  esp_now_send(broadcast, myMac, 6);
  delay(1000);
}   