#include <SPI.h>
#include <stdio.h>
#include <stdarg.h>

#include "lcd.h"
#include "oledfont.h"
u16 BACK_COLOR;   //Background color

/******************************************************************************
       Function description: LCD serial data write function
       Entry data: serial data to be written to dat
       Return value: None
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{
  //SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE2));
  //noInterrupts();
	OLED_CS_Clr();
  SPI.transfer(dat);
	OLED_CS_Set();
  //interrupts();
  //SPI.endTransaction();
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
  //SPI.setDataMode(SPI_MODE2);
  //SPI.setFrequency(10000000);
  //SPI.setBitOrder(MSBFIRST);
  SPI.begin();
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE1));
  SPI.endTransaction();
}

/******************************************************************************
     Function description: LCD initialization function
       Entry data: None
       Return value: None
******************************************************************************/
void Lcd_Init(void)
{

  //pinMode(tft_cs,OUTPUT);
  pinMode(tft_rst,OUTPUT);
  pinMode(tft_rs,OUTPUT);
  //pinMode(tft_bl,OUTPUT);
  //pinMode(tft_scl,OUTPUT);
  //pinMode(tft_sda,OUTPUT);

  //digitalWrite(tft_cs,HIGH);
  digitalWrite(tft_rs,LOW);
  //digitalWrite(tft_bl,LOW);
  digitalWrite(tft_sda,HIGH);
  //digitalWrite(tft_scl,HIGH);
  //delay(1);

  spi_config();
  //SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE2));

  OLED_RST_Clr(); // into reset
  delay(200);
  OLED_RST_Set(); // release reset
  delay(20);
  OLED_BLK_Set(); // backlight on

  LCD_WR_REG(0x11); // turn off sleep mode
  delay(100);

  LCD_WR_REG(0x21); // display inversion mode

  LCD_WR_REG(0xB1); // Set the frame frequency of the full colors normal mode
            // Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))
            // fosc = 850kHz
  LCD_WR_DATA8(0x05); // RTNA
  LCD_WR_DATA8(0x3A); // FPA
  LCD_WR_DATA8(0x3A); // BPA

  LCD_WR_REG(0xB2); // Set the frame frequency of the Idle mode
            // Frame rate=fosc/((RTNB x 2 + 40) x (LINE + FPB + BPB +2))
            // fosc = 850kHz
  LCD_WR_DATA8(0x05); // RTNB
  LCD_WR_DATA8(0x3A); // FPB
  LCD_WR_DATA8(0x3A); // BPB

  LCD_WR_REG(0xB3); // Set the frame frequency of the Partial mode/ full colors
  LCD_WR_DATA8(0x05);  
  LCD_WR_DATA8(0x3A);
  LCD_WR_DATA8(0x3A);
  LCD_WR_DATA8(0x05);
  LCD_WR_DATA8(0x3A);
  LCD_WR_DATA8(0x3A);

  LCD_WR_REG(0xB4);
  LCD_WR_DATA8(0x03);

  LCD_WR_REG(0xC0);
  LCD_WR_DATA8(0x62);
  LCD_WR_DATA8(0x02);
  LCD_WR_DATA8(0x04);

  LCD_WR_REG(0xC1);
  LCD_WR_DATA8(0xC0);

  LCD_WR_REG(0xC2);
  LCD_WR_DATA8(0x0D);
  LCD_WR_DATA8(0x00);

  LCD_WR_REG(0xC3);
  LCD_WR_DATA8(0x8D);
  LCD_WR_DATA8(0x6A);   

  LCD_WR_REG(0xC4);
  LCD_WR_DATA8(0x8D); 
  LCD_WR_DATA8(0xEE); 

  LCD_WR_REG(0xC5);  /*VCOM*/
  LCD_WR_DATA8(0x0E);    

  LCD_WR_REG(0xE0);
  LCD_WR_DATA8(0x10);
  LCD_WR_DATA8(0x0E);
  LCD_WR_DATA8(0x02);
  LCD_WR_DATA8(0x03);
  LCD_WR_DATA8(0x0E);
  LCD_WR_DATA8(0x07);
  LCD_WR_DATA8(0x02);
  LCD_WR_DATA8(0x07);
  LCD_WR_DATA8(0x0A);
  LCD_WR_DATA8(0x12);
  LCD_WR_DATA8(0x27);
  LCD_WR_DATA8(0x37);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x0D);
  LCD_WR_DATA8(0x0E);
  LCD_WR_DATA8(0x10);

  LCD_WR_REG(0xE1);
  LCD_WR_DATA8(0x10);
  LCD_WR_DATA8(0x0E);
  LCD_WR_DATA8(0x03);
  LCD_WR_DATA8(0x03);
  LCD_WR_DATA8(0x0F);
  LCD_WR_DATA8(0x06);
  LCD_WR_DATA8(0x02);
  LCD_WR_DATA8(0x08);
  LCD_WR_DATA8(0x0A);
  LCD_WR_DATA8(0x13);
  LCD_WR_DATA8(0x26);
  LCD_WR_DATA8(0x36);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x0D);
  LCD_WR_DATA8(0x0E);
  LCD_WR_DATA8(0x10);

  LCD_WR_REG(0x3A); // define the format of RGB picture data
  LCD_WR_DATA8(0x05); // 16-bit/pixel

  LCD_WR_REG(0x36);
  if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
  else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
  else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
  else LCD_WR_DATA8(0xA8);

  LCD_WR_REG(0x29); // Display On

}

