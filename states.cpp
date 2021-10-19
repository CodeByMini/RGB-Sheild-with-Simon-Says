#include "Arduino.h"
#include "states.h"
#include "defines.h"
#include "uart.h"

/*Variables used for debouncing buttons*/
unsigned long lastInterrupt1 = 0;
unsigned long lastInterrupt2 = 0;
bool isInterrupted1 = false;
bool isInterrupted2 = false;

/*Handles buttonpresses from an interrupt button*/
void HandleInterrupt1() {
  /*detatch button from interrupt to avoid unwanted triggering*/
  detachInterrupt(INTERRUPT1);
  /*if the button never been pressed this will evaluate to false and set isInterrupted to true
    and the time when the button was pressed is stored in an varable. If a debouncecheck is ongoing
    this will not trigger*/
  if (!isInterrupted1) {
    isInterrupted1 = true;
    lastInterrupt1 = millis();
  }
  /*reattach the button to be an interrupt*/
  attachInterrupt(digitalPinToInterrupt(INTERRUPT1), HandleInterrupt1, RISING);
}

/*this is the same function as above but all 1's have been changed to 2's*/
void HandleInterrupt2() {
  detachInterrupt(INTERRUPT2);
  if (!isInterrupted2) {
    isInterrupted2 = true;
    lastInterrupt2 = millis();
  }
  attachInterrupt(digitalPinToInterrupt(INTERRUPT2), HandleInterrupt2, RISING);
}

/*Button DeBouncing function. makes sure its an actual keypress and not bad contact inside the button*/
bool DebounceInterrupt1() {

  /*if this bool is true, that means that a debouncing check is ongoing*/
  if (isInterrupted1) {

    /*if DEBOUNCE-amount of time has not passed this will not trigger */
    if (millis() - lastInterrupt1 > DEBOUNCE) {

      /*change the bool to false allows for new button presses to be triggered*/
      isInterrupted1 = false;

      /*return true to show that a button press has happened*/
      return true;
    }
  } else {

    /*no actual buttonpress has happened. yet.*/
    return false;
  }
}

/*Again. copy paste of above with numbers doubleing*/
bool DebounceInterrupt2() {
  if (isInterrupted2) {
    if (millis() - lastInterrupt2 > DEBOUNCE) {
      isInterrupted2 = false;
      return true;
    }
  } else {
    return false;
  }
}

/*This bool tells us if a new serial command has arrived*/
bool newCommand = false;

/*Generic State Handler, accepts a STATE pointer and a int to set the number of
  states for stateSwitcher*/
  void SetState(STATE *state, int states) {
    newCommand = ReciveString();
    if (newCommand) {
      int tempCommand = ParseData();
      //Serial.println(tempCommand);
      if(tempCommand == -1){
        if(state->stateExcecuter){
          state->stateExcecuter = 0;
        }else{
          state->stateExcecuter = 1;
        }
      }else if (tempCommand > 9){
        state->stateSwitcher = tempCommand;
      }
    }
 
  /*Checks the DebounceInterrupt1 if its true indicating that a button was pressed
    and increments the choice state, rolls back to 0 if the cap is reached. cap = states variable*/
  if (DebounceInterrupt1()) {
    if (state->stateSwitcher < states) {
      state->stateSwitcher++;
    } else {
      state->stateSwitcher = 0;
    }
  }

  /*Same as above, but only switches stateExcecuter between 1 and 0 */
  if (DebounceInterrupt2()) {
    if (state->stateExcecuter == 0) {
      state->stateExcecuter = 1;
      
    } else {
      state->stateExcecuter = 0;
    }
  }
}
