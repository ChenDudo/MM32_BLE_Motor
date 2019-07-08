#define _LCD_C_

#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "system_mm32.h"
#include "mm32.h"
#include "common.h"

#include "hal_exti.h"
#include "drv.h"
#include "hal_spi.h"
#include "hal_rcc.h"
#include "hal_gpio.h"
#include "hal_tim.h"

#include "bsp.h"
#include "bsp_spi.h"
#include "drv_spi.h"
#include "bsp_tim.h"

#include "lcd.h"
#include "font.h"
#include "datetime.h"

char* itoa(int num,char* str,int radix)
{
	char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
	unsigned unum;              //存放要转换的整数的绝对值,转换的整数可能是负数
	int i = 0, j, k;            //i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。
 
	//获取要转换的整数的绝对值
	if(radix == 10 && num < 0)    //要转换成十进制数并且是负数
	{
		unum = (unsigned)(-num);    //将num的绝对值赋给unum
		str[i++] = '-';         //在字符串最前面设置为'-'号，并且索引加1
	}
	else unum = (unsigned)num;  //若是num为正，直接赋值给unum
 
	//转换部分，注意转换后是逆序的
	do {
		str[i++] = index[unum % (unsigned)radix];//取unum的最后一位，并设置为str对应位，指示索引加1
		unum /= radix;          //unum去掉最后一位
 
	} while(unum);              //直至unum为0退出循环
 
	str[i] = '\0';              //在字符串最后添加'\0'字符，c语言字符串以'\0'结束。
 
	//将顺序调整过来
	if(str[0] == '-') 
        k = 1;                  //如果是负数，符号不用调整，从符号后面开始调整
	else
        k = 0;                  //不是负数，全部都要调整
 
	char temp;                  //临时变量，交换两个值时用到
	for (j = k; j <= (i - 1) / 2; j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
	{
		temp = str[j];          //头部赋值给临时变量
		str[j] = str[i - 1 + k - j];//尾部赋值给头部
		str[i - 1 + k - j] = temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
	}
 
	return str;                 //返回转换后的字符串
}

////////////////////////////////////////////////////////////////////////////////
static void lcd_gpioInit()
{
    GPIOB_ClockEnable();
    GPIOC_ClockEnable();

    GPIO_SetBits(GPIOC, GPIO_Pin_11);
    GPIO_AF_Init(GPIOC, GPIO_Pin_11, GPIO_Mode_Out_PP, GPIO_Speed_50MHz, NO_REMAP, GPIO_AF_0); //LCD_spi_CS

    GPIO_Mode_AF_PP_50MHz_Init  (GPIOB, GPIO_Pin_13, NO_REMAP, GPIO_AF_0);      // LCD_spi2_sck
    GPIO_Mode_AF_PP_50MHz_Init  (GPIOB, GPIO_Pin_15, NO_REMAP, GPIO_AF_0);      // LCD_spi2_mosi

    GPIO_AF_Init(GPIOC, GPIO_Pin_12, GPIO_Mode_Out_PP, GPIO_Speed_50MHz, NO_REMAP, GPIO_AF_0); //LCD_CD

    LCD_CS(1);
    LCD_BS(0); //close backlight
}

////////////////////////////////////////////////////////////////////////////////
static void lcd_spiInit()
{
    Set_SPI_Clock(SPI2);

    SPI_InitTypeDef InitStructure;

    InitStructure.SPI_CPOL  = SPI_CPOL_Low;
    InitStructure.SPI_CPHA  = SPI_CPHA_1Edge;
    InitStructure.SPI_DataWidth         = 9;

    InitStructure.SPI_NSS               = SPI_NSS_Soft;
    InitStructure.SPI_Mode              = SPI_Mode_Master;
    InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

    SPI_Init(SPI2, &InitStructure);

    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Tx);
    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Rx);

    SPI_Cmd(SPI2, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void spiWritex(u16 x)
{
    __asm volatile("cpsid i");
    WRITE_REG(SPI2->TDR, x >> 1);
    WRITE_REG(SPI2->TDR, x << 7);
    __asm volatile("cpsie i");
    while(!SPI_GetFlagStatus(SPI2, SPI_FLAG_TXEPT)) {
    }
}

////////////////////////////////////////////////////////////////////////////////
static void lcd_writeCmd(u8 cmd)
{
    LCD_CS(0);
    u16 x = cmd;
    spiWritex(x);
    LCD_CS(1);
}

////////////////////////////////////////////////////////////////////////////////
static void lcd_writeData(u8 data)
{
    LCD_CS(0);
    u16 x = data | 0x100;
    spiWritex(x);
    LCD_CS(1);
}

////////////////////////////////////////////////////////////////////////////////
static void lcd_writeData16(u16 data)
{
    LCD_CS(0);
    spiWritex((data  >> 8)  | 0x100);
    spiWritex((data & 0xff) | 0x100);
    LCD_CS(1);
}

////////////////////////////////////////////////////////////////////////////////
void lcd_addressSet(u16 x1, u16 y1, u16 x2, u16 y2)
{
    lcd_writeCmd(0x2a);
    lcd_writeData(x1 >> 8);
    lcd_writeData(x1);
    lcd_writeData(x2 >> 8);
    lcd_writeData(x2);

    lcd_writeCmd(0x2b);
    lcd_writeData(y1 >> 8);
    lcd_writeData(y1);
    lcd_writeData(y2 >> 8);
    lcd_writeData(y2);

    lcd_writeCmd(0x2C);
}

////////////////////////////////////////////////////////////////////////////////
#if (1)
void lcd_clear(u16 color)
{
    u16 i;

    lcd_addressSet(0, 0, LCD_Width - 1, LCD_Height - 1);

    for (i = 0; i < (LCD_Width * LCD_Height) / 8; i++) {
        lcd_writeData16(color);
        lcd_writeData16(color);
        lcd_writeData16(color);
        lcd_writeData16(color);
        lcd_writeData16(color);
        lcd_writeData16(color);
        lcd_writeData16(color);
        lcd_writeData16(color);
    }
}
#else

////////////////////////////////////////////////////////////////////////////////
void lcd_clear(u16 color)
{
    u16 i, j;
    u8 data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;

    lcd_addressSet(0, 0, LCD_Width - 1, LCD_Height - 1);

    for (j = 0; j < LCD_Buf_Size / 2; j++) {
        lcd_buf[j * 2] =  data[0];
        lcd_buf[j * 2 + 1] =  data[1];
    }

    for (i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_Buf_Size); i++){
        for(j = 0; j < LCD_Buf_Size; j++){
            lcd_writeData(lcd_buf[j]);
        }
    }
}
#endif


