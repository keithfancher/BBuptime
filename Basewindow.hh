//  Basewindow.hh for bbtools
//
//  Copyright (c) 1998-1999 by John Kennis, j.m.b.m.kennis@ele.tue.nl
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
#ifndef __BASEWINDOW_HH
#define __BASEWINDOW_HH

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xresource.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

#include "version.h"
#include "Image.hh"
//#include "resource.hh"

class BImageControl;
class Resource;


class Basewindow {

	public:
		Basewindow(int, char**);
		virtual ~Basewindow();


	/* compatible with BImageControl */
	Display *getDisplay(void) { return dpy; }
	int getDepth(void) { return depth; }
	int getScreenNumber(void) { return screen; }
	Window getRootWindow(void) { return root; }
	Visual *getVisual(void) { return v; }
	void grab(void);
	void ungrab(void);
	int getColorsPerChannel(void) { return colors_per_channel; }
	Bool hasImageDither(void) { return image_dither; }
	void setupImageControl(void);

	enum { B_LeftJustify = 1, B_RightJustify, B_CenterJustify };	
	BImageControl *imageControl(void) {return image_control;}
	const BColor &borderColor(void) { return border_color; }	
	Cursor sessionCursor(void) { return cursor;}
	unsigned int xRes(void) {return	xres;}
	unsigned int yRes(void) {return yres;}

	Display *dpy;
	Visual *v;
	unsigned int depth;
	BImageControl *image_control;
	BColor border_color;

	int colors_per_channel;
	bool image_dither;

		char *position;
	char *config_filename;
	char *nobb_filename;
	char *bbconfig_filename;
	char *spooldir;
	bool default_config;
	bool withdrawn;
	bool shape;

	protected:
		virtual void ParseOptions(int argc,char **argv);

		Cursor cursor;
		GC menuGC;
		GC menuHiBGGC;
		GC menuHiGC;
		GC menuFrameGC;
		unsigned int xres;
		unsigned int yres;
		int screen;
		Window root;
		Window menuwin;
		char *display_name;
		int display_height;
		int display_width;

	private:
		int server_grabs;
		void MakeCursor(void);
		void Usage(void);

};

#endif /* __BASEWINDOW_HH */
