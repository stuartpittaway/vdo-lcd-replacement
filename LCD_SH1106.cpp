/*
Cut down library from
 https://github.com/stanleyhuangyc/MultiLCD
 */
#include "LCD_SH1106.h"

void LCD_SH1106::WriteCommand(unsigned char ins)
{
  i2c.beginTransmission(I2C_ADDR);//0x78 >> 1
  i2c.write(0x00);  //0x00
  i2c.write(ins);
  i2c.endTransmission();
}

void LCD_SH1106::setCursor(unsigned char x, unsigned char y)
{
  m_col = x + 2;
  m_row = y;
  WriteCommand(0xb0 + m_row);
  WriteCommand(m_col & 0xf);//set lower column address
  WriteCommand(0x10 | (m_col >> 4));//set higher column address
}

void LCD_SH1106::begin(byte sclPin,byte sdaPin)
{
  i2c = SoftI2CMaster( sdaPin,sclPin );

  //Wire.begin();

  WriteCommand(0xAE);    /*display off*/
  WriteCommand(0x02);    /*set lower column address*/
  WriteCommand(0x10);    /*set higher column address*/
  WriteCommand(0x40);    /*set display start line*/
  WriteCommand(0xB0);    /*set page address*/

  WriteCommand(SSD1306_ContrastControlMode);    /*contract control*/
  WriteCommand(0x80);    /*128*/

  WriteCommand(0xA1);    /*set segment remap*/
  WriteCommand(0xA6);    /*normal / reverse*/

  WriteCommand(0xA8);    /*multiplex ratio*/
  WriteCommand(0x3F);    /*duty = 1/32*/

  WriteCommand(0xad);    /*set charge pump enable*/
  WriteCommand(0x8b);     /*external VCC   */
  WriteCommand(0x30);    /*0X30---0X33  set VPP   9V liangdu!!!!*/

  WriteCommand(0xC8);    /*Com scan direction*/
  WriteCommand(0xD3);    /*set display offset*/
  WriteCommand(0x00);   /*   0x20  */

  WriteCommand(0xD5);    /*set osc division*/
  WriteCommand(0x80);
  WriteCommand(0xD9);    /*set pre-charge period*/
  WriteCommand(0x1f);    /*0x22*/
  WriteCommand(0xDA);    /*set COM pins*/
  WriteCommand(0x12);
  WriteCommand(0xdb);    /*set vcomh*/
  WriteCommand(0x40);
  WriteCommand(0xAF);    /*display ON*/

  clear();
}


void LCD_SH1106::clear(byte x, byte y, byte width, byte height)
{
  WriteCommand(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
  WriteCommand(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
  WriteCommand(SSD1306_SETSTARTLINE | 0x0); // line #0

    height >>= 3;
  width >>= 3;
  y >>= 3;

  for (byte i = 0; i < height; i++) {
    // send a bunch of data in one xmission
    WriteCommand(0xB0 + i + y);//Set Page Address: (B0H - B7H) 
    WriteCommand((x + 2) & 0xf);//set lower column address
    WriteCommand(0x10 | (x >> 4));//set higher column address

    for(byte j = 0; j < 8; j++){
      i2c.beginTransmission(I2C_ADDR);
      i2c.write(0x40);
      for (byte k = 0; k < width; k++) {
        i2c.write(0);
      }
      i2c.endTransmission();
    }
  }

  setCursor(0, 0);
}

void LCD_SH1106::drawFromRAM(byte* buffer, byte width, byte height)
{
  WriteCommand(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
  WriteCommand(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
  WriteCommand(SSD1306_SETSTARTLINE | 0x0); // line #0

    int p = 0;
  height >>= 3;
  width >>= 3;
  for (byte i = 0; i < height; i++) {
    // send a bunch of data in one xmission
    WriteCommand(0xB0 + i + m_row);//set page address
    WriteCommand(m_col & 0xf);//set lower column address
    WriteCommand(0x10 | (m_col >> 4));//set higher column address

    for(byte j = 0; j < 8; j++){
      i2c.beginTransmission(I2C_ADDR);
      i2c.write(0x40);
      for (byte k = 0; k < width; k++, p++) {
        i2c.write( buffer[p] );
      }
      i2c.endTransmission();
    }
  }
  m_col += width;
}


void LCD_SH1106::draw(const PROGMEM byte* buffer, byte width, byte height)
{
  WriteCommand(SSD1306_SETLOWCOLUMN | 0x0); // low col = 0
  WriteCommand(SSD1306_SETHIGHCOLUMN | 0x0); // hi col = 0
  WriteCommand(SSD1306_SETSTARTLINE | 0x0); // line #0
  const PROGMEM byte *p = buffer;
  height >>= 3;
  width >>= 3;
  for (byte i = 0; i < height; i++) {
    // send a bunch of data in one xmission
    WriteCommand(0xB0 + i + m_row);//set page address
    WriteCommand(m_col & 0xf);//set lower column address
    WriteCommand(0x10 | (m_col >> 4));//set higher column address
    for(byte j = 0; j < 8; j++){
      i2c.beginTransmission(I2C_ADDR);
      i2c.write(0x40);
      for (byte k = 0; k < width; k++, p++) {
        i2c.write(pgm_read_byte(p));
      }
      i2c.endTransmission();
    }
  }
  m_col += width;
}


