/* RGBAFade.cpp
  Defines class RGBAFade.
  Written by Jimmy C. Chau 2015 Mar 30
*/

#include "RGBAFade.h"

RGBAFade::RGBAFade(byte keyA[6], byte keyB[4]) {
  // initialize RC4 PRNG
  // use the MAC address, IP address, and analog inputs as the key
  byte rc4_key[16];
  for(int i=0; i<6; i++)
    rc4_key[i] = keyA[i];
  for(int i=0; i<4; i++)
    rc4_key[i+6] = keyB[i];
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
  // not part of the RC4 PRNG
  for(int i=0; i<256; i++)
    nLUT[i] = ( i&0x80 + i&0x40 + i&0x20 + i&0x10 - i&0x08 - i&0x04 - i&0x02 - i&0x01 );
} // end constructor RGBAFade(byte keyA[6], byte keyB[4])

void RGBAFade::doFade(LDriver &leds) {
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
} //end void RGBAFade::doFade(LDriver &leds)

// returns a pseudorandom byte (using RC4)
byte RGBAFade::prng() {
  static int i=0, j=0;
  int temp;
  i = ( i + 1 ) % 256;
  j = ( j + rc4_S[i] ) % 256;
  temp = rc4_S[i];
  rc4_S[i] = rc4_S[j];
  rc4_S[j] = temp;
  return( rc4_S[ ( rc4_S[i] + rc4_S[j] ) % 256 ] );
}