////////////////////////////////////////////////////////////////////////////////
void lcd_drawPoint(u16 x, u16 y)
{
    lcd_addressSet(x, y, x, y);
    lcd_writeData16(POINT_COLOR);
}

////////////////////////////////////////////////////////////////////////////////
void LCD_drawColorPoint(u16 x, u16 y,u16 color)
{
    lcd_addressSet(x, y, x, y);
    lcd_writeData16(color);
}

////////////////////////////////////////////////////////////////////////////////
void lcd_drawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if(y1 == y2){
        lcd_addressSet(x1, y1, x2, y2);

        for(u16 x = 0; x < (x2 - x1); x++){
            lcd_writeData16(POINT_COLOR);
        }
        return;
    }

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0) 
        incx = 1;

    else if(delta_x == 0)incx = 0;

    else {
        incx = -1;
        delta_x = -delta_x;
    }

    if(delta_y > 0)
        incy = 1;

    else if(delta_y == 0)
        incy = 0;

    else {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
        distance = delta_x;

    else
        distance = delta_y;

    for(t = 0; t <= distance + 1; t++) {
        lcd_drawPoint(row, col);
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance) {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance) {
            yerr -= distance;
            col += incy;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void lcd_drawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    lcd_drawLine(x1, y1, x2, y1);
    lcd_drawLine(x1, y1, x1, y2);
    lcd_drawLine(x1, y2, x2, y2);
    lcd_drawLine(x2, y1, x2, y2);
}

////////////////////////////////////////////////////////////////////////////////
void lcd_drawCircle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);

    while(a <= b){
        lcd_drawPoint(x0 - b, y0 - a);
        lcd_drawPoint(x0 + b, y0 - a);
        lcd_drawPoint(x0 - a, y0 + b);
        lcd_drawPoint(x0 - b, y0 - a);
        lcd_drawPoint(x0 - a, y0 - b);
        lcd_drawPoint(x0 + b, y0 + a);
        lcd_drawPoint(x0 + a, y0 - b);
        lcd_drawPoint(x0 + a, y0 + b);
        lcd_drawPoint(x0 - b, y0 + a);
        a++;

        if(di < 0)di += 4 * a + 6;
        else{
            di += 10 + 4 * (a - b);
            b--;
        }
        lcd_drawPoint(x0 + a, y0 + b);
    }
}

