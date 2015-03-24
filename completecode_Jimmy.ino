#include <SPI.h>
#include <Ethernet.h>

//this retrieves an IP address via DHCP router and prints 
//out the server address.It also relays messages
//It then gets an input from the user and sets the values of the LEDs

//NOTES: Ethernet.begin returns NO value. Ethernet.localIP() does
//switching ports in the router (1 to 2) will give the same add

int ledred = 3; // red LEDs connenected to pin 3
int ledgreen = 6; // green LEDs connenected to pin 6
int ledblue = 9; // blue LEDs connenected to pin 9
// pin 11 is taken for the Ethernet shield; use 5 instead (Jimmy)
int ledyellow = 5; // yellow LEDs connenected to pin 11

 
byte mac[] = {
  0x90, 0xA2, 0xDA,0x0C, 0x00, 0xA3 };  //assigning mac address to arduino
  
EthernetServer server(23);
boolean alreadyConnected = false;
  
int r = 255;
int g = 255;
int b = 255;
int y = 255;
  
void setup() {
  //setting pins to output
  //pinMode(ledred, OUTPUT);
  //pinMode(ledgreen, OUTPUT);
  //pinMode(ledblue, OUTPUT);
  //pinMode(ledyellow, OUTPUT);
  //default values for leds, all on:
  analogWrite(ledred,r);
  analogWrite(ledgreen,g);
  analogWrite(ledblue,b);
  analogWrite(ledyellow,y);
  
  Ethernet.begin(mac);
  server.begin();
  Serial.begin(9600);
  Serial.print("Chat server address:");
  Serial.println(Ethernet.localIP());
}

void loop() {
  
  //****************************chat server**********************************
  EthernetClient client = server.available(); //gives you a client!
  
  if(client) {   // if statement makes sure there is a client to avoid error
    /*
    if (!alreadyConnected) {
      //client.flush();
      Serial.println("We have a new client");
      client.println("Hello,client");
      alreadyConnected = true;
    }
    */
    
    
    
    //***********************setting values to LEDs***************************
    int myval=0;
    char command[7];  //sets command to set length 7
    
    //erase function. creates clean slate each time command is
    //or ignored in order to recieve the next command
    for (int i = 0; i < 7; i++) {
      command[i] = '\0';
    }
    
    //storing the input from client into command
    for (int n=0; n<6; n++) {
      do {
        command[n] = client.read();
        Serial.print('.');
        // keep trying if we don't get a character
      } while(command[n] == -1);
      Serial.print("\nReceived: ");
      Serial.print(command[n]);
      Serial.print(", ");
      Serial.println(int(command[n]));
      
      // make sure everything gets printed before going into uncertain parts (Jimmy)
      Serial.flush();
      
      //once user hits "enter" button, we try and utilize command
      if (command[n] == '\n') { //if "enter" button has been pressed
        Serial.println("if statement has been called");
        Serial.flush();
        if (command[0] == 'r' || command[0] == 'g' || command[0] == 'b' || command[0] == 'y'  )
        {
          int val[3]; //myval will store our light intensity value 
          for (int k = 0; k < 3; k++) {
            val[k] = command[k + 1]-48;
            Serial.println(val[k]);
            // wait until val[0] through val[2] is set
            
          }
          myval = 100*val[0] + 10*val[1] + val[2];
          Serial.print("myval= ");
          Serial.println(myval); Serial.flush();
        }
        break; //at the end of the if statement, leave the for loop.
      }
    }
    
//  Serial.print("myval= ");
//  Serial.println(myval);

    
    //changing output of pins OR printing status
    switch (command[0]) {
      case 'r':
        r=myval;
        analogWrite(ledred, r);
        break;
      case 'g':
        b=myval;
        analogWrite(ledgreen, myval);
        break;
      case 'b':
        g=myval;
        analogWrite(ledblue, myval);
        break;
      case 'y':
        y=myval;
        analogWrite(ledyellow, myval);
        break;
      default: 
        // values in the case of no input
        client.print("Red: ");
        client.println(r);  //prints current values or default
        client.print("Green: ");
        client.println(g);  //prints current values or default
        client.print("Blue: ");
        client.println(b);  //prints current values or default
        client.print("Yellow: ");
        client.println(y);  //prints current values or default
    }
  }
}
