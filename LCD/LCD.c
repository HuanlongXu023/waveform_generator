#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h"
// #include "delay.h"
#include "string.h"

// LCD的画笔颜色和背景色
uint16_t POINT_COLOR = 0x0000; // 画笔颜色
uint16_t BACK_COLOR = 0xFFFF;  // 背景色

// 管理LCD重要参数
// 默认为竖屏
_lcd_dev lcddev;

// 写寄存器(CMD)
// regval:寄存器值
// static void ST7789_WR_REG(volatile uint16_t regval)
void LCD_WR_REG(uint16_t data)
{
	// regval = regval;
	ST7789_REG = data;
	// ST7789_WR_REG(data);
}

// 写数据
// data:要写入的值
// static void ST7789_WR_DATA(volatile uint16_t data)
void LCD_WR_DATA(uint16_t data)
{
	//    data = data;
	ST7789_RAM = data;
	// ST7789_WR_DATA(data);
}

// 写寄存器
// LCD_Reg:寄存器编号
// LCD_RegValue:要写入的值
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);
	LCD_WriteRAM(LCD_RegValue);
}

// 开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}
// LCD写GRAM
// RGB_Code:颜色值
void LCD_WriteRAM(uint16_t RGB_Code)
{
	LCD_WR_DATA(RGB_Code); // 写十六位GRAM
}

// 当mdk -O1时间优化时需要设置
// 延时i
void opt_delay(uint8_t i)
{
	while (i--)
		;
}

// LCD开启显示
void LCD_DisplayOn(void)
{
	LCD_WR_REG(0X29); // 开启显示
}
// LCD关闭显示
void LCD_DisplayOff(void)
{
	LCD_WR_REG(0X28); // 关闭显示
}
// 设置光标位置
// Xpos:横坐标
// Ypos:纵坐标
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(Xpos >> 8);
	LCD_WR_DATA(Xpos & 0XFF);
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(Ypos >> 8);
	LCD_WR_DATA(Ypos & 0XFF);
}

// 画点
// x,y:坐标
// POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x, uint16_t y)
{
	LCD_SetCursor(x, y);	// 设置光标位置
	LCD_WriteRAM_Prepare(); // 开始写入GRAM
	LCD_WriteRAM(POINT_COLOR);
}
// 快速画点
// x,y:坐标
// color:颜色
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
	// 设置光标位置
	LCD_SetCursor(x, y);
	// 写入颜色
	LCD_WriteReg(lcddev.wramcmd, color);
}

// dir:方向选择 	0-0度旋转，1-180度旋转，2-270度旋转，3-90度旋转
void LCD_Display_Dir(uint8_t dir)
{
	if (dir == 0 || dir == 1) // 竖屏
	{
		lcddev.dir = 0; // 竖屏
		lcddev.width = 240;
		lcddev.height = 320;

		lcddev.wramcmd = 0X2C;
		lcddev.setxcmd = 0X2A;
		lcddev.setycmd = 0X2B;

		if (dir == 0) // 0-0度旋转
		{
			LCD_WR_REG(0x36);
			LCD_WR_DATA((0 << 3) | (0 << 7) | (0 << 6) | (0 << 5));
		}
		else // 1-180度旋转
		{
			LCD_WR_REG(0x36);
			LCD_WR_DATA((0 << 3) | (1 << 7) | (1 << 6) | (0 << 5));
		}
	}
	else if (dir == 2 || dir == 3)
	{

		lcddev.dir = 1; // 横屏
		lcddev.width = 320;
		lcddev.height = 240;

		lcddev.wramcmd = 0X2C;
		lcddev.setxcmd = 0X2A;
		lcddev.setycmd = 0X2B;

		if (dir == 2) // 2-270度旋转
		{
			LCD_WR_REG(0x36);
			LCD_WR_DATA((0 << 3) | (1 << 7) | (0 << 6) | (1 << 5));
		}
		else // 3-90度旋转
		{
			LCD_WR_REG(0x36);
			LCD_WR_DATA((0 << 3) | (0 << 7) | (1 << 6) | (1 << 5));
		}
	}

	// 设置显示区域
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(0);
	LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.width - 1) >> 8);
	LCD_WR_DATA((lcddev.width - 1) & 0XFF);
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(0);
	LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.height - 1) >> 8);
	LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}