////////////////////////////////////////////////////////////////////////////////
static u32 lcd_pow(u8 m, u8 n)
{
    u32 result = 1;

    while(n--)result *= m;

    return result;
}

////////////////////////////////////////////////////////////////////////////////
void lcd_showChar(u16 x, u16 y, char chr, u8 size)
{
    u8 temp, t1, t;
    u8 csize;
    u16 colortemp;
    u8 sta;

    chr = chr - ' ';

    if((x > (LCD_Width - size / 2)) || (y > (LCD_Height - size)))   return;

    lcd_addressSet(x, y, x + size / 2 - 1, y + size - 1);

    if ((size == 16) || (size == 32)) {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

        for(t = 0; t < csize; t++) {
            if(size == 16)temp = asc2_1608[chr][t];
            else if(size == 32)temp = asc2_3216[chr][t];
            else return;

            for(t1 = 0; t1 < 8; t1++) {
                if(temp & 0x80) colortemp = POINT_COLOR;
                else colortemp = BACK_COLOR;

                lcd_writeData16(colortemp);
                temp <<= 1;
            }
        }
    }

    else if  (size == 12) {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

        for(t = 0; t < csize; t++) {
            temp = asc2_1206[chr][t];

            for(t1 = 0; t1 < 6; t1++) {
                if(temp & 0x80) colortemp = POINT_COLOR;
                else colortemp = BACK_COLOR;

                lcd_writeData16(colortemp);
                temp <<= 1;
            }
        }
    }

    else if(size == 24) {
        csize = (size * 16) / 8;

        for(t = 0; t < csize; t++) {
            temp = asc2_2412[chr][t];

            if(t % 2 == 0)sta = 8;
            else sta = 4;

            for(t1 = 0; t1 < sta; t1++) {
                if(temp & 0x80) colortemp = POINT_COLOR;
                else colortemp = BACK_COLOR;

                lcd_writeData16(colortemp);
                temp <<= 1;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void lcd_showNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;

    for(t = 0; t < len; t++) {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;

        if(enshow == 0 && t < (len - 1)) {
            if(temp == 0) {
                lcd_showChar(x + (size / 2)*t, y, ' ', size);
                continue;
            }

            else enshow = 1;
        }

        lcd_showChar(x + (size / 2)*t, y, temp + '0', size);
    }
}

////////////////////////////////////////////////////////////////////////////////
void lcd_showxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;

    for(t = 0; t < len; t++) {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;

        if(enshow == 0 && t < (len - 1)) {
            if(temp == 0) {
                if(mode)lcd_showChar(x + (size / 2)*t, y, '0', size);
                else
                    lcd_showChar(x + (size / 2)*t, y, ' ', size);
                continue;
            }
            else enshow = 1;
        }
        lcd_showChar(x + (size / 2)*t, y, temp + '0', size);
    }
}

////////////////////////////////////////////////////////////////////////////////
void lcd_showString(u16 x, u16 y, u16 width, u16 height, u8 size, char *p)
{
    u8 x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' ')) {
        if (x >= width) {
            x = x0;
            y += size;
        }

        if(y >= height)
            break;

        lcd_showChar(x, y, *p, size);
        x += size / 2;
        p++;
    }
}


