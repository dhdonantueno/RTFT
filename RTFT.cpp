/*
  RTFT.cpp - Raspberry Pi library for video output.
  Copyright (C)2015 Daniel Donantueno. All right reserved
  
  This library is an adaptation of the popular UTFT.cpp 
  Arduino/chipKit library support for Color TFT LCD Boards by 
  Rinky-Dink Electronics, Henning Karlsen. Frame Buffer handing
  for the Raspberry Pi platform is based on comments found 
  in http://raspberrycompote.blogspot.com.ar.
  
  Due to the popularity of UTFT library I felt it would be nice to
  adapt that library in order to allow Arduino programs to work on 
  the Raspberry pi platform.

  This library is based in the Frame Buffer technology wich allows
  a very good performance and ease of work.

  Repository https://github.com/dhdonantueno/RTFT.git

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/

#include <RTFT.h>

unsigned char RTFT::init(unsigned short int x, unsigned short int y) { 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	if (x%32) x = x + 32 - (x%32);
	if (y%16) y = y + 16 - (y%16);

    current_color = 0xFF;
    current_back_color = 0;
    _transparent = true;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
    fbp = NULL;
    fbfd = 0;
    screensize = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
      fprintf(stderr,"RTFT Error 01: cannot open framebuffer device.\n");
      return 1;
    }
//    printf("The framebuffer device was opened successfully.\n");

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
      fprintf(stderr,"RTFT Error 02: reading variable information.\n");
      return 2;
    }
//    printf("Original %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, 
//       vinfo.bits_per_pixel );

    // Store for reset (copy vinfo to vinfo_orig)
    memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));
    
    // Change variable info
    // use: 'fbset -depth x' to test different bpps
	vinfo.bits_per_pixel = 16;
	vinfo.xres = x;
	vinfo.yres = y;
	vinfo.xres_virtual = vinfo.xres;
	vinfo.yres_virtual = vinfo.yres;

	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
		fprintf(stderr,"RTFT Error 03: setting variable information.\n");
		return 3;
	}
	
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
      fprintf(stderr,"RTFT Error 04: reading variable information.\n");
      return 4;
    }
//    printf("New %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, 
//       vinfo.bits_per_pixel );

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
      fprintf(stderr,"RTFT Error 05: reading fixed information.\n");
      return 5;
    }

    // map fb to user mem 
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    
    fbp = (char*)mmap(0, 
              screensize, 
              PROT_READ | PROT_WRITE, 
              MAP_SHARED, 
              fbfd, 
              0);

    if (fbp == MAP_FAILED) {
        fprintf(stderr,"RTFT Error 06: Failed to mmap.\n");
        return 6;
    } else {
	clrScr();
        return 0;
    }
}

RTFT::~RTFT() {
	memcpy(&vinfo, &orig_vinfo, sizeof(struct fb_var_screeninfo));
	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
		fprintf(stderr,"RTFT Error 90: setting variable information.\n");
	}
	munmap(fbp,screensize);
}

void RTFT::drawRect(unsigned short int x1, unsigned short int y1, 
unsigned short int x2, unsigned short int y2)
{
	if (x1>x2) swap(unsigned short int, x1, x2);
	if (y1>y2) swap(unsigned short int, y1, y2);

	drawHLine(x1, y1, x2-x1);
	drawHLine(x1, y2, x2-x1);
	drawVLine(x1, y1, y2-y1);
	drawVLine(x2, y1, y2-y1);
}

void RTFT::drawRoundRect(unsigned short int x1, unsigned short int y1, 
unsigned short int x2, unsigned short int y2)
{
	if (x1>x2) swap(unsigned short int, x1, x2);
	if (y1>y2) swap(unsigned short int, y1, y2);
	
	if ((x2-x1)>4 && (y2-y1)>4)	{
		drawPixel(x1+1,y1+1);
		drawPixel(x2-1,y1+1);
		drawPixel(x1+1,y2-1);
		drawPixel(x2-1,y2-1);
		drawHLine(x1+2, y1, x2-x1-4);
		drawHLine(x1+2, y2, x2-x1-4);
		drawVLine(x1, y1+2, y2-y1-4);
		drawVLine(x2, y1+2, y2-y1-4);
	}
}

void RTFT::fillRect(unsigned short int x1, unsigned short int y1, 
unsigned short int x2, unsigned short int y2)
{
	if (x1>x2) swap(unsigned short int, x1, x2);
	if (y1>y2) swap(unsigned short int, y1, y2);

    for (int y = y1; y <= y2 ; y++) {
        drawHLine(x1, y, x2-x1);
    }

}

void RTFT::fillRoundRect(unsigned short int x1, unsigned short int y1, 
unsigned short int x2, unsigned short int y2)
{
	if (x1>x2) swap(unsigned short int, x1, x2);
	if (y1>y2) swap(unsigned short int, y1, y2);

	if ((x2-x1)>4 && (y2-y1)>4)	{
		for (unsigned short int i=0; i<((y2-y1)/2)+1; i++) {
			switch(i) {
			case 0:
				drawHLine(x1+2, y1+i, x2-x1-4);
				drawHLine(x1+2, y2-i, x2-x1-4);
				break;
			case 1:
				drawHLine(x1+1, y1+i, x2-x1-2);
				drawHLine(x1+1, y2-i, x2-x1-2);
				break;
			default:
				drawHLine(x1, y1+i, x2-x1);
				drawHLine(x1, y2-i, x2-x1);
			}
		}
	}
}

void RTFT::drawCircle(unsigned short int x, unsigned short int y, 
unsigned short int radius)
{
	short int f = 1 - radius;
	short int ddF_x = 1;
	short int ddF_y = -2 * radius;
	short int x1 = 0;
	short int y1 = radius;
 
    drawPixel(x, y + radius);
    drawPixel(x, y - radius);
    drawPixel(x + radius, y);
    drawPixel(x - radius, y);
 
	while(x1 < y1) {
		if(f >= 0) {
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;    
        drawPixel(x + x1, y + y1);
        drawPixel(x - x1, y + y1);
        drawPixel(x + x1, y - y1);
        drawPixel(x - x1, y - y1);
        drawPixel(x + y1, y + x1);
        drawPixel(x - y1, y + x1);
        drawPixel(x + y1, y - x1);
        drawPixel(x - y1, y - x1);
	}

}

void RTFT::fillCircle(unsigned short int x, unsigned short int y, 
unsigned short int radius) {
	int r2 = radius * radius;
	for( short int y1=-radius; y1<=0; y1++) {
		int y2 = y1 * y1;

		for( short int x1=-radius; x1<=0; x1++)
			if(x1*x1+y2 <= r2) {
				drawHLine(x+x1, y+y1, 2*(-x1));
				drawHLine(x+x1, y-y1, 2*(-x1));
				break;
			}
	}
}

void RTFT::clrScr() {
    fillScr(0);
}

void RTFT::fillScr(unsigned char r, unsigned char g, unsigned char b) {
    unsigned short int color = ((r&248)<<8 | (g&252)<<3 | (b&248)>>3);
	fillScr(color);
}

void RTFT::fillScr(unsigned short int color) {
    memset(fbp, color, screensize);
}

void RTFT::setColor(unsigned char r, unsigned char g, unsigned char b) {
    current_color = ((r&248)<<8 | (g&252)<<3 | (b&248)>>3);
}

void RTFT::setColor(unsigned short int color) {
    current_color = color;
}

unsigned short int RTFT::getColor() {
    return current_color;
}

void RTFT::setBackColor(unsigned char r, unsigned char g, unsigned char b) {
    current_back_color = ((r&248)<<8 | (g&252)<<3 | (b&248)>>3);
}

void RTFT::setBackColor(unsigned short int color) {
    current_back_color = color;
}

unsigned short int RTFT::getBackColor() {
    return current_back_color;
}

/*
void RTFT::setPixel(word color)
{
	LCD_Write_DATA((color>>8),(color&0xFF));	// rrrrrggggggbbbbb
}
*/

