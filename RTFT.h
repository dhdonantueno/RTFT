#ifndef RTFT_H
#define RTFT_H

#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <DefaultFonts.c>

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define PORTRAIT 0
#define LANDSCAPE 1

//*********************************
// COLORS
//*********************************
// VGA color palette
#define VGA_BLACK		0x0000
#define VGA_WHITE		0xFFFF
#define VGA_RED			0xF800
#define VGA_GREEN		0x0400
#define VGA_BLUE		0x001F
#define VGA_SILVER		0xC618
#define VGA_GRAY		0x8410
#define VGA_MAROON		0x8000
#define VGA_YELLOW		0xFFE0
#define VGA_OLIVE		0x8400
#define VGA_LIME		0x07E0
#define VGA_AQUA		0x07FF
#define VGA_TEAL		0x0410
#define VGA_NAVY		0x0010
#define VGA_FUCHSIA		0xF81F
#define VGA_PURPLE		0x8010
#define VGA_TRANSPARENT	0xFFFFFFFF


#define swap(type, i, j) {type t = i; i = j; j = t;}
#define fontbyte(x) cfont.font[x] 
#define bitmapdatatype unsigned short*

struct _current_font
{
	const unsigned char* font;
	unsigned char x_size;
	unsigned char y_size;
	unsigned char offset;
	unsigned char numchars;
};

class RTFT
{
	long int screensize;
	char *fbp;
	int fbfd;
	struct fb_var_screeninfo orig_vinfo;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	_current_font	cfont;
    bool	_transparent;
    unsigned short int     current_color;
	unsigned short int     current_back_color;
	
	public:

~RTFT();     
unsigned char init(unsigned short int x, unsigned short int y);
void drawRect(unsigned short int x1, unsigned short int y1, unsigned short int x2, unsigned short int y2);
void drawRoundRect(unsigned short int x1, unsigned short int y1, unsigned short int x2, unsigned short int y2);
void fillRect(unsigned short int x1, unsigned short int y1, unsigned short int x2, unsigned short int y2);
void fillRoundRect(unsigned short int x1, unsigned short int y1, unsigned short int x2, unsigned short int y2);
void drawCircle(unsigned short int x, unsigned short int y, unsigned short int radius);
void fillCircle(unsigned short int x, unsigned short int y, unsigned short int radius);
void clrScr();
void fillScr(unsigned char r, unsigned char g, unsigned char b);
void fillScr(unsigned short int color);
void setColor(unsigned char r, unsigned char g, unsigned char b);
void setColor(unsigned short int color);
unsigned short int getColor();
void setBackColor(unsigned char r, unsigned char g, unsigned char b);
void setBackColor(unsigned short int color);
unsigned short int getBackColor();
void drawPixel(unsigned short int x, unsigned short int y);
void drawPixel(unsigned short int x, unsigned short int y, unsigned short int color);
void drawLine(unsigned short int x1, unsigned short int y1, unsigned short int x2, unsigned short int y2);
void drawHLine(unsigned short int x, unsigned short int y, short int l);
void drawVLine(unsigned short int x, unsigned short int y, short int l);
void printChar(unsigned char c, unsigned short int x, unsigned short int y);
void rotateChar(unsigned char c, unsigned short x, unsigned short y, int pos, unsigned short deg);
void print(char *st, unsigned short int x, unsigned short int y, unsigned short int deg=0);
void printNumI(long num, unsigned short int x, unsigned short int y, unsigned char length=0, char filler=' ');
void printNumF(float num, unsigned char dec, unsigned short int x, unsigned short int y, char divider='.', unsigned short int length=0, char filler=' ');
void setFont(const unsigned char* font, bool transparent=false);
const unsigned char* getFont();
unsigned char getFontXsize();
unsigned char getFontYsize();
void drawBitmap(unsigned short int x, unsigned short int y, unsigned short int sx, unsigned short int sy, bitmapdatatype data);
void drawBitmap(unsigned short int x, unsigned short int y, unsigned short int sx, unsigned short int sy, bitmapdatatype data, unsigned short int deg, unsigned short int rox, unsigned short int roy);
int getDisplayXSize();
int getDisplayYSize();
void setDisplayPage(unsigned char page);
void setWritePage(unsigned char page);
void _convert_float(char *buf, float num, unsigned short int width, unsigned char prec);
};

#endif
