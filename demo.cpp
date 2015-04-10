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
#include <time.h>

// Declare which fonts we will be using
extern fontdatatype SmallFont[];

RTFT* myGLCD;
time_t seconds;

char *fbp = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

int main(int argc, char* argv[])
{

// Setup the LCD
  myGLCD = new RTFT();
  myGLCD->init(800,480);
  myGLCD->setFont(SmallFont);

  seconds = time(NULL);

while (true)
{

	 
  int buf[798];
  int x, x2;
  int y, y2;
  int r;

// Clear the screen and draw the frame
  myGLCD->clrScr();

  myGLCD->setColor(255, 0, 0);
  myGLCD->fillRect(0, 0, 799, 13);
  myGLCD->setColor(64, 64, 64);
  myGLCD->fillRect(0, 466, 799, 479);
  myGLCD->setColor(255, 255, 255);
  myGLCD->setBackColor(255, 0, 0);
  myGLCD->print("* Raspberry Pi Display Library UTFT compatible *", CENTER, 1);
  myGLCD->setBackColor(64, 64, 64);
  myGLCD->setColor(255,255,0);
  myGLCD->print("<http://GESTION-E.com.ar>", CENTER, 467);

  myGLCD->setColor(0, 0, 255);
  myGLCD->drawRect(0, 14, 799, 465);

// Draw crosshairs
  myGLCD->setColor(0, 0, 255);
  myGLCD->setBackColor(0, 0, 0);
  myGLCD->drawLine(399, 15, 399, 464);
  myGLCD->drawLine(1, 239, 798, 239);
  for (int i=9; i<790; i+=10)
    myGLCD->drawLine(i, 237, i, 242);
  for (int i=19; i<470; i+=10)
    myGLCD->drawLine(397, i, 402, i);

// Draw sin-, cos- and tan-lines  
  myGLCD->setColor(0,255,255);
  myGLCD->print("Sin", 5, 15);
  for (int i=1; i<798; i++)
  {
    myGLCD->drawPixel(i,239+(sin(((i*1.13)*3.14)/180)*200));
  }
  
  myGLCD->setColor(255,0,0);
  myGLCD->print("Cos", 5, 27);
  for (int i=1; i<798; i++)
  {
    myGLCD->drawPixel(i,239+(cos(((i*1.13)*3.14)/180)*200));
  }

  myGLCD->setColor(255,255,0);
  myGLCD->print("Tan", 5, 39);
  for (int i=1; i<798; i++)
  {
    myGLCD->drawPixel(i,239+(int)(tan(((i*1.13)*3.14)/180))%200);
  }

  sleep(10);

  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);
  myGLCD->setColor(0, 0, 255);
  myGLCD->setBackColor(0, 0, 0);
  myGLCD->drawLine(399, 15, 399, 464);
  myGLCD->drawLine(1, 239, 798, 239);

// Draw a moving sinewave
  x=1;
  for (int i=1; i<(798*20); i++) 
  {
    x++;
    if (x==799)
      x=1;
    if (i>799)
    {
      if ((x==399)||(buf[x-1]==239))
        myGLCD->setColor(0,0,255);
      else
        myGLCD->setColor(0,0,0);
      myGLCD->drawPixel(x,buf[x-1]);
    }
    myGLCD->setColor(0,255,255);
    y=239+(sin(((i*1.65)*3.14)/180)*(200-(i / 100)));
    myGLCD->drawPixel(x,y);
    buf[x-1]=y;
  }

  sleep(2);
  
  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);

// Draw some random filled rectangles
  for (int i=0; i<50; i++)
  {
    myGLCD->setColor(rand()%255, rand()%255, rand()%255);
    x=2+rand()%746;
    y=16+rand()%397;
    x2=x+50;
    y2=y+50;
    myGLCD->fillRect(x, y, x2, y2);
  }

  sleep(2);
  
  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);