// 设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
// sx,sy:窗口起始坐标(左上角)
// width,height:窗口宽度和高度,必须大于0!!
// 窗体大小:width*height.
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
	uint16_t twidth, theight;
	twidth = sx + width - 1;
	theight = sy + height - 1;

	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(sx >> 8);
	LCD_WR_DATA(sx & 0XFF);
	LCD_WR_DATA(twidth >> 8);
	LCD_WR_DATA(twidth & 0XFF);
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(sy >> 8);
	LCD_WR_DATA(sy & 0XFF);
	LCD_WR_DATA(theight >> 8);
	LCD_WR_DATA(theight & 0XFF);
}
// 初始化lcd
void LCD_Init(void)
{

	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);

	delay_ms(1);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);

	delay_ms(10);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);

	delay_ms(120);

	LCD_WR_REG(0x11);
	delay_ms(120); // Delay 120ms
	//--------display and color format setting-----------//
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0x3a);
	LCD_WR_DATA(0x05);
	//--------ST7789V Frame rate setting------------//
	LCD_WR_REG(0xb2);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x0c);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x33);
	LCD_WR_DATA(0x33);
	LCD_WR_REG(0xb7);
	LCD_WR_DATA(0x35);
	//-----------ST7789V Power setting---------------//
	LCD_WR_REG(0xbb);
	LCD_WR_DATA(0x28);
	LCD_WR_REG(0xc0);
	LCD_WR_DATA(0x2c);
	LCD_WR_REG(0xc2);
	LCD_WR_DATA(0x01);
	LCD_WR_REG(0xc3);
	LCD_WR_DATA(0x0b);
	LCD_WR_REG(0xc4);
	LCD_WR_DATA(0x20);
	LCD_WR_REG(0xc6);
	LCD_WR_DATA(0x0f);
	LCD_WR_REG(0xd0);
	LCD_WR_DATA(0xa4);
	LCD_WR_DATA(0xa1);
	//------------ST7789V gamma setting-------------//
	LCD_WR_REG(0xe0);
	LCD_WR_DATA(0xd0);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x0f);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x2a);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x55);
	LCD_WR_DATA(0x44);
	LCD_WR_DATA(0x3a);
	LCD_WR_DATA(0x0b);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x20);
	LCD_WR_REG(0xe1);
	LCD_WR_DATA(0xd0);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x0a);
	LCD_WR_DATA(0x0b);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x34);
	LCD_WR_DATA(0x43);
	LCD_WR_DATA(0x4a);
	LCD_WR_DATA(0x2b);
	LCD_WR_DATA(0x1b);
	LCD_WR_DATA(0x1c);
	LCD_WR_DATA(0x22);
	LCD_WR_DATA(0x1f);
	LCD_WR_REG(0x29);
}

// 读LCD数据
// 返回值:读到的值
uint16_t LCD_RD_DATA(void)
{
	volatile uint16_t ram;
	ram = ST7789_RAM;
	return ram;
}

// 读显示屏 ID
uint16_t LCD_Read_ID(uint8_t reg)
{
	LCD_WR_REG(reg);
	LCD_RD_DATA();			   // dummy read
	LCD_RD_DATA();			   // 读到0X00
	lcddev.id = LCD_RD_DATA(); // 读取85
	lcddev.id <<= 8;
	lcddev.id |= LCD_RD_DATA(); // 读取52
	return lcddev.id;
}