/******************************************************************************
       Function description: LCD write data
       Entry data: data written by dat
       Return value: None
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	OLED_DC_Set();//Write data
	LCD_Writ_Bus(dat);
}

/******************************************************************************
	   Function description: LCD write data
       Entry data: data written by dat
       Return value: None
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	OLED_DC_Set();//Write data
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}

/******************************************************************************
	   Function description: LCD write command
       Entry data: command written by dat
       Return value: None
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	OLED_DC_Clr();//Write command
	LCD_Writ_Bus(dat);
}

/******************************************************************************
	   Function description: Set start and end addresses
       Entry data: x1, x2 set the start and end addresses of the column
                   y1, y2 set the start and end addresses of the line
       Return value: None
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//Column address settings
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//Row address setting
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//Memory write
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//Column address settings
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//Row address setting
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//Memory write
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//Column address settings
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//Row address setting
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//Memory write
	}
	else
	{
		LCD_WR_REG(0x2a);//Column address settings
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//Row address setting
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//Memory write
	}
}

/******************************************************************************
	   Function description: LCD clear screen function
       Entry data: None
       Return value: None
******************************************************************************/
void LCD_Clear(u16 Color)
{
	u16 i,j;  	
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	  {
			for (j=0;j<LCD_H;j++)
				{
					LCD_WR_DATA(Color);
				}
	  }
}

/******************************************************************************
	   Function description: LCD display Chinese characters
       Entry data: x, y starting coordinates
                   index Chinese character number
                   size font size
       Return value: None
******************************************************************************/
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color)	
{  
	u8 i,j;
	u8 *temp,size1;
	if(size==16){temp=Hzk16;}//选择字号
	if(size==32){temp=Hzk32;}
  LCD_Address_Set(x,y,x+size-1,y+size-1); //设置一个汉字的区域
  size1=size*size/8;//一个汉字所占的字节
	temp+=index*size1;//写入的起始位置
	for(j=0;j<size1;j++)
	{
		for(i=0;i<8;i++)
		{
		 	if((*temp&(1<<i))!=0)//从数据的低位开始读
			{
				LCD_WR_DATA(color);//点亮
			}
			else
			{
				LCD_WR_DATA(BACK_COLOR);//不点亮
			}
		}
		temp++;
	 }
}

/******************************************************************************
	   Function description: LCD draws point
       Entry data: x, y starting coordinates
       Return value: None
******************************************************************************/
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//设置光标位置 
	LCD_WR_DATA(color);
} 

/******************************************************************************
	   Function description: LCD draws a large dot
       Entry data: x, y starting coordinates
       Return value: None
******************************************************************************/
void LCD_DrawPoint_big(u16 x,u16 y,u16 color)
{
	LCD_Fill(x-1,y-1,x+1,y+1,color);
} 

/******************************************************************************
	   Function description: fill color in the specified area
       Entry data: xsta, ysta starting coordinates
                   xend, yend termination coordinates
       Return value: None
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	LCD_Address_Set(xsta,ysta,xend,yend);      //设置光标位置 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//设置光标位置 	    
	} 					  	    
}

/******************************************************************************
	   Function description: draw a line
       Entry data: x1, y1 starting coordinates
                   x2, y2 terminating coordinates
       Return value: None
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

/******************************************************************************
	   Function description: draw a rectangle
       Entry data: x1, y1 starting coordinates
                   x2, y2 terminating coordinates
       Return value: None
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

/******************************************************************************
	   Function description: draw circle
       Entry data: x0, y0 center coordinates
                   r radius
       Return value: None
******************************************************************************/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	// int di;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//Determine whether the points to be drawn are too far away
		{
			b--;
		}
	}
}