// Draw some random filled, rounded rectangles
  for (int i=0; i<50; i++)
  {
    myGLCD->setColor(rand()%255, rand()%255, rand()%255);
    x=2+rand()%746;
    y=16+rand()%397;
    x2=x+50;
    y2=y+50;
    myGLCD->fillRoundRect(x, y, x2, y2);
  }
  
  sleep(2);
  
  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);

// Draw some random filled circles
  for (int i=0; i<50; i++)
  {
    myGLCD->setColor(rand()%255, rand()%255, rand()%255);
    x=27+rand()%746;
    y=41+rand()%397;
    myGLCD->fillCircle(x, y, 25);
  }
  
  sleep(2);
  
  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);

// Draw some lines in a pattern
  myGLCD->setColor (255,0,0);
  for (int i=15; i<463; i+=5)
  {
    myGLCD->drawLine(1, i, (i*1.66)-10, 463);
  }
  myGLCD->setColor (255,0,0);
  for (int i=463; i>15; i-=5)
  {
    myGLCD->drawLine(798, i, (short int)((i*1.66)+30), 15);
  }
  myGLCD->setColor (0,255,255);
  for (int i=463; i>15; i-=5)
  {
    myGLCD->drawLine(1, i, (int)(770-(i*1.66)), 15);
  }
  myGLCD->setColor (0,255,255);
  for (int i=15; i<463; i+=5)
  {
    myGLCD->drawLine(798, i, (int)(810-(i*1.66)), 463);
  }
  sleep(2);
  
  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);

// Draw some random circles
  for (int i=0; i<250; i++)
  {
    myGLCD->setColor(rand()%255, rand()%255, rand()%255);
    x=32+rand()%736;
    y=45+rand()%386;
    r=rand()%30;
    myGLCD->drawCircle(x, y, r);
  }

  sleep(2);
  
  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);

// Draw some random rectangles
  for (int i=0; i<250; i++)
  {
    myGLCD->setColor(rand()%255, rand()%255, rand()%255);
    x=2+rand()%796;
    y=16+rand()%447;
    x2=2+rand()%796;
    y2=16+rand()%447;
    myGLCD->drawRect(x, y, x2, y2);
  }

  sleep(2);
  
  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);

// Draw some random rounded rectangles
  for (int i=0; i<250; i++)
  {
     myGLCD->setColor(rand()%255, rand()%255, rand()%255);
    x=2+rand()%796;
    y=16+rand()%447;
    x2=2+rand()%796;
    y2=16+rand()%447;
    myGLCD->drawRoundRect(x, y, x2, y2);
  }

  sleep(2);
  
  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);

  for (int i=0; i<250; i++)
  {
     myGLCD->setColor(rand()%255, rand()%255, rand()%255);
    x=2+rand()%796;
    y=16+rand()%447;
    x2=2+rand()%796;
    y2=16+rand()%447;
    myGLCD->drawLine(x, y, x2, y2);
  }

  sleep(2);
  
  myGLCD->setColor(0,0,0);
  myGLCD->fillRect(1,15,798,464);

  for (int i=0; i<10000; i++)
  {
    myGLCD->setColor(rand()%255, rand()%255, rand()%255);
    myGLCD->drawPixel(2+rand()%796, 16+rand()%447);
  }

  sleep(2);

  myGLCD->fillScr(0, 0, 255);
  myGLCD->setColor(255, 0, 0);
  myGLCD->fillRoundRect(320, 190, 479, 289);
  
  myGLCD->setColor(255, 255, 255);
  myGLCD->setBackColor(255, 0, 0);
  myGLCD->print("That's it!", CENTER, 213);
  myGLCD->print("Restarting in", CENTER, 239);
  myGLCD->print("ten seconds...", CENTER, 252);
  
  myGLCD->setColor(0, 255, 0);
  myGLCD->setBackColor(0, 0, 255);
  myGLCD->print("Runtime: (seconds)", CENTER, 450);
  myGLCD->printNumI(time(NULL)-seconds, CENTER, 465);

  sleep (10);
}
}