// 读取个某点的颜色值
// x,y:坐标
// 返回值:此点的颜色
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
	uint16_t rgb1, rgb2;
	uint16_t r, g, b;

	// 1. 边界检查
	if (x >= lcddev.width || y >= lcddev.height)
		return 0;

	// 2. 设置坐标窗口 (设置为 1x1 像素)
	//    这里复用你代码库中已有的 SetCursor 或 Set_Window
	LCD_SetCursor(x, y);

	// 3. 发送读 GRAM 指令 (0x2E)
	LCD_WR_REG(0x2E);

	// ==========================================
	// 4. 开始读取 (FSMC 模式下直接读变量即可)
	// ==========================================

	// 第1次读取：Dummy Read (假读，必须执行，数据丢弃)
	// 你的库中原本有 LCD_RD_DATA()，它本质就是 return ST7789_RAM;
	(void)LCD_RD_DATA();

	// 第2次读取：读取 R 和 G 分量 (高8位R，低8位G)
	rgb1 = LCD_RD_DATA();

	// 第3次读取：读取 B 分量 (以及下一个像素的R，我们只需要高8位B)
	rgb2 = LCD_RD_DATA();

	// ==========================================
	// 5. 数据组合 (将 RGB666 压缩为 RGB565)
	// ==========================================
	// 假设读取格式：
	// rgb1 = [R7 R6 R5 R4 R3 R2 R1 R0, G7 G6 G5 G4 G3 G2 G1 G0]
	// rgb2 = [B7 B6 B5 B4 B3 B2 B1 B0, ...]

	// 提取高位并对齐
	r = (rgb1 >> 8) & 0xF8; // 取 R 的高5位
	g = (rgb1 >> 3) & 0xFC; // 取 G 的高6位
	b = (rgb2 >> 8) & 0xF8; // 取 B 的高5位

	// 组合成标准的 RGB565 格式返回
	// 格式: RRRRR GGGGGG BBBBB
	return (r << 8) | (g << 3) | (b >> 3);
}

