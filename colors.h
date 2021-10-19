#ifndef COLORS_H
#define COLORS_H

#include "states.h"
#include "defines.h"

/*How to struct a led in 5 lines*/
typedef struct {
    int red;
    int blue;
    int green;
}   RGB;

/*Oh shiny colors!*/
typedef struct{
      RGB red = {0, 255, 255};
      RGB blue = {255, 255, 00};
      RGB green = {255, 0, 255};
      RGB yellow = {0, 0, 255};
      RGB magenta = {0, 255, 0};
      RGB white = {0, 0, 0};
      RGB orange = {0, 128, 255};
      RGB pink = {0, 153, 77};
      RGB cyan = {255, 0, 0};
      RGB black = {255, 255, 255};
      
} COLORS;

/*we need these elsewhere*/
int MappedValue(int);
long MappedValue(long, int);
void RGBcolor(RGB);
void Rainbow(STATE*);
void ColorSwitcher(STATE*);

#endif
