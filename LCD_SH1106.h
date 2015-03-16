/*
Cut down library from
https://github.com/stanleyhuangyc/MultiLCD
*/
#include <Arduino.h>
#include <SoftI2CMaster.h>

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

/*
Specifies line address to determine the initial display line or COM0. The RAM display data becomes the 
top line of OLED screen. It is followed by the higher number of lines in ascending order, corresponding to the duty cycle. 
When this command changes the line address, the smooth scrolling or page change takes place. 
*/
#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_ContrastControlMode 0x81
 
#define I2C_ADDR 0x78 >> 1

class LCD_SH1106 
{
public:
  void begin(byte sclPin,byte sdaPin);
  void drawFromRAM(byte* buffer, byte width, byte height);
  void setCursor(byte column, byte line);
  void clear(byte x = 0, byte y = 0, byte width = 128, byte height = 64);
  void draw(const PROGMEM byte* buffer, byte width, byte height);

private:
  void WriteCommand(unsigned char ins);
  SoftI2CMaster i2c;
  byte m_col;
  byte m_row;
};


