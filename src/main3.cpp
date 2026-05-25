#include "irsensors.h"
#include "motors.h"
#include "ultrasonic.h"
#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  setupultrasonic();
  setupirsensors();
  setupmotors();
}

void loop() {
  long leftDistance = getleftdistance();
  delay(50);
  long rightDistance = getrightdistance();
  if (leftDistance > 15 && rightDistance > 15) {
    moveforward();
    Serial.println("moving forward");
    delay(500);
  } else if (leftDistance < 15 && rightDistance < 15) {
    stopmotors();
    delay(200);
    movebackward();
    Serial.println("moving backwards");
    delay(150);
  } else if (rightDistance <= 15 && leftDistance >= 15) {
    stopmotors();
    delay(200);
    turnleft();
    Serial.println("turning left");
    delay(500);
  } else if (leftDistance <= 15 && rightDistance >= 15) {
    stopmotors();
    delay(200);
    turnright();
    Serial.println("turning right.");
    delay(500);
  }
  Serial.print("Left: ");
  Serial.print(leftDistance);
  Serial.print(" cm    Right: ");
  Serial.print(rightDistance);
  Serial.println(" cm");
  delay(200);
}