////////////////////////////////////////////////////////////////////////////////
void initLcdTimer()
{
    BSP_TIM_CH4_GPIO_Configure(TIM4, 0, 0, 1);
    
    COMMON_EnableIpClock(emCLOCK_TIM4);
    
    TIM_TimeBaseInitTypeDef pBase = {
        .TIM_Prescaler         = 0,
        .TIM_Period            = 999,
        .TIM_ClockDivision     = TIM_CKD_DIV1,
        .TIM_CounterMode       = TIM_CounterMode_Up,
        .TIM_RepetitionCounter = 0
    };

    TIM_OCInitTypeDef pOC = {
        .TIM_Pulse        = 60,
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_OCPolarity   = TIM_OCPolarity_High,
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset
    };
    TIM_TimeBaseInit(TIM4, &pBase);

    TIM_OC4Init(TIM4, &pOC);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);    
}

////////////////////////////////////////////////////////////////////////////////
void lcd_desplay()
{
    lcd_drawLine  ( 5, 10, 235, 10);
    lcd_showString(10,  15,230, 24, 24, "MindMotion Nanjing");
    lcd_drawLine  ( 5, 45, 235, 45);
    
    lcd_showString(20, 80,220, 16, 16, "DATE: Y-M-D Hour-Min-Sec");

    lcd_drawRectangle(15, 115, 225, 150);
        
        
        
    lcd_drawLine  (10, 220, 230, 220);
    //lcd_showString(160, 228, 220, 12, 12, "by CD103");
    lcd_showString(18, 228, 220, 12, 12, "MindMotion(NJ) Company Logo Setting");
    
}

char* weekString[7] = {"Sun.", "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat."};

////////////////////////////////////////////////////////////////////////////////
void lcdTick()
{
    char s1[32];
    u16 temp;
    
    temp = gtp.year;
    itoa(temp, s1, 10);
    lcd_showString(30, 170, 210, 24, 24, s1); 
    
    strcpy(s1, "-");
    lcd_showString(90, 170, 210, 24, 24, s1); 
    
    temp = gtp.month;
    if(temp < 10) {
        itoa(0, s1, 10);
        lcd_showString(118, 170, 210, 24, 24, s1);
        itoa(temp, s1, 10);
        lcd_showString(131, 170, 210, 24, 24, s1);
    }
    else {
        itoa(temp, s1, 10);
        lcd_showString(118, 170, 210, 24, 24, s1);
    }
    
    strcpy(s1, "-");
    lcd_showString(150, 170, 210, 24, 24, s1);
    
    temp = gtp.day;
    if(temp < 10) {
        itoa(0, s1, 10);
        lcd_showString(180, 170, 210, 24, 24, s1);
        itoa(temp, s1, 10);
        lcd_showString(193, 170, 210, 24, 24, s1);
    }
    else {
        itoa(temp, s1, 10);
        lcd_showString(180, 170, 210, 24, 24, s1);
    }

//////////////////
    temp = gtp.hours;
    if(temp < 10) {
        itoa(0, s1, 10);
        lcd_showString(30, 120, 210, 24, 24, s1);
        itoa(temp, s1, 10);
        lcd_showString(43, 120, 210, 24, 24, s1);
    }
    else {
        itoa(temp, s1, 10);
        lcd_showString(30, 120, 210, 24, 24, s1);
    }
    
    strcpy(s1, ":");
    lcd_showString(58, 120, 210, 24, 24, s1); 
    
    temp = gtp.minute;
    if(temp < 10) {
        itoa(0, s1, 10);
        lcd_showString(75, 120, 210, 24, 24, s1);
        itoa(temp, s1, 10);
        lcd_showString(88, 120, 210, 24, 24, s1);
    }
    else {
        itoa(temp, s1, 10);
        lcd_showString(75, 120, 210, 24, 24, s1);
    }
    
    strcpy(s1, ":");
    lcd_showString(102, 120, 210, 24, 24, s1);
    
    temp = gtp.second; 
    if(temp < 10) {
        itoa(0, s1, 10);
        lcd_showString(120, 120, 210, 24, 24, s1);
        itoa(temp, s1, 10);
        lcd_showString(133, 120, 210, 24, 24, s1);
    }
    else {
        itoa(temp, s1, 10);
        lcd_showString(120, 120, 210, 24, 24, s1);
    }
    
    lcd_showString(170, 120, 240, 24, 24, weekString[gtp.week]);
}

