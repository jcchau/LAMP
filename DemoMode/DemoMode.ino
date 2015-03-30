/* DemoMode
  Demonstration and operating software for LAMP
  Original code (completecode_Jimmy) by Jimmy C. Chau and Lucy T. Yan.
  RGBAFade demo mode added and merged by Jimmy C. Chau <jchau@bu.edu> 2013 Apr 04.
  LIDAR module code added by Jimmy Chau 2015 Mar 25,
    using code from PulsedLight as a reference:
    https://github.com/PulsedLight3D/LIDARLite_Basics/blob/master/Arduino/LIDARLite_I2C_Library_GetDistance_ContinuousRead/LIDARLite_I2C_Library_GetDistance_ContinuousRead.ino
  Thanks to Yuting Zhang for her help with adding the LIDAR device.
*/

// for the Ethernet Shield
#include <SPI.h>
#include <Ethernet.h>

// For the LAMP driver
#include "LDriver.h"

// for the PulsedLight LIDAR-Lite Laser Module
// Arduino I2C Master Library from
//  http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library
#include <I2C.h>
// I2C configuration parameters (copied from Pulsed Light reference)
#define LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define RegisterMeasure     0x00          // Register to write to initiate ranging.
#define MeasureValue        0x04          // Value to initiate ranging.
#define RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

// *** Global vars for the regular demo (from completecode_Jimmy ***
// TODO: update this MAC address for each Arduino (with an Ethernet shield)
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0C, 0x00, 0x9F };

// Listen on port 23
EthernetServer server(23);

// Object to control the LAMP LEDs
LDriver leds;

// *** Global vars for RGBAFade ***
// number of elements in fadeStepWait
const int QTYWAIT = 8;
// (relatively prime) wait time between incrementing each color
// the first 4 entries are the curently chosen values for the 4 colors
// avoid factors of 255 (potential periodicities with bouncing)
int fadeStepWait[QTYWAIT] = {29, 19, 11, 7, 13, 23, 31, 37};
// stores the increment and the direction of fade (brighter or dimmer)
int fadeIncrement[4] = {1, 1, 1, 1};
// after a while, rotate the fadeStepWait array, so that one color isn't always slowest
long int rotateAfter = 3600000;  // after approximately an hour
long int untilRotate = 0;  // immediately rotate

// for PRNG (to add noise to increase the likelihood of eventually reaching every color combination)
byte rc4_S[256];

// the noise look-up table
byte nLUT[256];

// the current operating mode of the system
enum Demos {cmdMode, fadeMode, lidarMode} mode = lidarMode;

// whether the LIDAR system has a fresh measurement
bool lidarFresh = false;

// distance threshold
int distanceThreshold = 50;

// generates a pseudo-random byte
byte prng();

void setup() {  
  leds.show(1);  // startup indicator
  
  // Start the serial port for debugging
  Serial.begin(9600);
  Serial.println("LAMP Controller v.2015-03-30.");

  leds.show(2);  // startup indicator

  // Start networking
  Ethernet.begin(mac);
  server.begin();
  Serial.print("Server address: ");
  Serial.println(Ethernet.localIP());
  
  leds.show(3);  // startup indicator
  
  // Initialize the LIDAR device (copied from PulsedLight reference)
  I2c.begin();
  delay(100);
  I2c.timeOut(50);
  
  leds.show(4);  // startup indicator
  
  // initialize RC4 PRNG
  // use the MAC address, IP address, and analog inputs as the key
  byte rc4_key[16];
  for(int i=0; i<6; i++)
    rc4_key[i] = mac[i];
  for(int i=0; i<4; i++)
    rc4_key[i+6] = Ethernet.localIP()[i];
  for(int i=0; i<6; i++)
    rc4_key[i+10] = analogRead(i) % 256;
  for(int i=0; i<256; i++)
    rc4_S[i]=i;
  int j=0, temp;
  for(int i=0; i<256; i++) {
    j = ( j + rc4_S[i] + rc4_key[i%16] ) % 256;
    temp = rc4_S[i];
    rc4_S[i] = rc4_S[j];
    rc4_S[j] = temp;
  }
  
  // initialize nLUT[]: maps random bytes to additive noise
  for(int i=0; i<256; i++)
    nLUT[i] = ( i&0x80 + i&0x40 + i&0x20 + i&0x10 - i&0x08 - i&0x04 - i&0x02 - i&0x01 );
    
  leds.show(5);  // startup indicator
  
  leds.allOff();  // in preparation for LIDAR mode
}