void RTFT::drawPixel(unsigned short int x, unsigned short int y) {
    if (x>vinfo.xres) x=vinfo.xres;
	if (y>vinfo.yres) x=vinfo.yres;
    // calculate the pixel's byte offset inside the buffer
    // note: x * 2 as every pixel is 2 consecutive bytes
    unsigned int pix_offset = x * 2 + y * finfo.line_length;

    // now this is about the same as 'fbp[pix_offset] = value'
    // but a bit more complicated for RGB565
    //unsigned short c = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
    //unsigned short c = ((r / 8) * 2048) + ((g / 4) * 32) + (b / 8);
    // write 'two bytes at once'
    *((unsigned short*)(fbp + pix_offset)) = current_color;
}

void RTFT::drawPixel(unsigned short int x, unsigned short int y, 
unsigned short int color) {
	if (x>vinfo.xres) x=vinfo.xres;
	if (y>vinfo.yres) x=vinfo.yres;
    // calculate the pixel's byte offset inside the buffer
    // note: x * 2 as every pixel is 2 consecutive bytes
    unsigned int pix_offset = x * 2 + y * finfo.line_length;

    // now this is about the same as 'fbp[pix_offset] = value'
    // but a bit more complicated for RGB565
    //unsigned short c = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
    //unsigned short c = ((r / 8) * 2048) + ((g / 4) * 32) + (b / 8);
    // write 'two bytes at once'
    *((unsigned short*)(fbp + pix_offset)) = color;
}

