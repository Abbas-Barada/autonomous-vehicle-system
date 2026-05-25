#include "motors.h"
#include <Arduino.h>

const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 8;
const int IN4 = 11;
const int ENA = 5;
const int ENB = 9;

static int constrainSpeed(int speed) { return constrain(speed, 0, 255); }

void setupmotors() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void moveforward(int leftSpeed, int rightSpeed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, constrainSpeed(leftSpeed));
  analogWrite(ENB, constrainSpeed(rightSpeed));
}

void movebackward(int leftSpeed, int rightSpeed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, constrainSpeed(leftSpeed));
  analogWrite(ENB, constrainSpeed(rightSpeed));
}

void turnleft(int leftSpeed, int rightSpeed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, constrainSpeed(leftSpeed));
  analogWrite(ENB, constrainSpeed(rightSpeed));
}

void turnright(int leftSpeed, int rightSpeed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, constrainSpeed(leftSpeed));
  analogWrite(ENB, constrainSpeed(rightSpeed));
}

void stopmotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
