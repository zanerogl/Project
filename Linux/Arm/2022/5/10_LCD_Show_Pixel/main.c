#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

/**********************************************************************
 * 函数名称： lcd_put_pixel
 * 功能描述： 在LCD指定位置上输出指定颜色（描点）
 * 输入参数： x坐标，y坐标，颜色
 * 输出参数： 无
 * 返 回 值： 会
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2022/05/10	     V1.0	  Zanerogl	      创建
 ***********************************************************************/

void LCD_Put_Pixel(int x, int y, unsigned int color);

/*变量*/
static int LCD_fd;
static unsigned char *LCD_memory;
static int LCD_screen_size;
static unsigned int pixel_width;
static unsigned int Line_width;
static struct fb_var_screeninfo var;	//Linux内核中使用struct fb_var_screeninfo来描述可修改的LCD显示参数，如分辨率和像素比特数等。

int main(){
 	
		
	/*用unsinged char* 的原因：
		虽然不同类型的指针变量在相同的操作系统中所站的字节数相同（32位占4字节，64位占8字节）
	但是在指针解引用后所占的内存大小是不同的，*(char)占2字节而*(int)占4字节				*/
	
	LCD_fd = open("/dev/fb0", O_RDWR);
	if(LCD_fd < 0){
		printf("Can not open /dev/fb0\n");
		return -1;
	}
	
	//用ioctl函数调用LCD驱动文件
	if( ioctl(LCD_fd, 			//文件描述符
		FBIOGET_VSCREENINFO, 	//与驱动程序交互的命令，FBIOGET_VSCREENINFO是和FarmBuffer交互的
		&var					//var是参数，是根据参数二而定的，此处为根据FBIOGET_VSCREENINFO返回的输出数据
		) ){
		printf("Can not get var\n");
		return -1;
	}
	
	//获取LED屏幕的大小（总占的字节大小）
	pixel_width = var.bits_per_pixel / 8;	//每个像素点的位数/8 = 每个像素点的占的字节数
	LCD_screen_size = (var.xres * var.yres) * var.bits_per_pixel / 8;	//像素点个数 * 每个像素点的占的字节数
	
	//定义线的长度
	Line_width = var.xres * var.bits_per_pixel / 8;	//因为画的是横线所以用xres乘
	
	//内存映射
	LCD_memory = mmap(NULL, LCD_screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, LCD_fd, 0);
	if(LCD_memory == (unsigned char *)-1){
		printf("Can not mmap\n");
		return -1;
	}
	
	//清屏，全部设为黑色
	memset(LCD_memory, 			//指针指向的一片要被设定的内存
			0, 					//要设定的每一位
			LCD_screen_size		//内存的前LCD_screen_size个字节
			);
	
	//画长200的横线和竖线
	for(int i=0; i<200; i++){
		LCD_Put_Pixel(var.xres/2+i, var.yres/3, 0xFF0000);	//在屏幕x/2+i到y/3位置显示红色
		LCD_Put_Pixel(var.xres/2, var.yres/3+i, 0xFF0000);
	}
	
	//释放内存
	munmap(LCD_memory , LCD_screen_size);
	return 0;
	
}

void LCD_Put_Pixel(int x, int y, unsigned int color){
	unsigned char *pen_8 = LCD_memory + y * Line_width + x * pixel_width;
	unsigned short *pen_16;
	unsigned int *pen_32;
	
	unsigned int red, green, blue;
	
	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;
	
	switch(var.bits_per_pixel)
	{
		case 8:
		{
			*pen_8 = color;
			break;
		}
		case 16:
		{
			/*565*/
			red   = (color >> 16) & 0xff;
			green = (color >> 8) & 0xff;
			blue  = (color >> 0) & 0xff;
			color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			*pen_16 = color;
			break;
		}
		case 32:
		{
			*pen_32 = color;
			break;
		}
		default:
		{
			printf("can't surport %dbpp\n", var.bits_per_pixel);
			break;
		}
	}
}
