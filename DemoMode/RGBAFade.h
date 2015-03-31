/* RGBAFade.h
  Declares class RGBAFade, which provides the functionality for the fade demo mode.
  Written by Jimmy C. Chau 2015 Mar 30
*/

#ifndef RGBAFADE_H
#define RGBAFADE_H

// for the "byte" type
#include "Arduino.h"

// To receive a LDriver object to manipulate the colors.
#include "LDriver.h"

class RGBAFade {
  public:
    // let keyA be the MAC address and keyB = Ethernet.localIP()
    RGBAFade(byte keyA[6], byte keyB[4]);
  
    // Do one step of the RGBAFade loop
    void doFade(LDriver &leds);
  
  private:
    RGBAFade();
  
    // number of elements in fadeStepWait
    static const int QTYWAIT = 8;
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
    
    // generates a pseudo-random byte
    byte prng();
};

#endif
