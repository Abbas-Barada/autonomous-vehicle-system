#include "irsensors.h"
#include "motors.h"
#include "ultrasonic.h"
#include <Arduino.h>

enum ActionType {
  ACTION_NONE,
  ACTION_FORWARD,
  ACTION_CURVE_LEFT,
  ACTION_CURVE_RIGHT,
  ACTION_TURN_LEFT,
  ACTION_TURN_RIGHT,
  ACTION_BACKWARD,
  ACTION_SEARCH_CURVE_RIGHT,
  ACTION_STOPPED,
};

struct ActionState {
  ActionType type;
  unsigned long endTime;
};

const long OBSTACLE_THRESHOLD_CM = 15;
const unsigned long ULTRASONIC_INTERVAL_MS = 50;
const unsigned long FORWARD_STEP_MS = 120;
const unsigned long TURN_CORRECT_MS = 120;
const unsigned long SEARCH_SPIN_MS = 320;
const unsigned long OBSTACLE_BACK_MS = 220;
const unsigned long OBSTACLE_TURN_MS = 360;
const unsigned long OBSTACLE_PAUSE_MS = 80;
const unsigned long DEBUG_INTERVAL_MS = 250;

const int LINE_SPEED = 180;
const int TURN_SPEED = 160;
const int TURN_CORRECT_SPEED = 140;
const int BACK_SPEED = 170;
const int AVOID_SPEED = 215;
const int SEARCH_SPEED = 150;

const bool IR_DETECTS_LOW = true;

const bool DIAGNOSTIC_MODE = false;

void runDiagnostics() {
  int lIR = readleftir();
  int rIR = readrightir();
  long ld = getleftdistance();
  delay(20);
  long rd = getrightdistance();

  Serial.print("DIAG LIR:");
  Serial.print(lIR);
  Serial.print(" RIR:");
  Serial.print(rIR);
  Serial.print(" | LD:");
  Serial.print(ld);
  Serial.print("cm RD:");
  Serial.print(rd);
  Serial.println("cm");

  moveforward(120, 120);
  delay(250);
  stopmotors();
  delay(150);
  movebackward(120, 120);
  delay(250);
  stopmotors();
  delay(150);
}

static ActionState currentAction = {ACTION_NONE, 0};
static unsigned long lastUltrasonicMs = 0;
static unsigned long lastDebugMs = 0;
static long currentLeftDistance = 1000;
static long currentRightDistance = 1000;

static int currentLeftSpeed = 0;
static int currentRightSpeed = 0;

bool isOnLine(int rawValue) {
  return IR_DETECTS_LOW ? (rawValue == LOW) : (rawValue == HIGH);
}

void setAction(ActionType action, int leftSpeed, int rightSpeed,
               unsigned long durationMillis) {
  currentLeftSpeed = leftSpeed;
  currentRightSpeed = rightSpeed;
  switch (action) {
  case ACTION_FORWARD:
    moveforward(leftSpeed, rightSpeed);
    break;
  case ACTION_CURVE_LEFT:
    moveforward(leftSpeed, rightSpeed);
    break;
  case ACTION_CURVE_RIGHT:
    moveforward(leftSpeed, rightSpeed);
    break;
  case ACTION_BACKWARD:
    movebackward(leftSpeed, rightSpeed);
    break;
  case ACTION_TURN_LEFT:
    turnleft(leftSpeed, rightSpeed);
    break;
  case ACTION_TURN_RIGHT:
    turnright(leftSpeed, rightSpeed);
    break;
  case ACTION_SEARCH_CURVE_RIGHT:
    moveforward(leftSpeed, rightSpeed);
    break;
  case ACTION_STOPPED:
    stopmotors();
    break;
  default:
    stopmotors();
    break;
  }
  currentAction.type = action;
  currentAction.endTime = millis() + durationMillis;
}

void updateUltrasonics() {
  unsigned long now = millis();
  if (now - lastUltrasonicMs >= ULTRASONIC_INTERVAL_MS) {
    currentLeftDistance = getleftdistance();
    delay(20);
    currentRightDistance = getrightdistance();
    lastUltrasonicMs = now;
  }
}

