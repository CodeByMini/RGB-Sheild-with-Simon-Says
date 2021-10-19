#include "Arduino.h"
#include "colors.h"
#include "states.h"
#include "uart.h"

/*This defines how many rounds of Simon Says to be played*/
#define GAMESIZE 3

const char* simonMenu[13] = {"3", "SIMON SAYS", "1. Color choice", "2. Answer"};

RGB red = {0, 255, 255};
RGB green = {255, 0, 255};
RGB blue = {255, 255, 0};
RGB magenta = {0, 255, 0};
RGB yellow = {0, 0, 255};
RGB black = {255, 255, 255};
RGB white = {0, 0, 0};
RGB offwhite = {128, 128, 128};

RGB startup[5] = {red, green, blue, magenta, yellow};

/*Blinks a color a number of times defined by howMany. "delay" time, time between blinks, is defined by delayTime*/
void BlinkColor(RGB color, int howMany, unsigned long delayTime) {
  
  /*set up variable to store the time it last blinked*/
  unsigned long lastMillis = 0;

  /*variable used to move the loop forward*/
  int i = 0;

  /*loops for howMany-times multiplied with 2 to account for the blackouts*/
  while (i < howMany * 2) {
    /*modulus to turn on the light when i is odd and turn off led when i is even*/
    if (i % 2 != 0) {
      RGBcolor(color);
    } else {
      RGBcolor(black);
    }

    /*only increment i if the delaytime has passed*/
    if (millis() - lastMillis > delayTime * 2) {
      i++;
      lastMillis = millis();
    }
  }
}

/*same as above but takes an array as parameter to be able to do a colorchanging
  blink effect*/
void BlinkColor(RGB colors[], unsigned long delayTime) {
  unsigned long lastMillis = 0;
  int i = 0;
  while (i < 6) {
    RGBcolor(colors[i]);
    if (millis() - lastMillis > delayTime) {
      lastMillis = millis();
      i++;
    }
  }
}

/*Simon says is a game where the computer shows you a sequence and you should 
  recreate that sequence from memory. Each round adds a color to remember*/

  /*The function takes a STATE pointer as parameter that is used to exit the game*/
void SimonSays(STATE * state) {

  /*But hey! heres another STATE. this is used to control the game*/
  STATE game = {0, 0};

  /*needed to pass around the current round*/
  int current = 0;

  /*the colors of the game that may show up to be remembered*/
  RGB colors[3] = {blue, magenta, yellow};

  /*Create two arrays to hold the computers choice and the players answers*/
  int gameArray[GAMESIZE];
  int answerArray[GAMESIZE];


  Serial.println("Game started");
  PrintMenu(simonMenu);
  
  /*A seed for the randomfunction*/
  randomSeed(millis());

  /*fill the computers sequence with "random" ints*/
  for (int i = 0; i < GAMESIZE; i++) {
    gameArray[i] = random(0, 3);

    //This is for cheaters
    //Serial.println(gameArray[i]);
  }

  /*Oh flashy lights!*/
  BlinkColor(startup, 100);

  /*An outer loop runs the entire game, the inner loop is for each round*/
  for (int j = 0; j < GAMESIZE; j++) {
    for (int i = 0; i < j + 1; i++) {
      
      /*remember we needed to save someting. well here it is.*/
      current = i;

      /*light up the current rounds sequence*/
      BlinkColor(black, 1, 600);
      BlinkColor(colors[gameArray[i]], 1, 500);
      BlinkColor(black, 1, 100);
    }

    /*Answertime! do you remember the colors?*/
    BlinkColor(white, 1, 500);

    /*So once again we need a variable from inside an for loop, outside the for loop*/
    int choice = 0;

    /*this loops through the round number of times to let the player
      answer for all the colors of the current round*/
    for (int r = 0; r < current + 1; r++) {

      /*as long as the user dont push button 2 its possible to cycle through the answers*/
      while (!game.stateExcecuter) {
        /*Set the current choice into variable to be checked against the computers list*/
        choice = r;

        /*This allows the user to actually use the buttons*/
        SetState(&game, GAMESIZE);

        /*Show the player what color i currently selected. It would be hard if the had to guess that aswell*/
        RGBcolor(colors[game.stateSwitcher]);
      }
      /*Blinky flashy thingy to show that an answer was recorded*/
      BlinkColor(offwhite, 3, 50);

      /*Checking the answer against the current rounds current color*/
      if (game.stateSwitcher != gameArray[choice]) {

        /*If you were wrong, you suck. the red light is an indicator that you suck*/
        Serial.println("wrong answer");
        BlinkColor(red, 2, 100);

        /*Game over. Exit to Main menu*/
        state->stateExcecuter = 0;
        return;
      } else {
        /*Record the answer inside the answer array. This is kind of redundant, but is nice if this game goes big*/
        answerArray[choice] = game.stateSwitcher;
        /*Onto the next round!*/
        game.stateExcecuter = 0;
      }
    }
    /*YOU MADE IT! all the way through the game. Awesome!*/
    if (current + 1 == GAMESIZE) {

      /*Let us just check your answers again because reason...*/
      for (int a = 0; a < current + 1; a++) {
        if (answerArray[a] == gameArray[a]) {
          
          /*Green light! You da best!*/
          BlinkColor(green, 2, 100);

        } else {
          /* will not get to this point. Wait for next release.*/
          BlinkColor(red, 2, 100);
        }
      }
      /*aaaaaand thats my exit cue. bye now*/
      state->stateExcecuter = 0;
      return;
    }
  }
}
