/*************************************************************************
 * Replacement LCD Display emulator for VDO MARINE gauge (and Volvo Penta)
 *
 * KI "W", Induktiv
 * N02 012 165 (168)
 * Date: KW 36/04
 *
 * http://www.em-schiffselektronik.de/html/508475_Marine_Katalog_2004_2005.pdf
 * 
 * Useful links...
 http://www.ybw.com/forums/showthread.php?263571-VDO-tachos-How-to-callibrate-and-make-them-work
 
 * Copyright (c) 2014 Stuart Pittaway (support@waterexplorer.co.uk)
 * All rights reserved.
 *************************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <SoftI2CMaster.h>
#include "LCD_SH1106.h"
#include <avr/pgmspace.h>

//http://arduino.cc/en/Hacking/PinMapping168

//back of vdo gauge wiring circuits
// A B C
//   D E F
//A=SDA (data) red wire
//B=+5v red
//C=gnd black
//D=SCL (clock) black wire
//E=n/c
//F=n/c

const uint8_t vdoBitmaps[] PROGMEM   =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0xF8, 0xF8, 
  0xF8, 0xF8, 0xF8, 0xF0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xC0, 0xF0, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0x78, 0x38, 0x08, 0x00, 0xF8, 0xF8, 0xF8, 
  0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 
  0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x80, 0xC0, 
  0xE0, 0xF0, 0xF0, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 
  0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0xE0, 0xE0, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x03, 0x07, 0x1F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xF0, 0xC0, 0xF0, 0xF8, 
  0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0x0F, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 
  0x81, 0x81, 0x81, 0x81, 0x81, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3C, 0x3C, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 
  0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x1F, 0x1F, 0x1F, 0x1F, 
  0x1F, 0x1F, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
  0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x0F, 0x07, 0x03, 0x00, 
  0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
  0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07, 
  0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

// Bitmap sizes for vdo
#define vdoWidthPixels  128
#define vdoHeightPixels  24

LCD_SH1106 lcd; /* for SH1106 OLED module */

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

//Slip screen down a little to align with cutout in black panel
#define yoffset 2

#define BLACK 0
#define WHITE 1

//Real panel width/height 128x64 pixels
#define WIDTH 		      128	//128 Pixels
#define HEIGHT 		      64	//64  Pixels

#define fig8_width 8
#define fig8_gap 2
#define fig8_height 8
#define fig8_vgap 1

#define space fig8_width+fig8_gap+fig8_gap+fig8_gap

//Reserve RAM just the size we really need (288 bytes)
#define BUFFER_HEIGHT    24
#define BUFFER_WIDTH     128

#define sclPin 7 // digital pin 7 wired to 'c' on BlinkM
#define sdaPin 6 // digital pin 6 wired to 'd' on BlinkM

uint8_t buffer[BUFFER_WIDTH * BUFFER_HEIGHT / 8];
uint8_t lastdisplaybytes[7];
uint8_t packet[24];
uint8_t displaybytes[7];


void setup()
{
  memset(buffer,0,sizeof(buffer));
  memset(lastdisplaybytes,0,sizeof(lastdisplaybytes));

  lcd.begin(sclPin,sdaPin);
  lcd.clear();
  lcd.setCursor(0, yoffset);
  lcd.draw(vdoBitmaps, vdoWidthPixels, vdoHeightPixels);
  delay(3000);
  lcd.clear();

  //Draw the 8888888 on the display
  for(int b=0;b< 96;b=b+space) {
    seven_segment_vdo_display(b,0,0xFF);
  }

  lcd.setCursor(0, yoffset);
  lcd.drawFromRAM(buffer,BUFFER_WIDTH,BUFFER_HEIGHT);

  delay(2000);

  //Start real i2c and wait for packet to arrive
  Wire.begin(0x38);                // join i2c bus with address #38
  Wire.onReceive(receiveEvent);
}


void receiveEvent(int bytes)
{
  //We expect to receive 15 bytes like this...
  //Which spells ADJUST if you are interested!
  //CA E0 F6 80 78 00 0B F7 7D 00 C3 A3 01 00 B0 00
  //0  1  2  3  4  5  6  7  8  9   A  B  C  D  E  F
  //               XX          XX          XX    XX = not used
  // binary= 0000 0000 0000 1011 1111 0111 0111 1101 0000 0000 1100 0011 1010 0011 0000 0001 0000 0000 1011 0000 0000 0000
  // byte  = 5    5       6    6    7    7    8    8    9    9    A    A    B    B    C    C    D    D    E    E    F    F

  int ptr=0;

  //Prevent buffer overflows
  if (bytes>sizeof(packet)) return;

  //Probably dont need this
  memset(packet,0,sizeof(packet));

  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    packet[ptr++]=c;
  }

  //Ensure packet is correct length
  if (ptr==0xF) {
    //Check its a display command
    if ((packet[0]==0xCA) && (packet[1]==0xE0) && (packet[2]==0xF6)  && (packet[3]==0x80)   && (packet[4]==0x78 || packet[4]==0x7A))
    {
      //Its a packet for me, decode it  

      //packet[4] = 0x78 or 0x7a to indicate which LCD ram buffer to use - we ignore this, but its used to flash the screen

      //moving to the left
      displaybytes[0]=(packet[0x6] & B11110000) | (packet[0xE]  & B00001111);
      //0x05 is not used
      //0x0D is not used
      displaybytes[1]=((packet[0x6] & B00001111)<<4 ) | ((packet[0xE] & B11110000)>>4);
      displaybytes[2]=(packet[0x7] & B11110000) | (packet[0xC]  & B00001111); 
      displaybytes[3]=((packet[0x7] & B00001111) <<4) | ((packet[0xC] & B11110000)>>4);
      displaybytes[4]=(packet[0x8] & B11110000) | (packet[0xB]  & B00001111);  
      //0x09 is not used
      displaybytes[5]=((packet[0x8] & B00001111)<<4 ) | ((packet[0xB] & B11110000)>>4); 
      //Most right character
      displaybytes[6]=packet[0xA];

      //As redrawing the screen is a slow and expensive operation
      //only do it if the screen has changed by comparing the two buffers
      if (memcmp(lastdisplaybytes, displaybytes, sizeof(lastdisplaybytes))!=0) {
        //Store into lastdisplaybytes
        memcpy(lastdisplaybytes, displaybytes, sizeof(lastdisplaybytes));

        const uint8_t y=0;
        uint8_t x=0;

        memset(buffer,0,sizeof(buffer));
        seven_segment_vdo_display(x,y,displaybytes[0]);
        x=x+space;
        seven_segment_vdo_display(x,y,displaybytes[1]);
        x=x+space;
        seven_segment_vdo_display(x,y,displaybytes[2]);
        x=x+space;
        seven_segment_vdo_display(x,y,displaybytes[3]);
        x=x+space;
        seven_segment_vdo_display(x,y,displaybytes[4]);
        x=x+space;
        seven_segment_vdo_display(x,y,displaybytes[5]);
        x=x+space;
        seven_segment_vdo_display(x,y,displaybytes[6]);
        x=x+space;

        //Send RAM display buffer over i2c to real display
        lcd.setCursor(0, yoffset);
        lcd.drawFromRAM(buffer,BUFFER_WIDTH,BUFFER_HEIGHT);
      }
    }  
  }  
}

