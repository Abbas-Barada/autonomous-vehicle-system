#ifndef MOTORS_H
#define MOTORS_H

void setupmotors();
void moveforward(int leftSpeed = 255, int rightSpeed = 255);
void movebackward(int leftSpeed = 255, int rightSpeed = 255);
void turnleft(int leftSpeed = 255, int rightSpeed = 255);
void turnright(int leftSpeed = 255, int rightSpeed = 255);
void stopmotors();

#endif
