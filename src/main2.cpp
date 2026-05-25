#include "motors.h"
#include "ultrasonic.h"
#include <Arduino.h>

const long SAFE_DISTANCE = 15;
const int FORWARD_SPEED = 200;
const int TURN_SPEED = 180;

const unsigned long TURN_DURATION = 300;
const unsigned long BACKUP_DURATION = 300;
const unsigned long SECOND_TURN_DURATION = 400;

enum Action {
  ACTION_NONE,
  ACTION_FORWARD,
  ACTION_TURN_RIGHT,
  ACTION_TURN_LEFT,
  ACTION_BACKWARD
};

Action currentAction = ACTION_NONE;
unsigned long actionEnd = 0;
Action queuedAction = ACTION_NONE;
unsigned long queuedDuration = 0;

void startAction(Action a, unsigned long duration = 0) {
  currentAction = a;
  if (duration > 0)
    actionEnd = millis() + duration;
  else
    actionEnd = 0;

  switch (a) {
  case ACTION_FORWARD:
    moveforward(FORWARD_SPEED, FORWARD_SPEED);
    break;
  case ACTION_TURN_RIGHT:
    turnright(TURN_SPEED, TURN_SPEED);
    break;
  case ACTION_TURN_LEFT:
    turnleft(TURN_SPEED, TURN_SPEED);
    break;
  case ACTION_BACKWARD:
    movebackward(180, 180);
    break;
  case ACTION_NONE:
  default:
    stopmotors();
    break;
  }
}

void setup() {
  Serial.begin(9600);
  setupmotors();
  setupultrasonic();
  stopmotors();
  currentAction = ACTION_NONE;
  queuedAction = ACTION_NONE;
}

void loop() {
  unsigned long now = millis();

  if (actionEnd != 0 && now < actionEnd) {
    return;
  }

  if (actionEnd != 0 && now >= actionEnd) {
    actionEnd = 0;
    if (queuedAction != ACTION_NONE) {
      startAction(queuedAction, queuedDuration);
      queuedAction = ACTION_NONE;
      queuedDuration = 0;
      return;
    } else {
      startAction(ACTION_NONE);
    }
  }

  long left = getleftdistance();
  long right = getrightdistance();

  if (left <= 0)
    left = 300;
  if (right <= 0)
    right = 300;

  Serial.print("Left: ");
  Serial.print(left);
  Serial.print(" cm  Right: ");
  Serial.print(right);
  Serial.println(" cm");

  if (left > SAFE_DISTANCE && right > SAFE_DISTANCE) {
    if (currentAction != ACTION_FORWARD)
      startAction(ACTION_FORWARD);
  } else if (left <= SAFE_DISTANCE && right > SAFE_DISTANCE) {
    startAction(ACTION_TURN_RIGHT, TURN_DURATION);
  } else if (right <= SAFE_DISTANCE && left > SAFE_DISTANCE) {
    startAction(ACTION_TURN_LEFT, TURN_DURATION);
  } else {
    startAction(ACTION_BACKWARD, BACKUP_DURATION);
    queuedAction = ACTION_TURN_RIGHT;
    queuedDuration = SECOND_TURN_DURATION;
  }
}
