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

// For RGBAFade (fadeMode)
#include "RGBAFade.h"

// For LIDAR distance readings
#include <I2C.h>
#include "Lidar.h"

// *** Global vars for the regular demo (from completecode_Jimmy ***
// TODO: update this MAC address for each Arduino (with an Ethernet shield)
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0C, 0x00, 0x9F };

// Listen on port 23
EthernetServer server(23);

// Object to control the LAMP LEDs
LDriver leds;

// Object for RGBAFade mode
RGBAFade *fadeMachine;

// the current operating mode of the system
enum Demos {cmdMode, fadeMode, lidarMode} mode = lidarMode;

// whether the LIDAR system has a fresh measurement
bool lidarFresh = false;

// distance threshold
int distanceThreshold = 50;

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
  
  Lidar::begin();
  
  leds.show(4);  // startup indicator
  
  // initialize the RGBAFade fadeMachine
  // Pass it the MAC address and 4 bytes of the IP address to initialize the RC4 PRNG
  byte ipaddr[4];
  for(int i=0; i<4; i++)
    ipaddr[4] = Ethernet.localIP()[i];
  fadeMachine = new RGBAFade(mac, ipaddr);
      
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
        fadeMachine->doFade(leds);
        break; // end case fadeMode
      
      case lidarMode:
        if(Lidar::getReading() < distanceThreshold) {
          leds.brighter(LDriver::GREEN);
          leds.darker(LDriver::RED);
        }
        else {
          leds.brighter(LDriver::RED);
          leds.darker(LDriver::GREEN);
        } // end else of if(distance < distanceThreshold)
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

