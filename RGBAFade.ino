/*
 RGBAFade
 
 Causes the RGBA lamp to cycle through all possible colors.
 
 Written by Jimmy C. Chau <jchau@bu.edu>
 */

/*
int led = 9;           // the pin that the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 1;    // how many points to fade the LED by
*/

int pin[4] = {3, 5, 6, 9};
int brightness[4] = {0, 63, 127, 191};
int fadeAmount[4] = {3, 5, 7, 11};

// the setup routine runs once when you press reset:
void setup()  { 
  /*
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
  */
  for(int i=0; i<4; i++)
    pinMode(pin[i], OUTPUT);
} 

// the loop routine runs over and over again forever:
void loop()  { 
  /*
  // set the brightness of pin 9:
  analogWrite(led, brightness);

  // set the brightness of the other pins
  analogWrite(3, brightness);
  analogWrite(5, brightness);
  analogWrite(6, brightness);
  
  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade: 
  if (brightness == 0 || brightness == 255) {
    fadeAmount = -fadeAmount ; 
  }     
  // wait for 30 milliseconds to see the dimming effect    
  delay(6);
  */
  for(int i=0; i<4; i++) {
    analogWrite(pin[i], brightness[i]);
    brightness[i] += fadeAmount[i];
    
    // if the brightness passes the boundaries, "bounce" off
    if(brightness[i] <= 0) {
      brightness[i] = -brightness[i];
      fadeAmount[i] = -fadeAmount[i];
    }
    else if(brightness[i] >= 255) {
      brightness[i] = 510-brightness[i];
      fadeAmount[i] = -fadeAmount[i];
    }
  }
  
  delay(30);
}