void loop() {
  EthernetClient client = server.available(); //gives you a client!
  
  if(client) {  // tests if any data from the client is available for reading
    // Await the client's command if the client is connected

    char command[7];  // allocate 7 chars for client commands
    int commandVal=0;  // stores the 3-digit value provided with some commands

    // clear command to receive the next one
    for(int i=0; i<7; i++)
      command[i] = '\0';
      
    for (int n=0; n<6; n++) {
      do {
        command[n] = client.read();
        // keep trying if we don't get a character
      } while(command[n] == -1);

      // once user hits "enter" button, we try and utilize command
      if (command[n] == '\n') { //if "enter" button has been pressed

        // We're receiving commands, so disable the RGBAFade demo mode
        mode = cmdMode;
    
        // enter has been pressed, so break from for loop to read additional charaters
        // into the command
        break;
      } // end if enter has been pressed
    } // end for loop that reads characters into the command.  
    
    // execute the received command
    switch (command[0]) {
      case 'r':
        leds.set(LDriver::RED, extractDecimal(command, 1, 3));
        break;
      case 'a':
        leds.set(LDriver::AMBER, extractDecimal(command, 1, 3));
        break;
      case 'g':
        leds.set(LDriver::GREEN, extractDecimal(command, 1, 3));
        break;
      case 'b':
        leds.set(LDriver::BLUE, extractDecimal(command, 1, 3));
        break;
      case 'q':
        // to quit and return to RGBAFade demo mode
        client.println("Returning to the RGBAFade demo until the next command.");
        // re-enable the RGBAFade demo
        mode = fadeMode;
        break;
      case 'l': // "el"
        // change to the LIDAR demo mode
        client.println("Switching to LIDAR demo mode unitl the next command.");
        leds.allOff();
        mode = lidarMode;
        break;
      default: 
        // values in the case of no input
        client.print("Red: ");
        client.println(leds.get(LDriver::RED));
        client.print("Green: ");
        client.println(leds.get(LDriver::GREEN));
        client.print("Blue: ");
        client.println(leds.get(LDriver::BLUE));
        client.print("Amber: ");
        client.println(leds.get(LDriver::AMBER));
    } // end received command execution switch statement
    
  } // end if(client)
  else {
    // if no data is available from the client
    
    switch(mode) {
      case fadeMode:
      // Run RGBAFade
      delay(1);
      
      untilRotate--;
      
      for(int i=0; i<4; i++) {
        if((untilRotate%fadeStepWait[i]) == 0) {
          // increment brightness and add noise
          leds.brightness[i] += fadeIncrement[i] + nLUT[prng()];
        
          // if the brightness passes the boundaries, "bounce" off
          if(leds.brightness[i] < 0) {
            leds.brightness[i] = -leds.brightness[i];
            fadeIncrement[i] = -fadeIncrement[i];
          }
          else if(leds.brightness[i] > 255) {
            leds.brightness[i] = 510-leds.brightness[i];
            fadeIncrement[i] = -fadeIncrement[i];
          }
          
          leds.reset(i);
        }
      }
      
      // rotate the fade values
      if(untilRotate <= 0) {
        untilRotate = rotateAfter;
        int toWrapAround = fadeStepWait[0];
        for(int i=0; i<QTYWAIT-1; i++)
          fadeStepWait[i] = fadeStepWait[i+1];
        fadeStepWait[QTYWAIT-1] = toWrapAround;
  
        // do a random swap
        int i = prng() % QTYWAIT;
        int j = prng() % QTYWAIT;
        int temp = fadeStepWait[i];
        fadeStepWait[i] = fadeStepWait[j];
        fadeStepWait[j] = temp;
      }
      break; // end case fadeMode
      
      case lidarMode:
      if(lidarFresh) {
        byte distanceArray[2];
        if(I2c.read(LIDARLite_ADDRESS, RegisterHighLowB, 2, distanceArray) == 0) {
          Serial.print("R");
          Serial.flush();
          lidarFresh = false;
          int distance = (distanceArray[0] << 8) + distanceArray[1];
          Serial.print("LIDAR distance: ");
          Serial.println(distance);
          Serial.flush();
          if(distance < distanceThreshold) {
            leds.brighter(LDriver::GREEN);
            leds.darker(LDriver::RED);
          }
          else {
            leds.brighter(LDriver::RED);
            leds.darker(LDriver::GREEN);
          } // end else of if(distance < distanceThreshold)
        } // end if(I2c.read(...) == 0)
        else {
          Serial.print("r");
          Serial.flush();
        }        
      } // end if(lidarFresh)
      else {
        Serial.print("t");
        Serial.flush();
        if(I2c.write(LIDARLite_ADDRESS,RegisterMeasure, MeasureValue) == 0) {
          lidarFresh = true;
        }
      } // end else of if(lidarFresh)
      
      delay(1); // wait 1 ms to prevent overpolling
      break; // end case lidarMode
    } // end switch(mode) 
  } // end else for if(client)
}

// Converts n decimal digits in string str, starting from start, into an integer
// If a non-decimal character is encountered, it stops processing characters and
//  returns the value so far.
// Note that the returned value may not necessarily be between 0 and 255.
int extractDecimal(char str[], int start, int n) {
  int value = 0;
  for(int i=0; i<n; i++) {
    int digit = str[start + i]-48;
    if(digit < 0 || digit > 9)
      break;
    value = value*10 + digit;
  }
  return(value);
}

// returns a pseudorandom byte (using RC4)
byte prng() {
  static int i=0, j=0;
  int temp;
  i = ( i + 1 ) % 256;
  j = ( j + rc4_S[i] ) % 256;
  temp = rc4_S[i];
  rc4_S[i] = rc4_S[j];
  rc4_S[j] = temp;
  return( rc4_S[ ( rc4_S[i] + rc4_S[j] ) % 256 ] );
}