// 清屏函数
// color:要清屏的填充色
void LCD_Clear(uint16_t color)
{
	uint32_t index = 0;
	uint32_t totalpoint = lcddev.width;
	totalpoint *= lcddev.height; // 得到总点数
	LCD_SetCursor(0x00, 0x0000); // 设置光标位置
	LCD_WriteRAM_Prepare();		 // 开始写入GRAM
	for (index = 0; index < totalpoint; index++)
		LCD_WriteRAM(color);
}
// 在指定区域内填充指定颜色
// 区域大小:(xend-xsta+1)*(yend-ysta+1)
// xsta
// color:要填充的颜色
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
	uint16_t i, j;
	uint16_t xlen = 0;
	uint16_t temp;
	if ((lcddev.id == 0X6804) && (lcddev.dir == 1)) // 6804横屏的时候特殊处理
	{
		temp = sx;
		sx = sy;
		sy = lcddev.width - ex - 1;
		ex = ey;
		ey = lcddev.width - temp - 1;
		lcddev.dir = 0;
		lcddev.setxcmd = 0X2A;
		lcddev.setycmd = 0X2B;
		LCD_Fill(sx, sy, ex, ey, color);
		lcddev.dir = 1;
		lcddev.setxcmd = 0X2B;
		lcddev.setycmd = 0X2A;
	}
	else
	{
		xlen = ex - sx + 1;
		for (i = sy; i <= ey; i++)
		{
			LCD_SetCursor(sx, i);	// 设置光标位置
			LCD_WriteRAM_Prepare(); // 开始写入GRAM
			for (j = 0; j < xlen; j++)
				LCD_WriteRAM(color); // 设置光标位置
		}
	}
}
// 在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
// color:要填充的颜色
void LCD_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
	uint16_t height, width;
	uint16_t i, j;
	width = ex - sx + 1;  // 得到填充的宽度
	height = ey - sy + 1; // 高度
	for (i = 0; i < height; i++)
	{
		LCD_SetCursor(sx, sy + i); // 设置光标位置
		LCD_WriteRAM_Prepare();	   // 开始写入GRAM
		for (j = 0; j < width; j++)
			LCD_WriteRAM(color[i * width + j]); // 写入数据
	}
}
// 画线
// x1,y1:起点坐标
// x2,y2:终点坐标
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) // 画线输出
	{
		LCD_DrawPoint(uRow, uCol); // 画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}
// 画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1, y1, x2, y1);
	LCD_DrawLine(x1, y1, x1, y2);
	LCD_DrawLine(x1, y2, x2, y2);
	LCD_DrawLine(x2, y1, x2, y2);
}
// 在指定位置画一个指定大小的圆
//(x,y):中心点
// r    :半径
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
{
	int a, b;
	int di;
	a = 0;
	b = r;
	di = 3 - (r << 1); // 判断下个点位置的标志
	while (a <= b)
	{
		LCD_DrawPoint(x0 + a, y0 - b); // 5
		LCD_DrawPoint(x0 + b, y0 - a); // 0
		LCD_DrawPoint(x0 + b, y0 + a); // 4
		LCD_DrawPoint(x0 + a, y0 + b); // 6
		LCD_DrawPoint(x0 - a, y0 + b); // 1
		LCD_DrawPoint(x0 - b, y0 + a);
		LCD_DrawPoint(x0 - a, y0 - b); // 2
		LCD_DrawPoint(x0 - b, y0 - a); // 7
		a++;
		// 使用Bresenham算法画圆
		if (di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
	}
}
// 在指定位置显示一个字符
// x,y:起始坐标
// num:要显示的字符:" "--->"~"
// size:字体大小 12/16/24
// mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
	uint8_t temp, t1, t;
	uint16_t y0 = y;
	uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); // 得到字体一个字符对应点阵集所占的字节数
	num = num - ' ';												// 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for (t = 0; t < csize; t++)
	{
		if (size == 12)
			temp = asc2_1206[num][t]; // 调用1206字体
		else if (size == 16)
			temp = asc2_1608[num][t]; // 调用1608字体
		else if (size == 24)
			temp = asc2_2412[num][t]; // 调用2412字体
		else
			return; // 没有的字库
		for (t1 = 0; t1 < 8; t1++)
		{
			if (temp & 0x80)
				LCD_Fast_DrawPoint(x, y, POINT_COLOR);
			else if (mode == 0)
				LCD_Fast_DrawPoint(x, y, BACK_COLOR);
			temp <<= 1;
			y++;
			if (y >= lcddev.height)
				return; // 超区域了
			if ((y - y0) == size)
			{
				y = y0;
				x++;
				if (x >= lcddev.width)
					return; // 超区域了
				break;
			}
		}
	}
}
// m^n函数
// 返回值:m^n次方.
uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
		result *= m;
	return result;
}
// 显示数字,高位为0,则不显示
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// color:颜色
// num:数值(0~4294967295);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / LCD_Pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
	}
}
// 显示数字,高位为0,还是显示
// x,y:起点坐标
// num:数值(0~999999999);
// len:长度(即要显示的位数)
// size:字体大小
// mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / LCD_Pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				if (mode & 0X80)
					LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);
				else
					LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01);
	}
}
// 显示字符串
// x,y:起点坐标
// width,height:区域大小
// size:字体大小
//*p:字符串起始地址
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p)
{
	uint8_t x0 = x;
	width += x;
	height += y;
	while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
	{
		if (x >= width)
		{
			x = x0;
			y += size;
		}
		if (y >= height)
			break; // 退出
		LCD_ShowChar(x, y, *p, size, 0);
		x += size / 2;
		p++;
	}
}

