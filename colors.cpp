#include "colors.h"
#include "uart.h"
#include <Arduino.h>

/*Creating the menus needed for the functions*/
const char *rgbMenu[6] = {"4", "RGB Colors", "1. Red", "2. Green", "3. Blue"};
const char *rainbowMenu[4] = {"3", "Rainbow", "Turn potentiometer to change speed", "write !pot:value to change speed"};

//int lastSerialPot = 0;

int lastPot = 0;
int gotSerialPot = 0;
int currentPot = 0;
int value = 0;
//unsigned long latestValue = 0;
//unsigned long tempRainbow = 0;

/*This checks if theres a new potentiometer value from the serial port*/
bool GotSerialPot(){
  gotSerialPot = GetSerialPotValue();
  if(gotSerialPot > 0){
    return true;
  }
  return false;
}
/*Reads the potentiometer and returns true if the value if its not in 2% range of the last value. 
  Also saves value to global variable*/
bool GotPotValue(int biggest){
  currentPot = map(analogRead(pot), 0, 1023, 10, biggest);
  if (currentPot >= lastPot+lastPot*0.02 || currentPot <= lastPot-lastPot*0.02) {
     lastPot = currentPot;
     return true;
  }
return false;
}

/*Returns a pot value. either a value recieved from serial or from potentiometer*/
int MappedValue(int biggest) {
  if(GotPotValue(biggest)){
    return currentPot;
  }
  if(GetSerialPotBool()){
    GotSerialPot();
    return gotSerialPot;
  }
  //return -1 if no new value has been recieved or read
  return -1;
}

/*
long MappedValue(long biggest, char *i) {
  return (long)map(analogRead(pot), 0, 1023, 1, biggest);
}
*/

/*Write PWM values to LED. accepts a RGB struct*/
void RGBcolor(RGB colors) {
  analogWrite(redPin, colors.red);
  analogWrite(greenPin, colors.green);
  analogWrite(bluePin, colors.blue);
}

/*Function for Rainbow/fade */
void Rainbow(STATE *state) {

  //Variable to keep track when it was last updated
  unsigned long lastChanged = 0;

  //Variable to keep track of potentiometer
  int temp = 0;

  //Create a color to start off with
  RGB rainbowColor = {127, 1, 254};

  //Set starting direction for fade.
  int redDir = 1, greenDir = -1, blueDir = 1;

  //Prints out the menu
  PrintMenu(rainbowMenu);

  /*Uses key 2 as on or off. Off returns to main menu*/
  while (state->stateExcecuter) {

    /*Updates states if a key is pressed*/
    SetState(state, MENU_CHOICES);

    /*Get value from potentiometer or serial, the int is max value*/
    value = MappedValue(10000);

    /*Since the MappedValue returns -1 if no new values has been recieved
    Only update temp value(Thats used for the speed) if it is above 0*/
    if(value > 0){
      temp = value;
    }
    /*Delay without blocking for the speed*/
    if (micros() - lastChanged > temp) {
      lastChanged = micros();

      /*increment or decrement with 1 depending on direction*/
      rainbowColor.red = rainbowColor.red + redDir;
      rainbowColor.green = rainbowColor.green + greenDir;
      rainbowColor.blue = rainbowColor.blue + blueDir;

      /*Nifty (and stolen) way to change the direction
        if the value reaches 255 or 0 direction is inverted by multiplying with -1*/
      if (rainbowColor.red >= 255 || rainbowColor.red <= 0) {
        redDir *= -1;
      }
      if (rainbowColor.green >= 255 || rainbowColor.green <= 0) {
        greenDir *= -1;
      }
      if (rainbowColor.blue >= 255 || rainbowColor.blue <= 0) {
        blueDir *= -1;
      }
      /*Light'em Up! (with the new color)*/
      RGBcolor(rainbowColor);
    }
  }
}

/*Cycles through the LED's colors and change their intensity with the potentiometer (or serial command)*/
void ColorSwitcher(STATE *state) {

  /*Starting color. red is on since its controlled first*/
  RGB color = {0, 255, 255};

  /*Create a state to handle buttonpresses*/
  STATE colorchanger = {0, 1};

  /*Variables used to check if the intensity is changed*/
  int temp = 0;

  /*Print the menu*/
  PrintMenu(rgbMenu);

  /*Button 2 exits function and returns to main menu*/
  while (state->stateExcecuter) {
    if (!colorchanger.stateExcecuter) {
      state->stateExcecuter = 0;
    }

    /*Updates states if a key is pressed, the int represents the number of choices in this menu*/
    SetState(&colorchanger, 3);

    /*Get value from potentiometer or serial, the int is max value*/
    value = MappedValue(255);
    /*Since the MappedValue returns -1 if no new values has been recieved
    Only update temp value(Thats used for the speed) if it is above 0*/
    if(value > 0){
      temp = value;
    }
    /*cycles through colors if button 1 is pressed*/
    switch (colorchanger.stateSwitcher) {
    case 0:
      color.red = temp;
      break;
    case 1:
      color.green = temp;
      break;
    case 2:
      color.blue = temp;
      break;
    }
    /*LED there be Light!*/
    RGBcolor(color);
  }
}
