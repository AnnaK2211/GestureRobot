#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "Motors.h"

const int IN1_PIN = 27;
const int IN2_PIN = 26;
const int IN3_PIN = 25;
const int IN4_PIN = 33;

const int EN1_PIN = 18;
const int EN2_PIN = 19;

const int LEFT_WHEEL_SPEED = 175;
const int RIGHT_WHEEL_SPEED = 165;

const unsigned long COMMAND_TIMEOUT = 1000;
unsigned long lastCommandTime = 0;

enum Command : char
{
  STOP = 'S',
  FORWARD = 'F',
  BACKWARD = 'B',
  LEFT = 'L',
  RIGHT = 'R'
};

Command command = STOP;

Motors motors(IN1_PIN, IN2_PIN, IN3_PIN, IN4_PIN, EN1_PIN, EN2_PIN);

void onReceive(const uint8_t *mac, const uint8_t *data, int len)
{
  if (len == sizeof(Command))
  {
    command = (Command)data[0];
    lastCommandTime = millis();
    Serial.print(F("Command = "));
    Serial.println(command);
  }
}

void setup()
{
  Serial.begin(9600);

  motors.begin();
  motors.setSpeed(LEFT_WHEEL_SPEED, RIGHT_WHEEL_SPEED);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println(F("ESP-NOW init FAIL"));
    return;
  }

  esp_now_register_recv_cb(onReceive);
}

void loop()
{
  if (millis() - lastCommandTime > COMMAND_TIMEOUT)
  {
    command = STOP;
  }

  switch (command)
  {
  case FORWARD:
    motors.forward();
    break;
  case BACKWARD:
    motors.backward();
    break;
  case LEFT:
    motors.left();
    break;
  case RIGHT:
    motors.right();
    break;
  case STOP:
    motors.stop();
    break;
  }
}