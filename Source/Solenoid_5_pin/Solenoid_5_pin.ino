/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
int bytes[10][5] = {
//  {1, 0, 0, 0, 0},
//  {0, 1, 0, 0, 0},
//  {0, 0, 1, 0, 0},
//  {0, 0, 0, 1, 0},
//  {0, 0, 0, 0, 1},
//  {0, 0, 0, 0, 1},
//  {0, 0, 0, 1, 0},
//  {0, 0, 1, 0, 0},
//  {0, 1, 0, 0, 0},
//  {1, 0, 0, 0, 0},

  {0, 0, 0, 0, 0},
  {0, 1, 0, 1, 0},
  {1, 0, 1, 0, 1},
  {1, 0, 0, 0, 1},
  {0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0},
  {0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},

};

int pins[5] = { 13, 12, 11, 10, 9 };
int del = 40;
int delDelta = 0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  for(int pinCtr = 0 ; pinCtr < 5 ; pinCtr++) {
    pinMode(pins[pinCtr], OUTPUT);     
    digitalWrite(pins[pinCtr],LOW);
  }
}

// the loop routine runs over and over again forever:
void loop() {
  for(int row = 0 ; row < 10 ; row++ ) {
    for(int pinCtr = 0 ; pinCtr < 5 ; pinCtr++) {
      if(bytes[row][pinCtr] > 0) {
        digitalWrite(pins[pinCtr],HIGH);
      } else {
        digitalWrite(pins[pinCtr],LOW);
      }
    }
    delay(del);
  }
}