/******************************************************************************
	   Function description: display characters
       Entry data: x, y starting point coordinates
                   num characters to display
                   mode 1 superimposed mode 0 non-superimposed mode
       Return value: None
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color,u8 size)
{
    u8 temp;
    u8 pos,t;
	u16 x0=x;
    u8 px,py;  

    if(x>LCD_W-(16*size)||y>LCD_H-(16*size))    return;	    //Settings window		   
	num=num-' ';//Get offset value
	LCD_Address_Set(x,y,x+(8*size)-1,y+(16*size)-1);      //Set cursor position
	if(!mode) //Non-overlapping
	{
		for(pos=0;pos<16;pos++)
		{ 
            for (py = 0; py < size; py++)
            {
                temp=asc2_1608[(u16)num*16+pos];		 //Call 1608 font
                for(t=0;t<8;t++)
                {                 
                    if(temp&0x01)
                    {
                        for(px = 0; px < size; px++)
                        {
                            LCD_WR_DATA(color);
                        }
                    }
                    else 
                    {
                        for(px = 0; px < size; px++)
                        {
                            LCD_WR_DATA(BACK_COLOR);
                        }
                    }
                    temp>>=1;
                    x += size;
                }
                x=x0;
                y++;
            }
        }	
	}else//overlapping mode
	{
		for(pos=0;pos<16;pos++)
		{
		    temp=asc2_1608[(u16)num*16+pos];		 //Call 1608 font
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//Draw a dot   
		        temp>>=1; 
		    }
		}
	}   	   	 	  
}

/******************************************************************************
	   Function description: display string
       Entry data: x, y starting point coordinates
                   *p string start address
       Return value: None
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color,u8 size)
{
    while(*p!='\0')
    {       
        if(x>LCD_W-(16*size)){x=0;y+=(16*size);}
        //if(y>LCD_H-(16*size)){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,0,color,size);
        x+=(8*size);
        p++;
    }  
}

/******************************************************************************
	   Function description: display string
       Entry data: x, y starting point coordinates
                   *p string start address
       Return value: None
******************************************************************************/
void LCD_Printf(u16 x, u16 y, u16 color,u8 size, char* fmt, ...)
{
    char buff[100];
    va_list list = list;
    size_t max = 100;

    va_start(list, fmt);
    vasnprintf(buff,&max,fmt,list);
    va_end(list);
    LCD_ShowString(x,y,(const u8*)buff,color,size);
}

/******************************************************************************
	   Function description: display numbers
       Entry data: base m, n exponent
       Return value: None
******************************************************************************/
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

/******************************************************************************
	   Function description: display numbers
       Entry data: x, y starting point coordinates
                   num number to display
                   len number of digits to display
       Return value: None
******************************************************************************/
void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color)
{         	
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+8*t,y,' ',0,color,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color,1); 
	}
} 

/******************************************************************************
	   Function description: display decimal
       Entry data: x, y starting point coordinates
                   num decimal to display
                   len number of digits to display
       Return value: None
******************************************************************************/
void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color)
{         	
	u8 t,temp;
	// u8 enshow=0;
	u16 num1;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+8*(len-2),y,'.',0,color,1);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color,1);
	}
}

/******************************************************************************
	   Function description: display 160x40 16bit (RGB565) picture
       Entry data: x, y starting point coordinates
       Return value: None
******************************************************************************/
void LCD_ShowPicture(u16 x1,u16 y1,u16 x2,u16 y2)
{
	int i;
	LCD_Address_Set(x1,y1,x2,y2);
	for(i=0;i<12800;i++)
	{ 	
		// LCD_WR_DATA8(image[i*2+1]);
		LCD_WR_DATA8(image[i]);
	}			
}

/*
void LCD_ShowLogo(void)
{
	int i;
	LCD_Address_Set(0,0,159,75);
	for(i=0;i<25600;i++)
	{
		LCD_WR_DATA8(logo_bmp[i]);
	}			
}
*/