void RTFT::drawLine(unsigned short int x1, unsigned short int y1, 
unsigned short int x2, unsigned short int y2) {
	if (y1==y2)
		drawHLine(x1, y1, x2-x1);
	else if (x1==x2)
		drawVLine(x1, y1, y2-y1);
	else {
		unsigned int	dx = (x2 > x1 ? x2 - x1 : x1 - x2);
		short			xstep =  x2 > x1 ? 1 : -1;
		unsigned int	dy = (y2 > y1 ? y2 - y1 : y1 - y2);
		short			ystep =  y2 > y1 ? 1 : -1;
		int				col = x1, row = y1;

		if (dx < dy) {
			int t = - (dy >> 1);
			while (true) {
                drawPixel(col, row);

				if (row == y2)
					return;
				row += ystep;
				t += dx;
				if (t >= 0) {
					col += xstep;
					t   -= dy;
				}
			} 
		} else {
			int t = - (dx >> 1);
			while (true) {
                drawPixel(col, row);

				if (col == x2) return;
				col += xstep;
				t += dy;
				if (t >= 0) {
					row += ystep;
					t   -= dx;
				}
			} 
		}
	}
}

void RTFT::drawHLine(unsigned short int x, unsigned short int y, 
short int l) {
	if (l<0) {
		l = -l;
		x -= l;
	}
    for (short int x1=x; x1<= x+l; x1++) {
        drawPixel(x1, y);
    }
}

void RTFT::drawVLine(unsigned short int x, unsigned short int y, 
short int l) {
	if (l<0) {
		l = -l;
		y -= l;
	}
    for (short int y1=y; y1<= y+l; y1++) {
        drawPixel(x, y1);
    }
}

void RTFT::printChar(unsigned char c, unsigned short int x, 
unsigned short int y) {
	unsigned char i,ch;
	unsigned short j, fila, columna, idx;
	unsigned short temp; 

	if (!_transparent) {
			idx = 0;
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j++) {
				ch=cfont.font[temp];
				for(i=0;i<8;i++) {   
					fila = y + (idx / cfont.x_size);
					columna = x + (idx % cfont.x_size);
					
					if((ch&(1<<(7-i)))!=0) {
						drawPixel(columna, fila, current_color);
					} else {
						drawPixel(columna, fila, current_back_color);
					}   
					idx++;
				}
				temp++;
			}
	} else {
			idx = 0;
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j++) {
				ch=cfont.font[temp];
				for(i=0;i<8;i++) {   
					fila = y + (idx / cfont.x_size);
					columna = x + (idx % cfont.x_size);
					
					if((ch&(1<<(7-i)))!=0) {
						drawPixel(columna, fila, current_color);
					} 
					idx++;
				}
				temp++;
			}
	}
}

void RTFT::rotateChar(unsigned char c, unsigned short x, 
unsigned short y, int pos, unsigned short deg) {
	unsigned char i,j,ch;
	unsigned short temp; 
	unsigned short newx,newy;
	float radian;
	radian=deg*0.0175;  

	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
	for(j=0;j<cfont.y_size;j++) {
		for (int zz=0; zz<(cfont.x_size/8); zz++) {
			ch=cfont.font[temp+zz]; 
			for(i=0;i<8;i++) {   
				newx=x+(((i+(zz*8)+(pos*cfont.x_size))*cos(radian))-((j)*sin(radian)));
				newy=y+(((j)*cos(radian))+((i+(zz*8)+(pos*cfont.x_size))*sin(radian)));

//				setXY(newx,newy,newx+1,newy+1);
				
				if((ch&(1<<(7-i)))!=0) {
//					setPixel((fch<<8)|fcl);
					drawPixel(newx, newy, current_color);
				} else {
					if (!_transparent)
//						setPixel((bch<<8)|bcl);
						drawPixel(newx, newy, current_back_color);
				}   
			}
		}
		temp+=(cfont.x_size/8);
	}
}

