/****************************************************
  SMART MOUSE TRAP NOTIFIER
  -----------------------------------------
  ESP32 detects mouse using IR sensor and
  sends Telegram notification.

  Author: Student Project
****************************************************/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Prevent ESP32 reset when power unstable
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ===== WiFi Credentials =====
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ===== Telegram Bot Details =====
#define BOT_TOKEN "YOUR_BOT_TOKEN"
#define CHAT_ID   "YOUR_CHAT_ID"

// ===== Sensor Pin =====
#define IR_DO_PIN 27

// Sensor detection logic
const int DETECTED_STATE = LOW;

// Detection confirmation time
const unsigned long CONFIRM_MS = 1000;

// Cooldown between alerts
const unsigned long COOLDOWN_MS = 60000;

// Telegram client
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Timing variables
unsigned long detectedStart = 0;
unsigned long lastAlert = 0;


// Connect ESP32 to WiFi
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
}


// Setup runs once
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);

  pinMode(IR_DO_PIN, INPUT);

  client.setInsecure();

  connectWiFi();

  // Notify system ready
  bot.sendMessage(CHAT_ID, "Mouse trap armed.", "");
}


// Main loop
void loop() {

  // Reconnect WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  int sensorValue = digitalRead(IR_DO_PIN);
  unsigned long now = millis();

  // If mouse detected
  if (sensorValue == DETECTED_STATE) {

    if (detectedStart == 0)
      detectedStart = now;

    if ((now - detectedStart) > CONFIRM_MS) {

      if ((now - lastAlert) > COOLDOWN_MS) {

        lastAlert = now;

        bot.sendMessage(
          CHAT_ID,
          "Mouse detected in trap!",
          ""
        );

        detectedStart = 0;
      }
    }
  }
  else {
    detectedStart = 0;
  }

  delay(50);
}