// 汉字 16*16
void GUI_DrawFont16(uint16_t x, uint16_t y, uint8_t *s, uint8_t mode)
{
	uint8_t i, j;
	uint16_t k;
	uint16_t HZnum;
	uint16_t x0 = x;
	HZnum = sizeof(tfont16) / sizeof(typFNT_GB16); // 自动统计汉字数目

	for (k = 0; k < HZnum; k++)
	{
		if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
		{
			LCD_Set_Window(x, y, 16, 16);
			LCD_WriteRAM_Prepare();
			for (i = 0; i < 16 * 2; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont16[k].Msk[i] & (0x80 >> j))
							LCD_WriteRAM(POINT_COLOR);
						else
							LCD_WriteRAM(BACK_COLOR);
					}
					else
					{
						// POINT_COLOR=fc;
						if (tfont16[k].Msk[i] & (0x80 >> j))
							LCD_DrawPoint(x, y); // 画一个点
						x++;
						if ((x - x0) == 16)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // 恢复窗口为全屏
}

// 汉字 24*24
void GUI_DrawFont24(uint16_t x, uint16_t y, uint8_t *s, uint8_t mode)
{
	uint8_t i, j;
	uint16_t k;
	uint16_t HZnum;
	uint16_t x0 = x;
	HZnum = sizeof(tfont24) / sizeof(typFNT_GB24); // 自动统计汉字数目

	for (k = 0; k < HZnum; k++)
	{
		if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)))
		{
			LCD_Set_Window(x, y, 24, 24);
			LCD_WriteRAM_Prepare();
			for (i = 0; i < 24 * 3; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont24[k].Msk[i] & (0x80 >> j))
							LCD_WriteRAM(POINT_COLOR);
						else
							LCD_WriteRAM(BACK_COLOR);
					}
					else
					{
						// POINT_COLOR=fc;
						if (tfont24[k].Msk[i] & (0x80 >> j))
							LCD_DrawPoint(x, y); // 画一个点
						x++;
						if ((x - x0) == 24)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // 恢复窗口为全屏
}

// 汉字 32*32
void GUI_DrawFont32(uint16_t x, uint16_t y, uint8_t *s, uint8_t mode)
{
	uint8_t i, j;
	uint16_t k;
	uint16_t HZnum;
	uint16_t x0 = x;
	HZnum = sizeof(tfont32) / sizeof(typFNT_GB32); // 自动统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)))
		{
			LCD_Set_Window(x, y, 32, 32);
			LCD_WriteRAM_Prepare();
			for (i = 0; i < 32 * 4; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont32[k].Msk[i] & (0x80 >> j))
							LCD_WriteRAM(POINT_COLOR);
						else
							LCD_WriteRAM(BACK_COLOR);
					}
					else
					{
						// POINT_COLOR=fc;
						if (tfont32[k].Msk[i] & (0x80 >> j))
							LCD_DrawPoint(x, y); // 画一个点
						x++;
						if ((x - x0) == 32)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // 恢复窗口为全屏
}

// 显示汉字或者字符串

/**
 * @brief 显示字符串
 *
 * @param x 起点x坐标
 * @param y 起点y坐标
 * @param str 字符串指针
 * @param size 字体大小
 * @param mode 显示模式 叠加方式(1)还是非叠加方式(0)
 */
void Show_Str(uint16_t x, uint16_t y, uint8_t *str, uint8_t size, uint8_t mode)
{
	uint16_t x0 = x;
	uint8_t bHz = 0; // 字符或者中文

	// ===================== 新增：非法指针防护（必加！）=====================
	if (str == NULL || (uint32_t)str < 0x08000000 || (uint32_t)str > 0x30000000)
	{
		return; // 直接过滤野指针，防止崩溃
	}
	// =====================================================================

	while (*str != 0) // 数据未结束
	{
		// ===================== 新增：再次防御检查 =====================
		if (str == NULL || (uint32_t)str < 0x08000000)
			return;
		// ==============================================================

		if (!bHz)
		{
			if (x > (lcddev.width - size / 2) || y > (lcddev.height - size))
				return;
			if (*str > 0x7F) // 修改：更标准的中文判断（>0x7F）
				bHz = 1;	 // 中文
			else			 // 字符
			{
				if (*str == 0x0D) // 换行符号
				{
					y += size;
					x = x0;
					str++;
				}
				else
				{
					if (size >= 24) // 字库中没有集成12X24 16X32的英文字体,用8X16代替
					{
						LCD_ShowChar(x, y, *str, 24, mode);
						x += 12;
					}
					else
					{
						LCD_ShowChar(x, y, *str, size, mode);
						x += size / 2;
					}
					str++;
				}
			}
		}
		else // 中文
		{
			if (x > (lcddev.width - size) || y > (lcddev.height - size))
				return;

			bHz = 0;

			// ===================== 修复：中文处理前必须判断是否到结尾 =====================
			if (*(str) == 0 || *(str + 1) == 0)
				break;
			// ==============================================================================

			if (size == 32)
				GUI_DrawFont32(x, y, str, mode);
			else if (size == 24)
				GUI_DrawFont24(x, y, str, mode);
			else
				GUI_DrawFont16(x, y, str, mode);

			str += 2; // 汉字偏移
			x += size;
		}
	}
}

// 显示40*40图片
void Gui_Drawbmp16(uint16_t x, uint16_t y, const unsigned char *p) // 显示40*40图片
{
	int i;
	unsigned char picH, picL;
	LCD_Set_Window(x, y, 40, 40);
	LCD_WriteRAM_Prepare();

	for (i = 0; i < 40 * 40; i++)
	{
		picL = *(p + i * 2); // 数据低位在前
		picH = *(p + i * 2 + 1);
		LCD_WriteRAM(picH << 8 | picL);
	}
	LCD_Set_Window(0, 0, lcddev.width, lcddev.height); // 恢复显示窗口为全屏
}

// 居中显示
void Gui_StrCenter(uint16_t x, uint16_t y, uint8_t *str, uint8_t size, uint8_t mode)
{
	uint16_t x1;
	uint16_t len = strlen((const char *)str);
	if (size > 16)
	{
		x1 = (lcddev.width - len * (size / 2)) / 2;
	}
	else
	{
		x1 = (lcddev.width - len * 8) / 2;
	}

	Show_Str(x + x1, y, str, size, mode);
}

void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);	// 清屏
	POINT_COLOR = BLUE; // 设置字体为蓝色
	BACK_COLOR = WHITE;
	LCD_ShowString(lcddev.width - 24, 0, 200, 16, 16, "RST"); // 显示清屏区域
	POINT_COLOR = RED;										  // 设置画笔蓝色
}
////////////////////////////////////////////////////////////////////////////////
// 电容触摸屏专有部分
// 画水平线
// x0,y0:坐标
// len:线长度
// color:颜色
void gui_draw_hline(uint16_t x0, uint16_t y0, uint16_t len, uint16_t color)
{
	if (len == 0)
		return;
	LCD_Fill(x0, y0, x0 + len - 1, y0, color);
}
// 画实心圆
// x0,y0:坐标
// r:半径
// color:颜色
void gui_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	uint32_t i;
	uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
	uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
	uint32_t x = r;
	gui_draw_hline(x0 - r, y0, 2 * r, color);
	for (i = 1; i <= imax; i++)
	{
		if ((i * i + x * x) > sqmax) // draw lines from outside
		{
			if (x > imax)
			{
				gui_draw_hline(x0 - i + 1, y0 + x, 2 * (i - 1), color);
				gui_draw_hline(x0 - i + 1, y0 - x, 2 * (i - 1), color);
			}
			x--;
		}
		// draw lines from inside (center)
		gui_draw_hline(x0 - x, y0 + i, 2 * x, color);
		gui_draw_hline(x0 - x, y0 - i, 2 * x, color);
	}
}

