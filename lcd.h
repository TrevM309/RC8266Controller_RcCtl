#ifndef __LCD_H
#define __LCD_H		

#include <Arduino.h>
#include <stdlib.h>

#define USE_HORIZONTAL 2  //Set horizontal or vertical screen display 0 or 1 for vertical screen 2 or 3 for horizontal screen
#define HAS_BLK_CNTL    1

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 80
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 80
#endif

//typedef unsigned char u8;
//typedef unsigned int u16;
//typedef unsigned long u32;    			

#define tft_scl 14 //hsck ok
#define tft_rs  2  //dc
#define tft_sda 13 //hmosi ok
#define tft_rst 16 //ok
//#define tft_cs  5 // hard wired to gnd
//#define tft_bl  12

// #define LED_ON gpio_bit_reset(GPIOC,GPIO_PIN_13)
// #define LED_OFF gpio_bit_set(GPIOC,GPIO_PIN_13)

#define LED_ON 
#define LED_OFF 

#define SPI0_CFG 1  //hardware spi
// #define SPI0_CFG 2  //hardware spi dma
// #define SPI0_CFG 3  //software spi

#define FRAME_SIZE  25600

//-----------------OLED端口定义---------------- 
#if SPI0_CFG == 1
#define OLED_SCLK_Clr() 
#define OLED_SCLK_Set() 

#define OLED_SDIN_Clr()
#define OLED_SDIN_Set()

//#define OLED_CS_Clr() digitalWrite(tft_cs,LOW)     //CS PB2
//#define OLED_CS_Set() digitalWrite(tft_cs,HIGH)
#define OLED_CS_Clr() 
#define OLED_CS_Set() 
#elif SPI0_CFG == 2
#define OLED_SCLK_Clr() 
#define OLED_SCLK_Set() 

#define OLED_SDIN_Clr()
#define OLED_SDIN_Set()

#define OLED_CS_Clr()
#define OLED_CS_Set()
#else /* SPI0_CFG */
  // leave empty
#endif /* SPI0_CFG */

#define OLED_RST_Clr() digitalWrite(tft_rst,LOW)     //RES PB1
#define OLED_RST_Set() digitalWrite(tft_rst,HIGH)

#define OLED_DC_Clr() digitalWrite(tft_rs,LOW)      //DC PB0
#define OLED_DC_Set() digitalWrite(tft_rs,HIGH)


#if     HAS_BLK_CNTL
#define OLED_BLK_Clr()  digitalWrite(tft_bl,LOW)  //BLK
#define OLED_BLK_Set()  //digitalWrite(tft_bl,HIGH)
#else
#define OLED_BLK_Clr()
#define OLED_BLK_Set()
#endif

#define OLED_CMD  0	//Write command
#define OLED_DATA 1	//Write data

extern  u16 BACK_COLOR;   //Background color
extern unsigned char image[12800];

void LCD_Writ_Bus(u8 dat);
void LCD_WR_DATA8(u8 dat);
void LCD_WR_DATA(u16 dat);
void LCD_WR_REG(u8 dat);
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);
void Lcd_Init(void);
void LCD_Clear(u16 Color);
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color);
void LCD_DrawPoint(u16 x,u16 y,u16 color);
void LCD_DrawPoint_big(u16 x,u16 y,u16 color);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color,u8 size);
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color,u8 size);
void LCD_Printf(u16 x, u16 y, u16 color,u8 size, char* fmt, ...);
u32 mypow(u8 m,u8 n);
void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color);
void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color);
void LCD_ShowPicture(u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_ShowLogo(void);


//Brush color
#define LCD_WHITE      0xFFFF
#define LCD_BLACK      0x0000	  
#define LCD_BLUE       0x001F  
#define LCD_BRED       0XF81F
#define LCD_GRED 			 0XFFE0
#define LCD_GBLUE			 0X07FF
#define LCD_RED      	 0xF800
#define LCD_MAGENTA  	 0xF81F
#define LCD_GREEN    	 0x07E0
#define LCD_CYAN     	 0x7FFF
#define LCD_YELLOW   	 0xFFE0
#define LCD_BROWN			 0XBC40 //brown
#define LCD_BRRED		   0XFC07 //maroon
#define LCD_GRAY 			 0X8430 //gray
//GUI color
#define LCD_DARKBLUE 	 0X01CF	//navy blue
#define LCD_LIGHTBLUE	 0X7D7C	//light blue 
#define LCD_GRAYBLUE 	 0X5458 //gray blue
//The above three colors are the colors of PANEL
 
#define LCD_LIGHTGREEN 0X841F //light green
#define LCD_LGRAY	     0XC618 //Light gray (PANNEL), form background color

#define LCD_LGRAYBLUE  0XA651 //Light gray blue (middle layer color)
#define LCD_LBBLUE     0X2B12 //Light brown blue (inverted color of selected item)
					  		 
#endif  
	 
	 
