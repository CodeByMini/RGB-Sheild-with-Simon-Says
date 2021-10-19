#include "Arduino.h"
#include "colors.h"
#include "states.h"
#include "simonsays.h"
#include "uart.h"

/*Create an array of char arrays that holds the menu, for printing*/
const char* mainMenu[12] = {"4", "MAIN MENU", "1. RGB Colors", "2. Rainbow", "3. Simon Says"};

/*Create an instance with some basic colors*/
COLORS colors;

/*Create a state for the main program, starts with 9 to trigger the case that prints the menu*/
volatile STATE state = {9, 0};

 
void setup() {
  /*Start serial communication*/
  Serial.begin(115200);

  /*Set up pins*/
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(pot, INPUT);
  pinMode(key1, INPUT);
  pinMode(key2, INPUT);

  /*Setup interrupts*/
  attachInterrupt(digitalPinToInterrupt(INTERRUPT1), HandleInterrupt1, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT2), HandleInterrupt2, RISING);
  
  /*Make sure the LED is off at startup*/
  RGBcolor(colors.black);
}

void loop() {
  /*As long as button 2 is not pressed loop though a visual menu*/
  while (!state.stateExcecuter) {

    /*Updates state on button presses or serial commands*/
    SetState(&state, MENU_CHOICES);

    /*This is just to show the user where they are in the menu*/
    switch (state.stateSwitcher) {
      case 0:
        RGBcolor(colors.pink);
        return;
      case 1:
        RGBcolor(colors.orange);
        return;
      case 2:
        RGBcolor(colors.cyan);
        return;
        /*Case 9 is triggered every time the user returns to main menu*/
      case 9:
        PrintMenu(mainMenu);
        state.stateSwitcher = 0;
    }
  }
  /*Switch case triggered if button 2 is pressed. Runs the corresponding method determined by stateExcecuter*/
  switch (state.stateSwitcher) {
    case 0:
      ColorSwitcher(&state);
      state.stateSwitcher = 9;
      return;
    case 1:
      Rainbow(&state);
      state.stateSwitcher = 9;
      return;
    case 2:
      SimonSays(&state);
      state.stateSwitcher = 9;
      return; 
  }
}