// 画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
// size：线条的粗细程度
// color：线条的颜色
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
	uint16_t t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	if (x1 < size || x2 < size || y1 < size || y2 < size)
		return;
	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) // 画线输出
	{
		gui_fill_circle(uRow, uCol, size, color); // 画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

/*****************************************************************************
 * @name  : LCD_Draw_DashedLine
 * @brief : 画虚线
 * @param : x1,y1:     起点坐标
 *          x2,y2:     终点坐标
 *          color:     虚线颜色
 *          dashLength:实线部分的长度 (像素数)
 *          gapLength: 空白部分的长度 (像素数)
 * @retval: None
 *****************************************************************************/
void LCD_Draw_DashedLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
						 uint16_t color, uint8_t dashLength, uint8_t gapLength)
{
	uint16_t t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	uint16_t dash_count = 0;				  // 虚线周期计数器
	uint16_t period = dashLength + gapLength; // 计算一个完整周期的长度

	// 防止除零或无意义的参数，如果周期为0则退化为画点
	if (period == 0)
		period = 1;

	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;

	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}

	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}

	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;

	for (t = 0; t <= distance + 1; t++) // 画线输出
	{
		// 判断当前是在实线部分还是空白部分
		if (dash_count < dashLength)
		{
			// 使用 Fast_DrawPoint 直接传入颜色，避免修改全局 POINT_COLOR
			LCD_Fast_DrawPoint(uRow, uCol, color);
		}

		// 更新计数器
		dash_count++;
		if (dash_count >= period)
		{
			dash_count = 0; // 重置周期
		}

		// Bresenham 算法坐标更新
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

/*****************************************************************************
 * @name  : LCD_Draw_RoundRect
 * @brief : 画圆角矩形边框 (带颜色参数)
 * @param : x1,y1: 左上角坐标
 *          x2,y2: 右下角坐标
 *          r:     圆角半径
 *          color: 边框颜色
 * @retval: None
 *****************************************************************************/
void LCD_Draw_RoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color)
{
	// 检查参数有效性
	if (x1 >= x2 || y1 >= y2)
		return;
	if (r > (x2 - x1) / 2 || r > (y2 - y1) / 2)
		return;

	// 保存原来的全局画笔颜色，用完后恢复，避免影响其他函数
	uint16_t original_color = POINT_COLOR;
	POINT_COLOR = color;

	// 1. 画四条直线边 (连接圆角的部分)
	LCD_DrawLine(x1 + r, y1, x2 - r, y1); // 上边
	LCD_DrawLine(x1 + r, y2, x2 - r, y2); // 下边
	LCD_DrawLine(x1, y1 + r, x1, y2 - r); // 左边
	LCD_DrawLine(x2, y1 + r, x2, y2 - r); // 右边

	// 2. 画四个角的四分之一圆 (使用Bresenham算法)
	int a, b;
	int di;
	a = 0;
	b = r;
	di = 3 - (r << 1);

	// 定义四个角的圆心坐标
	// 0:左上, 1:右上, 2:右下, 3:左下
	uint16_t x0[4] = {x1 + r, x2 - r, x2 - r, x1 + r};
	uint16_t y0[4] = {y1 + r, y1 + r, y2 - r, y2 - r};

	while (a <= b)
	{
		// 左上角 (第二象限四分之一圆)
		LCD_DrawPoint(x0[0] - a, y0[0] - b);
		LCD_DrawPoint(x0[0] - b, y0[0] - a);

		// 右上角 (第一象限四分之一圆)
		LCD_DrawPoint(x0[1] + a, y0[1] - b);
		LCD_DrawPoint(x0[1] + b, y0[1] - a);

		// 右下角 (第四象限四分之一圆)
		LCD_DrawPoint(x0[2] + a, y0[2] + b);
		LCD_DrawPoint(x0[2] + b, y0[2] + a);

		// 左下角 (第三象限四分之一圆)
		LCD_DrawPoint(x0[3] - a, y0[3] + b);
		LCD_DrawPoint(x0[3] - b, y0[3] + a);

		a++;
		if (di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
	}

	// 恢复原来的画笔颜色
	POINT_COLOR = original_color;
}

/*****************************************************************************
 * @name  : LCD_Fill_RoundRect
 * @brief : 填充圆角矩形
 * @param : x1,y1: 左上角坐标
 *          x2,y2: 右下角坐标
 *          r:     圆角半径
 *          color: 填充颜色
 * @retval: None
 *****************************************************************************/
void LCD_Fill_RoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color)
{
	// 检查参数有效性
	if (x1 >= x2 || y1 >= y2)
		return;
	if (r > (x2 - x1) / 2 || r > (y2 - y1) / 2)
		return;

	// 1. 填充中间的主体矩形 (垂直方向)
	// 这是最中间的一条，避免了圆角区域
	LCD_Fill(x1, y1 + r, x2, y2 - r, color);

	// 2. 填充顶部和底部的矩形 (水平方向)
	// 顶部：在圆角半径高度内，左右各留一个圆角半径的宽度
	LCD_Fill(x1 + r, y1, x2 - r, y1 + r - 1, color);
	// 底部
	LCD_Fill(x1 + r, y2 - r + 1, x2 - r, y2, color);

	// 3. 填充四个角的扇形 (四分之一圆)
	int a, b;
	int di;
	a = 0;
	b = r;
	di = 3 - (r << 1);

	// 四个圆心
	uint16_t x0[4] = {x1 + r, x2 - r, x2 - r, x1 + r};
	uint16_t y0[4] = {y1 + r, y1 + r, y2 - r, y2 - r};

	while (a <= b)
	{
		// 利用现有的 gui_draw_hline 画水平线来填充扇形

		// 左上角 (从圆心向左画水平线)
		gui_draw_hline(x0[0] - a, y0[0] - b, a + 1, color);
		gui_draw_hline(x0[0] - b, y0[0] - a, b + 1, color);

		// 右上角 (从圆心向右画水平线)
		gui_draw_hline(x0[1], y0[1] - b, a + 1, color);
		gui_draw_hline(x0[1], y0[1] - a, b + 1, color);

		// 右下角 (从圆心向右画水平线)
		gui_draw_hline(x0[2], y0[2] + b, a + 1, color);
		gui_draw_hline(x0[2], y0[2] + a, b + 1, color);

		// 左下角 (从圆心向左画水平线)
		gui_draw_hline(x0[3] - a, y0[3] + b, a + 1, color);
		gui_draw_hline(x0[3] - b, y0[3] + a, b + 1, color);

		a++;
		if (di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
	}
}

void LCD_Draw_LineColor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	// 1. 保存当前的全局画笔颜色
	// uint16_t original_color = POINT_COLOR;

	// 2. 设置为新颜色
	POINT_COLOR = color;

	// 3. 直接调用库中原有的画线函数
	LCD_DrawLine(x1, y1, x2, y2);
}

