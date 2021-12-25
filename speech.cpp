#include "speech.h"
#include <string.h>
#include "globals.h"
#include "hardware.h"
#include "graphics.h"
/**
 * Draw the speech bubble background.
 * Use this file to draw speech bubbles, look at the uLCD libraries for printing
 * text to the uLCD (see graphics.cpp note for link)
 */
static void draw_speech_bubble();

/**
 * Erase the speech bubble.
 */
/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble()
{
    uLCD.filled_rectangle(0, 113, 127, 93, 0x000000); // blue rectangle
    uLCD.filled_rectangle(0,    93, 127,  92, 0x0000FF); // Top
    uLCD.filled_rectangle(0,   113, 127, 112, 0x0000FF); // Bottom
    uLCD.filled_rectangle(0,    113,   1, 92, 0x0000FF); // Left
    uLCD.filled_rectangle(127,  113, 126, 92, 0x0000FF); // Right
}



void draw_speech_line(const char* line, int which)
{
    uLCD.textbackground_color(BLACK); // text background color is black
    uLCD.color(0x0000FF);// setting text color to blue 
        if(which == TOP) uLCD.locate(1,12); // for TOP line
        else if(which == BOTTOM) uLCD.locate(1,13); // for BOTTOM line
        for(int i=0; i < 17; i++){   // each line has 17 char
            uLCD.printf("%c",*line++); // print 17 char only
            wait_ms(50);
        }
        
    
}
void speech_bubble_wait()
{
    while(1)
    {
        wait_ms(100);
        GameInputs in = read_inputs();
        if(in.b1 == 0){ // when action button is 0,    
        break; // when button is pressed, go out of while
        }
        wait_ms(100);
    }
}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    
}

void long_speech(char* line) 
{
    uLCD.textbackground_color(BLACK); // text background color is black
    uLCD.color(0x0000FF); //text color
    int i;
    while (*line)
    {
        uLCD.locate(1,12); // locate (1,12)
        draw_speech_bubble(); // draw speech bubble
        for(i = 0; i<17 && *line; i++){ // for statement until 17 chars printed and the line should not be empty
            uLCD.printf("%c", *line++); // print each char
            wait_ms(50);
        }
        if ( !(*line) ) break; // if line is empty, break while
        uLCD.locate(1,13); // print bottom line
        for(; i<34 && *line; i++){ // for statement until the next 17 chars printed and the line should not be empty total 34 chars
            uLCD.printf("%c", *line++); // print each char
            wait_ms(40); 
        }
        speech_bubble_wait(); // wait until action button
    }            
}