void RTFT::print(char *st, unsigned short int x, unsigned short int y, 
unsigned short int deg) {
	int stl, i;
	stl = strlen(st);

	if (x==RIGHT)
		x=(vinfo.xres)-(stl*cfont.x_size);
	if (x==CENTER)
		x=((vinfo.xres)-(stl*cfont.x_size))/2;
	

	for (i=0; i<stl; i++)
		if (deg==0)
			printChar(*st++, x + (i*(cfont.x_size)), y);
		else
			rotateChar(*st++, x, y, i, deg);
}

void RTFT::printNumI(long num, unsigned short int x, unsigned short int y, 
unsigned char length, char filler) {
	char buf[25];
	char st[27];
	bool neg=false;
	int c=0, f=0;
  
	if (num==0) {
		if (length!=0) {
			for (c=0; c<(length-1); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=0;
		} else {
			st[0]=48;
			st[1]=0;
		}
	} else {
		if (num<0) {
			neg=true;
			num=-num;
		}
	  
		while (num>0) {
			buf[c]=48+(num % 10);
			c++;
			num=(num-(num % 10))/10;
		}
		buf[c]=0;
	  
		if (neg) {
			st[0]=45;
		}
	  
		if (length>(c+neg)) {
			for (int i=0; i<(length-c-neg); i++) {
				st[i+neg]=filler;
				f++;
			}
		}

		for (int i=0; i<c; i++) {
			st[i+neg+f]=buf[c-i-1];
		}
		st[c+neg+f]=0;
	}

	print(st,x,y);
}

void RTFT::printNumF(float num, unsigned char dec, unsigned short int x, 
unsigned short int y, char divider, unsigned short int length, char filler) {
	char st[27];
	bool neg=false;

	if (dec<1)
		dec=1;
	else if (dec>5)
		dec=5;

	if (num<0)
		neg = true;

	_convert_float(st, num, length, dec);

	if (divider != '.') {
		for (unsigned short int i=0; i<sizeof(st); i++)
			if (st[i]=='.')
				st[i]=divider;
	}

	if (filler != ' ') {
		if (neg) {
			st[0]='-';
			for (unsigned short int i=1; i<sizeof(st); i++)
				if ((st[i]==' ') || (st[i]=='-'))
					st[i]=filler;
		} else {
			for (unsigned short int i=0; i<sizeof(st); i++)
				if (st[i]==' ')
					st[i]=filler;
		}
	}
	print(st,x,y);
}

void RTFT::setFont(const unsigned char* font, bool t)
{
	cfont.font=font;
	cfont.x_size=fontbyte(0);
	cfont.y_size=fontbyte(1);
	cfont.offset=fontbyte(2);
	cfont.numchars=fontbyte(3);

	_transparent = t;
}

const unsigned char* RTFT::getFont() {
	return cfont.font;
}

unsigned char RTFT::getFontXsize() {
	return cfont.x_size;
}

unsigned char RTFT::getFontYsize() {
	return cfont.y_size;
}

void RTFT::drawBitmap(unsigned short int x, unsigned short int y, 
unsigned short int sx, unsigned short int sy, bitmapdatatype data) {
	unsigned short col;

			for (unsigned short tc=0; tc<(sx*sy); tc++) {
				col=data[tc];
				short fila    = tc / sx;
				short columna = tc % sx;
				drawPixel(x+columna, y+fila, col);
			}
}

void RTFT::drawBitmap(unsigned short int x, unsigned short int y, 
unsigned short int sx, unsigned short int sy, bitmapdatatype data, unsigned short int deg, unsigned short int rox, unsigned short int roy) {
	unsigned short col;
	int tx, ty, newx, newy;
	double radian;
	radian=deg*0.0175;  

	if (deg==0)
		drawBitmap(x, y, sx, sy, data);
	else {
		for (ty=0; ty<sy; ty++)
			for (tx=0; tx<sx; tx++) {
				col=data[(ty*sx)+tx];

				newx=x+rox+(((tx-rox)*cos(radian))-((ty-roy)*sin(radian)));
				newy=y+roy+(((ty-roy)*cos(radian))+((tx-rox)*sin(radian)));

				drawPixel(newx, newy, col);
			}
	}
}

int RTFT::getDisplayXSize() {
		return vinfo.xres;
}

int RTFT::getDisplayYSize() {
		return vinfo.yres;
}

void RTFT::setDisplayPage(unsigned char page) {
}

void RTFT::setWritePage(unsigned char page) {
}

void RTFT::_convert_float(char *buf, float num, unsigned short int width, 
unsigned char prec) {
	
	char format[10];
	sprintf(format, "%%%i.%if", width, prec);
	sprintf(buf, format, num);
}
