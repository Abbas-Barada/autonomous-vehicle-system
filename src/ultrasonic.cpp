#include "ultrasonic.h"
#include <Arduino.h>


const int Righttrig = 3;
const int Rightecho = 2;
const int Lefttrig = 10;
const int Leftecho = 4;

void setupultrasonic() {
  pinMode(Rightecho, INPUT);
  pinMode(Righttrig, OUTPUT);
  pinMode(Leftecho, INPUT);
  pinMode(Lefttrig, OUTPUT);
}

long getleftdistance() {
  digitalWrite(Lefttrig, LOW);
  delayMicroseconds(2);
  digitalWrite(Lefttrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Lefttrig, LOW);
  long duration = pulseIn(Leftecho, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return distance;
}

long getrightdistance() {
  digitalWrite(Righttrig, LOW);
  delayMicroseconds(2);
  digitalWrite(Righttrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Righttrig, LOW);
  long duration = pulseIn(Rightecho, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return distance;
}
