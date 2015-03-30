/* LDriver.h
  Declares the LDriver class to interact with the LAMP driver.
  Written by Jimmy C. Chau 2015 Mar 30
  Based on old LAMP software, which was jointly developed with Lucy Yan.
*/

#ifndef LDRIVER_H
#define LDRIVER_H

class LDriver {
  public:
  
  // Index values for each color (0 to 3)
  enum ColorIndex {RED,AMBER,GREEN,BLUE};

  // The brightness setting for each color
  int brightness[4];

  LDriver();
  
  // Turns all LEDs off or on
  void allOff();
  void allOn();
  
  // Set or get the PWM setting for a specific color
  // ci is a constant defined in enum ColorIndex.
  void set(int ci, int value);
  int get(int ci);
  
  // Re-set a color to it's setting defined in brightness[]
  // (Allows the more direct manipulations on brightness[] done by RGBAFade.)
  void reset(int ci);
  
  // Incrementally make a color brighter or darker
  void brighter(int ci);
  void darker(int ci);
  
  // Shows the 4 least-significant bits of code as binary on the LEDs
  // Useful for relaying status codes to the user without a computer terminal.
  // Does not alter the saved brightness[].
  void show(int code);
  
  private:
  // output pin for each color
  int pin[4];
};

#endif