void debugStatus(int leftIR, int rightIR, bool leftOnLine, bool rightOnLine) {
  unsigned long now = millis();
  if (now - lastDebugMs < DEBUG_INTERVAL_MS) {
    return;
  }
  lastDebugMs = now;

  Serial.print("LD:");
  Serial.print(currentLeftDistance);
  Serial.print("cm RD:");
  Serial.print(currentRightDistance);
  Serial.print("cm | LIR:");
  Serial.print(leftIR);
  Serial.print(" RIR:");
  Serial.print(rightIR);
  Serial.print(" | LOn:");
  Serial.print(leftOnLine);
  Serial.print(" ROn:");
  Serial.print(rightOnLine);
  Serial.print(" | LS:");
  Serial.print(currentLeftSpeed);
  Serial.print(" RS:");
  Serial.print(currentRightSpeed);
  Serial.print(" | State:");
  switch (currentAction.type) {
  case ACTION_FORWARD:
    Serial.println("Forward");
    break;
  case ACTION_CURVE_LEFT:
    Serial.println("CurveLeft");
    break;
  case ACTION_CURVE_RIGHT:
    Serial.println("CurveRight");
    break;
  case ACTION_BACKWARD:
    Serial.println("Backward");
    break;
  case ACTION_TURN_LEFT:
    Serial.println("TurnLeft");
    break;
  case ACTION_TURN_RIGHT:
    Serial.println("TurnRight");
    break;
  case ACTION_SEARCH_CURVE_RIGHT:
    Serial.println("SearchCurve");
    break;
  case ACTION_STOPPED:
    Serial.println("Stopped");
    break;
  default:
    Serial.println("None");
    break;
  }
}

bool isObstacleDetected() {
  return currentLeftDistance <= OBSTACLE_THRESHOLD_CM ||
         currentRightDistance <= OBSTACLE_THRESHOLD_CM;
}

void handleObstacleAvoidance() {
  if (currentLeftDistance <= OBSTACLE_THRESHOLD_CM &&
      currentRightDistance <= OBSTACLE_THRESHOLD_CM) {
    setAction(ACTION_BACKWARD, BACK_SPEED, BACK_SPEED, OBSTACLE_BACK_MS);
    Serial.println("Avoidance: backing up");
    return;
  }

  if (currentRightDistance <= OBSTACLE_THRESHOLD_CM) {
    setAction(ACTION_TURN_LEFT, AVOID_SPEED, AVOID_SPEED, OBSTACLE_TURN_MS);
    Serial.println("Avoidance: turning left around obstacle");
    return;
  }

  if (currentLeftDistance <= OBSTACLE_THRESHOLD_CM) {
    setAction(ACTION_TURN_RIGHT, AVOID_SPEED, AVOID_SPEED, OBSTACLE_TURN_MS);
    Serial.println("Avoidance: turning right around obstacle");
    return;
  }
}

void chooseLineFollowAction(bool leftOnLine, bool rightOnLine) {
  if (leftOnLine && rightOnLine) {
    setAction(ACTION_FORWARD, LINE_SPEED, LINE_SPEED, FORWARD_STEP_MS);
    return;
  }

  if (leftOnLine && !rightOnLine) {
    setAction(ACTION_CURVE_LEFT, TURN_CORRECT_SPEED, LINE_SPEED,
              TURN_CORRECT_MS);
    return;
  }

  if (!leftOnLine && rightOnLine) {
    setAction(ACTION_CURVE_RIGHT, LINE_SPEED, TURN_CORRECT_SPEED,
              TURN_CORRECT_MS);
    return;
  }

  setAction(ACTION_SEARCH_CURVE_RIGHT, SEARCH_SPEED, TURN_CORRECT_SPEED,
            SEARCH_SPIN_MS);
}

void setup() {
  Serial.begin(9600);
  setupultrasonic();
  setupirsensors();
  setupmotors();
  stopmotors();
}

void loop() {
  unsigned long now = millis();
  if (DIAGNOSTIC_MODE) {
    runDiagnostics();
    return;
  }
  updateUltrasonics();

  int rawLeftIR = readleftir();
  int rawRightIR = readrightir();
  bool leftOnLine = isOnLine(rawLeftIR);
  bool rightOnLine = isOnLine(rawRightIR);

  if (isObstacleDetected()) {
    if (currentAction.type != ACTION_BACKWARD &&
        currentAction.type != ACTION_TURN_LEFT &&
        currentAction.type != ACTION_TURN_RIGHT) {
      stopmotors();
    }
    handleObstacleAvoidance();
    debugStatus(rawLeftIR, rawRightIR, leftOnLine, rightOnLine);
    return;
  }

  if (currentAction.endTime > now) {
    debugStatus(rawLeftIR, rawRightIR, leftOnLine, rightOnLine);
    return;
  }

  chooseLineFollowAction(leftOnLine, rightOnLine);
  debugStatus(rawLeftIR, rawRightIR, leftOnLine, rightOnLine);
}
