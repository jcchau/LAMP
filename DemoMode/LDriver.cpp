/* LDriver.cpp
  Defines the LDriver class declared in LDriver.h.
  Written by Jimmy C. Chau 2015 Mar 30.
*/

#include "Arduino.h"
#include "LDriver.h"

LDriver::LDriver() {
  // Determined by hardware wiring.
  this->pin[RED] = 3;
  this->pin[AMBER] = 5;
  this->pin[GREEN] = 6;
  this->pin[BLUE] = 9;
  
  for(int i=0; i<4; i++) {
    this->brightness[i] = 255;
    analogWrite(this->pin[i], 255);
  }
}

void LDriver::allOff() {
  for(int i=0; i<4; i++) {
    this->set(i, 255);
  }
}

void LDriver::allOn() {
  for(int i=0; i<4; i++) {
    this->set(i, 0);
  }
}

// Skip bound-/error-checking on inputs; this is a simple embedded system.

void LDriver::set(int color, int value) {
  this->brightness[color] = value;
  analogWrite(this->pin[color], value);
}

int LDriver::get(int color) {
  return this->brightness[color];
}

void LDriver::reset(int color) {
  analogWrite(this->pin[color], this->brightness[color]);
}

void LDriver::brighter(int color) {
  if(this->brightness[color] > 0) {
    this->brightness[color]--;
    analogWrite(this->pin[color], this->brightness[color]);
  }
}

void LDriver::darker(int color) {
  if(this->brightness[color] < 255) {
    this->brightness[color]++;
    analogWrite(this->pin[color], this->brightness[color]);
  }
}

void LDriver::show(int code) {
  for(int i=0; i<4; i++) {
    int b = (code>>i) & 0x1;
    analogWrite(this->pin[i], b?239:255);
  }
}