/*****************************************************************************
 * @name  : Show_Str_Color
 * @brief : 显示字符串(带颜色参数, 支持中英文混合)
 * @param : x,y:       起始坐标
 *          *str:      字符串指针
 *          size:      字体大小 (默认16, 可选12/16/24/32)
 *          mode:      显示模式 (默认1-叠加, 0-非叠加/填充背景)
 *          fontColor: 字体颜色
 *          bgColor:   背景颜色 (仅在 mode=0 时生效)
 * @retval: None
 *****************************************************************************/
void Show_Str_Color(uint16_t x, uint16_t y, uint8_t *str, uint16_t fontColor, uint16_t bgColor)
{

	POINT_COLOR = fontColor;
	BACK_COLOR = bgColor;

	Show_Str(x, y, str, 16, 0);
}

/**
 * @brief 显示数值，每隔3位数添加8像素的间隔
 * @param x 起始x坐标
 * @param y 起始y坐标
 * @param num 要显示的数值
 * @param fontColor 字体颜色
 * @param bgColor 背景颜色
 * @return 无
 */
uint8_t Show_Number_With_Separator(uint16_t x, uint16_t y, uint32_t num, uint16_t fontColor, uint16_t bgColor)
{
	char str[20];
	char formatted_str[20];
	int i, j, count;

	// 处理num=0的特殊情况
	if (num == 0)
	{
		Show_Str_Color(x, y, (uint8_t *)"0", fontColor, bgColor);
		return 1;
	}

	// 将数值转换为字符串
	sprintf(str, "%lu", num);

	// 计算字符串长度
	int len = strlen(str);

	// 格式化字符串，每隔3位数添加一个空格
	j = 0;
	count = 0;
	for (i = len - 1; i >= 0; i--)
	{
		formatted_str[j++] = str[i];
		count++;
		if (count % 3 == 0 && i != 0)
		{
			formatted_str[j++] = ' ';
		}
	}

	// 反转字符串
	for (i = 0; i < j / 2; i++)
	{
		char temp = formatted_str[i];
		formatted_str[i] = formatted_str[j - i - 1];
		formatted_str[j - i - 1] = temp;
	}

	formatted_str[j] = '\0';

	// 显示格式化后的字符串，确保颜色参数顺序正确
	Show_Str_Color(x, y, (uint8_t *)formatted_str, fontColor, bgColor);

	return strlen(formatted_str);
}

void Show_StrCenter_Color(uint16_t x, uint16_t y, uint8_t *str, uint16_t fontColor, uint16_t bgColor)
{
	POINT_COLOR = fontColor;
	BACK_COLOR = bgColor;

	uint16_t x1;
	uint16_t len = strlen((const char *)str);

	x1 = (len * 8) / 2;

	// Show_Str_Color(x + x1, y - 8, str, 16, 0);

	Show_Str_Color(x - x1, y - 8, str, fontColor, bgColor);
}
