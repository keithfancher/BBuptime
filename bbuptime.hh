//  bbuptime.hh for bbuptime - a tool for displaying uptime in X11.
//
//  Copyright (c) 2002 by Keith Fancher <discostoo@punkass.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// (See the included file COPYING / GPL-2.0)
//


#ifndef __MAIN_HH
#define __MAIN_HH

#include "Image.hh"
#include "Basewindow.hh"
#include "resource.hh"

#define LEFT_BUTTON 1
#define MIDDLE_BUTTON 2
#define RIGHT_BUTTON 3

class Resource;
class BaseResource;
class Basewindow;

struct PIXMAP {
	Pixmap 	frame;
	Pixmap	label;
};

struct GEOM {
	int height;
	int width;
	int x;
	int y;
};
	

class ToolWindow :public Basewindow
{
public:
  ToolWindow(int argc,char **argv);
  ~ToolWindow(void); 

  XGCValues gcv;
  GC frameGC;

  Window framewin;
  Window labelwin;
  Resource *resource;

  void MakeWindow(bool);
  void CheckDate(bool);
  void EventLoop(void);
  void Reconfigure(void);
  void Redraw(void);

  private:
   bool raised;
   int day,month,year;
   
   // ADDED BY KEITH
   int minutes, hours, days;
   
   PIXMAP  pixmap;
   GEOM frame;
   GEOM label;
};

#endif /* __MAIN_HH */
