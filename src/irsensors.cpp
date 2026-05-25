#include "irsensors.h"
#include <Arduino.h>


const int leftir = A0;
const int rightir = A1;

void setupirsensors() {
  pinMode(rightir, INPUT);
  pinMode(leftir, INPUT);
}

int readrightir() { return digitalRead(rightir); }

int readleftir() { return digitalRead(leftir); }
