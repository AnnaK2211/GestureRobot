#include <Arduino.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <esp_now.h>
#include <WiFi.h>

const int SCL_PIN = 16;
const int SDA_PIN = 17;

uint8_t SPIDER_MAC[] = {0x34, 0x98, 0x7A, 0xBB, 0xC1, 0x54};
uint8_t ROBOCAR_MAC[] = {0x34, 0x98, 0x7A, 0xBC, 0xB4, 0x88};

const float DEAD_ZONE = 7.0f;
const float THRESHOLD = 15.0f;
const float FILTER_ALPHA = 0.2f;

const unsigned long SEND_INTERVAL = 200;
unsigned long lastSendTime = 0;

enum Command : char
{
  STOP = 'S',
  FORWARD = 'F',
  BACKWARD = 'B',
  LEFT = 'L',
  RIGHT = 'R'
};

Command command = STOP;
Command lastCommand = STOP;

MPU6050 mpu;
int16_t ax, ay, az;
float pitch = 0.0f;
float roll = 0.0f;

esp_now_peer_info_t peerInfo;

float filter(float previous, float current)
{
  return previous * (1.0 - FILTER_ALPHA) + current * FILTER_ALPHA;
}

Command determineCommand(float pitch, float roll)
{
  if (abs(pitch) < DEAD_ZONE && abs(roll) < DEAD_ZONE)
    return STOP;
  else if (pitch > THRESHOLD)
    return FORWARD;
  else if (pitch < -THRESHOLD)
    return BACKWARD;
  else if (roll < -THRESHOLD)
    return LEFT;
  else if (roll > THRESHOLD)
    return RIGHT;
  else
    return STOP;
}

void onSent(const uint8_t *mac, esp_now_send_status_t status)
{
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? F("Sent OK") : F("Send FAIL"));
}

void setup()
{
  Serial.begin(9600);
  Wire.begin(SDA_PIN, SCL_PIN);

  mpu.initialize();
  Serial.println(mpu.testConnection() ? F("MPU6050 OK") : F("MPU6050 FAIL"));

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println(F("ESP-NOW init FAIL"));
    return;
  }

  esp_now_register_send_cb(onSent);

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  memcpy(peerInfo.peer_addr, ROBOCAR_MAC, sizeof(ROBOCAR_MAC));
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
    Serial.println(F("Failed to add ROBOCAR"));

  memcpy(peerInfo.peer_addr, SPIDER_MAC, sizeof(SPIDER_MAC));
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
    Serial.println(F("Failed to add SPIDER"));

  Serial.println(F("ESP-NOW ready"));
}

void loop()
{
  mpu.getAcceleration(&ax, &ay, &az);

  float newPitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  float newRoll = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;

  pitch = filter(pitch, newPitch);
  roll = filter(roll, newRoll);

  command = determineCommand(pitch, roll);

  if (command != lastCommand || millis() - lastSendTime > SEND_INTERVAL)
  {
    esp_now_send(ROBOCAR_MAC, (uint8_t *)&command, sizeof(command));
    esp_now_send(SPIDER_MAC, (uint8_t *)&command, sizeof(command));
    lastCommand = command;
    lastSendTime = millis();
  }

  Serial.print(F("Pitch="));
  Serial.print(pitch);
  Serial.print(F(" Roll="));
  Serial.print(roll);
  Serial.print(F(" Command="));
  Serial.println(command);
}