void loop()
{
  //Do nothing!
  delay(500);
}

void drawVLine(uint8_t x, uint8_t y, uint8_t h) {
  for(int a=0;a<h;a++) {
    drawPixel(x,y+a,WHITE); 
  }
}

void drawHLine(uint8_t x, uint8_t y, uint8_t w) {
  for(int a=0;a<w;a++) {
    drawPixel(x+a,y,WHITE); 
  }  
}

// the most basic function, set a single pixel
void drawPixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x < 0) || (x >= BUFFER_WIDTH) || (y < 0) || (y >= BUFFER_HEIGHT))
    return;

  /*
  // check rotation, move pixel around if necessary
   switch (0) {
   case 1:
   swap(x, y);
   x = WIDTH - x - 1;
   break;
   case 2:
   x = WIDTH - x - 1;
   y = HEIGHT - y - 1;
   break;
   case 3:
   swap(x, y);
   y = HEIGHT - y - 1;
   break;
   }  
   */
  // x is which column
  if (color == WHITE) 
    buffer[x+ (y/8) * BUFFER_WIDTH] |= _BV((y%8));  
  else
    buffer[x+ (y/8)*BUFFER_WIDTH] &= ~_BV((y%8)); 
}


// bresenham's algorithm - thx wikipedia
void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {

  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } 
  else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } 
    else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}


void seven_segment_vdo_display(uint8_t x,uint8_t y,uint8_t c) {  

  if (c==0) return;

  //12345678
  //gdbcahfe

  //g=middle
  //d=bottom
  //b=top right
  //c=bottom right
  //a=top
  //h=dot
  //f=top left
  //e=bottom left

  //8
  if (c & B00001000) {
    //Top
    drawHLine(x+fig8_gap,y,fig8_width);
    drawHLine(x+fig8_gap,y+1,fig8_width);
  }

  if (c & B00000010) {
    //Top left
    drawVLine(x,y+fig8_vgap,fig8_height);
    drawVLine(x+1,y+fig8_vgap,fig8_height);
  }

  if (c & B00100000) {
    //Top right
    drawVLine(x+fig8_width+fig8_gap,y+fig8_vgap,fig8_height);
    drawVLine(x+fig8_width+fig8_gap+1,y+fig8_vgap,fig8_height);
  }    

  if (c & B10000000) {
    //Middle
    drawHLine(x+fig8_gap,y+fig8_height+fig8_vgap-1,fig8_width);
    drawHLine(x+fig8_gap,y+fig8_height+fig8_vgap+1-1,fig8_width);
  }

  if (c & B00000001) {
    //bottom left
    drawVLine(x,y+fig8_height+fig8_vgap+fig8_vgap-1,fig8_height);
    drawVLine(x+1,y+fig8_height+fig8_vgap+fig8_vgap-1,fig8_height);
  }

  if (c & B00010000) {
    //bottom right
    drawVLine(x+fig8_width+fig8_gap,y+fig8_height+fig8_vgap+fig8_vgap-1,fig8_height);
    drawVLine(x+fig8_width+fig8_gap+1,y+fig8_height+fig8_vgap+fig8_vgap-1,fig8_height);
  }

  if (c & B01000000) {
    //Bottom
    drawHLine(x+fig8_gap,y+fig8_height+fig8_height+fig8_vgap+fig8_vgap-2,fig8_width); 
    drawHLine(x+fig8_gap,y+fig8_height+fig8_height+fig8_vgap+fig8_vgap+1-2,fig8_width); 
  }

  if (c & B00000100) {
    //DOT
    drawHLine(x+fig8_width+fig8_gap+fig8_gap+1,y+fig8_height+fig8_height+fig8_vgap+fig8_vgap-2,fig8_gap); 
    drawHLine(x+fig8_width+fig8_gap+fig8_gap+1,y+fig8_height+fig8_height+fig8_vgap+fig8_vgap+1-2,fig8_gap); 
  }  
}