////////////////////////////////////////////////////////////////////////////////
u8 lcd_init()
{
    initLcdTimer();
    lcd_gpioInit();
    lcd_spiInit();
    LCD_BS(1);

    for(u16 i = 10000; i > 0; i--);
    lcd_writeCmd(0x11);
    for(u16 i = 10000; i > 0; i--);


    /* Memory Data Access Control */
    lcd_writeCmd(0x36);
    lcd_writeData(0x00);

    /* RGB 5-6-5-bit  */
    lcd_writeCmd(0x3A);
    lcd_writeData(0x65);

    /* Porch Setting */
    lcd_writeCmd(0xB2);
    lcd_writeData(0x0C);
    lcd_writeData(0x0C);
    lcd_writeData(0x00);
    lcd_writeData(0x33);
    lcd_writeData(0x33);

    /*  Gate Control */
    lcd_writeCmd(0xB7);
    lcd_writeData(0x72);

    /* VCOM Setting */
    lcd_writeCmd(0xBB);
    lcd_writeData(0x3D);   //Vcom=1.625V

    /* LCM Control */
    lcd_writeCmd(0xC0);
    lcd_writeData(0x2C);

    /* VDV and VRH Command Enable */
    lcd_writeCmd(0xC2);
    lcd_writeData(0x01);

    /* VRH Set */
    lcd_writeCmd(0xC3);
    lcd_writeData(0x19);

    /* VDV Set */
    lcd_writeCmd(0xC4);
    lcd_writeData(0x20);

    /* Frame Rate Control in Normal Mode */
    lcd_writeCmd(0xC6);
    lcd_writeData(0x0F);    //60MHZ

    /* Power Control 1 */
    lcd_writeCmd(0xD0);
    lcd_writeData(0xA4);
    lcd_writeData(0xA1);

    /* Positive Voltage Gamma Control */
    lcd_writeCmd(0xE0);
    lcd_writeData(0xD0);
    lcd_writeData(0x04);
    lcd_writeData(0x0D);
    lcd_writeData(0x11);
    lcd_writeData(0x13);
    lcd_writeData(0x2B);
    lcd_writeData(0x3F);
    lcd_writeData(0x54);
    lcd_writeData(0x4C);
    lcd_writeData(0x18);
    lcd_writeData(0x0D);
    lcd_writeData(0x0B);
    lcd_writeData(0x1F);
    lcd_writeData(0x23);

    /* Negative Voltage Gamma Control */
    lcd_writeCmd(0xE1);
    lcd_writeData(0xD0);
    lcd_writeData(0x04);
    lcd_writeData(0x0C);
    lcd_writeData(0x11);
    lcd_writeData(0x13);
    lcd_writeData(0x2C);
    lcd_writeData(0x3F);
    lcd_writeData(0x44);
    lcd_writeData(0x51);
    lcd_writeData(0x2F);
    lcd_writeData(0x1F);
    lcd_writeData(0x1F);
    lcd_writeData(0x20);
    lcd_writeData(0x23);

    /* Display Inversion On */
    lcd_writeCmd(0x21);

    lcd_writeCmd(0x29);

    lcd_addressSet(0, 0, LCD_Width - 1, LCD_Height - 1);

    lcd_clear(BLACK);

    lcd_desplay();
    
    return 1;
}

