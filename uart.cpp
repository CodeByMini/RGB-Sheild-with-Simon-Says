#include "Arduino.h"
#include <string.h>
#include "uart.h"
const char* commands[41] = {"4", "Commands", "!go - runs the current choice", "!exit - returns to main menu", "!pot:value - sets the speed or intensity"};

/*This part is called "The Spaghetti Incident?" named after Guns N Roses fifth studio album. 
  Hope it doesn't break up the band.*/

/*lots of variables that needs to be reached in multiple functions*/
bool newData = false;
bool didWeGetNewSerialPot = false;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];

int serialPot = 0;

/*Used to get variable from here to elsewhere*/
bool GetSerialPotBool(){
  if(didWeGetNewSerialPot){
    return true;
  }
  return false;
}

/*Used to check if there was a new reading from the serial pot command
  returns an positive int if its true and -1 if its false*/
int GetSerialPotValue(){
  if(didWeGetNewSerialPot){
    didWeGetNewSerialPot = false;
    return serialPot;
  }
  return -1;
}

/*Recives chars from serial prompt and stacks them in a nice little box*/
bool ReciveString() {

    /*bool to indicate if the function is receving a message */
    static bool reciving = false;

    /*starting value to put the first char in the first box*/
    static byte index = 0;

    /*prefix char to get rid of cats walking on keyboard*/
    char start = '!';

    /*One char, One Serial.read(). This one will change. alot.*/
    char recievedCharacter;

    /*Serial.available returns a number if there's chars in the serial buffer
      newData changes if a newline \n is recived to indicate a new command has been recieved*/
    if (Serial.available() > 0 && newData == false) {

        /*here that little worker gets each char in the buffer*/
        recievedCharacter = Serial.read();

        /*when reciving each recieved char is put into recivedChars and the index is incremented 
          for each char, building that string to exellence. words over 32 chars will change the
          last letter until its done. So. Dont send words over 32 chars.
          If a new line is recieved the string will finish and the bools will reset*/
        if (reciving) {
          if (recievedCharacter != '\n') {
            receivedChars[index] = recievedCharacter;
            index++;
            if (index >= numChars) {
              index = numChars - 1;
            }
          }else {
            receivedChars[index] = '\0';
            reciving = false;
            index = 0;
            newData = true;
          }
        }
        else if (recievedCharacter == start) {
            reciving = true;
        }
    }
    /*Yay! we got new data*/
    if(newData){
      /*copy the string into a new variable and set newdata to false 
        to be ready to recive the next command*/
     strcpy(tempChars, receivedChars);
    newData = false;
    /*return true to tell the next function theres a new command to handle*/
      return true;
    }else{
      return false;
    }
}

/*Parse recived string into commands*/
int ParseData() {

  /*strtok reads the string until the delimiter ":" and puts it in p*/  
  char *p = strtok (tempChars, ":");

  /*compares p to different keywords and returns corresponding int for the SetState function*/
  while (p != NULL){
    if(strcmp(p, "go")==0 || strcmp(p, "exit")==0){
      return -1;
      break;
    }
    else if(strcmp(p, "pot")==0){
        p = strtok(NULL, ":");
        serialPot = atoi(p);
        didWeGetNewSerialPot = true;
        return -2;   
        break;
    }
    else if(strcmp(p, "help")==0){
        PrintMenu(commands);
        return -3;   
        break;
    }
    /*transform (char)*'1' to (int)1*/
    else if(atoi(p) < 9 && atoi(p) >= 1){
      return (atoi(p)-1);
      break;
    }
    else{
      return -3;
      break;
    }
  }
}

/*You'll never belive it, but this prints the menu*/
void PrintMenu(char *items[20]) {
  /*this is just to get some space between the menus.*/
  for(int j = 0; j< 50; j++){
    Serial.println("");
  }
  Serial.println("-------------");
  /*the first position in the array is a size*/
  for (int i = 1; i < atoi(items[0]) + 1; i++) {
    Serial.println(items[i]);
    if (i == 1) {
      Serial.println("-------------");
    }
  }
  Serial.println("-------------");
  Serial.println("Prefix your command with !");
  Serial.println("Type !help to get...help");
  
}
