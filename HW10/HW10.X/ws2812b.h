#ifndef WS2812B_H__
#define WS2812B_H__

#include<xc.h> // processor SFR definitions

// Timer2 delay times, you can tune these if necessary
#define LOWTIME 15 // number of 48MHz cycles to be low for 0.35uS
#define HIGHTIME 65 // number of 48MHz cycles to be high for 1.65uS

// link three 8bit colors together
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor; 

void ws2812b_setup();
void ws2812b_setColor(wsColor*,int);
wsColor HSBtoRGB(float hue, float sat, float brightness);

#endif