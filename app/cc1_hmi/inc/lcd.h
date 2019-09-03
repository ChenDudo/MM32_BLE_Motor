////////////////////////////////////////////////////////////////////////////////
#ifndef __LCD_H
#define __LCD_H
////////////////////////////////////////////////////////////////////////////////
#include "hal_spi.h"

////////////////////////////////////////////////////////////////////////////////
#define LCD_TOTAL_BUF_SIZE  (240*240*2)  //115200
#define LCD_Buf_Size 1152

#define LCD_Width   240
#define LCD_Height  240

#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40
#define BRRED            0XFC07
#define GRAY             0X8430

#define DARKBLUE         0X01CF
#define LIGHTBLUE        0X7D7C
#define GRAYBLUE         0X5458

#define LIGHTGREEN       0X841F
#define LIGHTGRAY        0XEF5B
#define LGRAY            0XC618

#define LGRAYBLUE        0XA651
#define LBBLUE           0X2B12
#define CHENDO           0xE79C

#define LCD_CS(n)       (n ? GPIO_SetBits(GPIOC, GPIO_Pin_11) : GPIO_ResetBits(GPIOC, GPIO_Pin_11))
//#define   LCD_DC(n)       (n ? GPIO_SetBits(GPIOC, GPIO_Pin_12) : GPIO_ResetBits(GPIOC, GPIO_Pin_12))
#define LCD_BS(n)       (n ? GPIO_SetBits(GPIOB, GPIO_Pin_9) : GPIO_ResetBits(GPIOB, GPIO_Pin_9))

////////////////////////////////////////////////////////////////////////////////
#ifdef _LCD_C_
#define GLOBAL

u16 POINT_COLOR = LIGHTGRAY;
u16 BACK_COLOR  = BLACK;

#else
#define GLOBAL extern

GLOBAL u16  POINT_COLOR;
GLOBAL u16  BACK_COLOR;

#endif

GLOBAL u8 lcd_buf[LCD_Buf_Size];

#undef GLOBAL

////////////////////////////////////////////////////////////////////////////////
u8 lcd_init();
void lcdTick();
char* itoa(int num,char* str,int radix);
void lcd_clear(u16 color);
void lcd_drawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void lcd_drawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void lcd_drawCircle(u16 x0, u16 y0, u8 r);
void lcd_showNum(u16 x, u16 y, u32 num, u8 len, u8 size);
void lcd_showChar(u16 x, u16 y, char chr, u8 size);
void lcd_showxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode);
void lcd_showString(u16 x, u16 y, u16 width, u16 height, u8 size, char *p);

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
