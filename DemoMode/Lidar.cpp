/* Lidar.cpp
  Defines the Lidar class.
  Written by Jimmy C. Chau 2015 Mar 30 using code from PulsedLight as a reference.
*/

// for the PulsedLight LIDAR-Lite Laser Module
// Arduino I2C Master Library from
//  http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library
#include <I2C.h>

#include "Lidar.h"

bool Lidar::requested;
int Lidar::distance;

void Lidar::begin() {
  // Initialize the LIDAR device (copied from PulsedLight reference)
  I2c.begin();
  delay(100);
  I2c.timeOut(50);

  // Make sure we have a valid reading in variable distance before continuing.
  getNewReading();
}

int Lidar::getReading() {
  // If a measurement has previously been requested, try to retrieve it
  if(requested) {
    updateReading();
  }
  
  // If the previous measurements have already been read out and we haven't successfully
  //  requested another measurement yet, try to do so.  
  if(!requested) {
    triggerMeasurement();
  }
  
  // return whatever's the most recent distance reading
  return(distance);
}

int Lidar::getNewReading() {
  // Keep trying until we trigger a new measurement
  while(!triggerMeasurement());
  
  // Keep trying until we successfully read the new measurement
  while(!updateReading());
  
  // Before we return, try to trigger the next measurement
  triggerMeasurement();
  
  // return the updated measurement (from the first two steps)
  return(distance);
}

// Attempt to request a new measurement
bool Lidar::triggerMeasurement() {
  delay(1);  // essential delay to prevent overpolling; LIDAR unit fails otherwise
  if(I2c.write(LIDARLite_ADDRESS,RegisterMeasure, MeasureValue) == 0) {
    requested = true;
    return(true);
  }
  else {
    return(false);
  }
}

// Attempt to read a measurement from the LIDAR unit.
// Sets flag requested to false once the requested measurement is read.
bool Lidar::updateReading() {
  byte distanceArray[2];
  delay(1);  // essential delay; LIDAR unit fails otherwise
  if(I2c.read(LIDARLite_ADDRESS, RegisterHighLowB, 2, distanceArray) == 0) {
    distance = (distanceArray[0] << 8) + distanceArray[1];
    //Serial.println(distance);
    requested = false;
    return(true);
  }
  else {
    return(false);
  }
}
