/* Lidar.h
  Declares the Lidar class, which provides functionality to read the PULSEDLIGHT LIDAR-Lite.
  Written by Jimmy C. Chau 2015 Mar 30 using code from PulsedLight as a reference:
    https://github.com/PulsedLight3D/LIDARLite_Basics/blob/master/Arduino/LIDARLite_I2C_Library_GetDistance_ContinuousRead/LIDARLite_I2C_Library_GetDistance_ContinuousRead.ino
*/

#ifndef LIDAR_H
#define LIDAR_H

// I2C configuration parameters (copied from Pulsed Light reference)
#define LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define RegisterMeasure     0x00          // Register to write to initiate ranging.
#define MeasureValue        0x04          // Value to initiate ranging.
#define RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

class Lidar {
  public:
  
    // Needs to be executed once in Arduino's setup() function.
    static void begin();
    
    // Attempts to return a new LIDAR distance reading;
    // If a new reading is not immediately available, returns the most recent one.
    static int getReading();
    
    // Returns a new LIDAR distance reading (blocks until complete)
    static int getNewReading();
  
  private:
    Lidar();
  
    // Send I2C message to request a new measurement
    // returns true if successful
    static bool triggerMeasurement();
    
    // Attempts to update int distance with a measurement from the LIDAR device
    // returns true if successful
    static bool updateReading();
  
    static bool requested;
    static int distance;
};

#endif
