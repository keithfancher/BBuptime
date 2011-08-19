//  Basewindow.cc for bbuptime - a tool for displaying uptime in X11.
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
#include "Basewindow.hh"
#include "resource.hh"

Basewindow::Basewindow(int argc,char **argv)
{
	display_name=NULL;
	config_filename=NULL;
	position=NULL;
	withdrawn=False;
	shape=False;	
	server_grabs=0;

	ParseOptions(argc,argv);

	dpy=NULL;
	dpy=XOpenDisplay(display_name);
	if (dpy==NULL)
	{
		fprintf(stderr,"Error can't open display %s\n",display_name);
		exit(1);
	}

	screen=DefaultScreen(dpy);
	v=DefaultVisual(dpy,screen);
	root=RootWindow(dpy,screen);
	depth=DefaultDepth(dpy,screen);
	display_height=DisplayHeight(dpy,screen);
	display_width=DisplayWidth(dpy,screen);

	xres = WidthOfScreen(ScreenOfDisplay(dpy, screen));
	yres = HeightOfScreen(ScreenOfDisplay(dpy, screen));	

	MakeCursor();

}

Basewindow::~Basewindow()
{
}

void Basewindow::MakeCursor(void)
{
	cursor = XCreateFontCursor(dpy, XC_left_ptr);
	XDefineCursor(dpy, root, cursor);
}


void Basewindow::setupImageControl()
{
	image_control = new BImageControl(this);
	image_control->installRootColormap();
}


void Basewindow::grab(void)
{
	if (! server_grabs++);
	   XGrabServer(dpy);

	XSync(dpy, False);
}

void Basewindow::ungrab(void)
{
	if (! --server_grabs);
  	XUngrabServer(dpy);

	if (server_grabs < 0) server_grabs = 0;
}

//CHANGED BY KEITH (re: it was broken)
void Basewindow::ParseOptions(int argc, char * * argv)
{
	int i;

	for(i = 1; i < argc; i++)
	{
		if((!strcmp(argv[i], "-display")) | (!strcmp(argv[i], "-d")))
		{
			if(++i == argc)
      { 
        Usage(); 
        exit(2); 
      }
			display_name = argv[i];
		}
 		else if((!strcmp(argv[i], "-bbconfig")) | (!strcmp(argv[i], "-bb")))
		{
			if(++i == argc)  
      { 
        Usage(); 
        exit(2); 
      }
      if(config_filename || default_config) 
      { 
        Usage(); 
        exit(2); 
      }
      bbconfig_filename = argv[i];
		}
 		else if((!strcmp(argv[i], "-config")) | (!strcmp(argv[i], "-c")))
		{
			if(++i == argc)  
      { 
        Usage(); 
        exit(2); 
      }
      if(bbconfig_filename || default_config) 
      { 
        Usage(); 
        exit(2); 
      }
			config_filename=argv[i];
		}
 		else if((!strcmp(argv[i], "-default")) | (!strcmp(argv[i], "-def")))
		{
      if (bbconfig_filename||config_filename) 
      { 
        Usage(); 
        exit(2); 
      }
      default_config=True;
    }
    else if((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "-version")))
		{
			fprintf(stderr, "%s version %s - (c) 2002 Keith Fancher\n",BBTOOL,BBTOOL_VERSION);
			exit(2);
		}
    else if((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "-help")) || (!strcmp(argv[i], "--help")))
		{
			Usage();
			exit(2);
		}
    else if((!strcmp(argv[i], "-position")) || (!strcmp(argv[i], "-p")))
		{
		  if(++i == argc)  
      { 
        Usage(); 
        exit(2); 
      }
			position=argv[i];
	  }
		else if((!strcmp(argv[i], "-withdrawn")) || (!strcmp(argv[i], "-w")))
		{
		 	withdrawn=True;
		}	
		else if((!strcmp(argv[i], "-shape")) || (!strcmp(argv[i], "-s")))
		{
		 	shape=True;
		}
    
	}
}

// CHANGED BY KEITH (re: it was fugly)
void Basewindow::Usage()
{
	fprintf(stderr, "%s version %s \nCopyright (c) 2002 Keith Fancher <discostoo@punkass.com>\n\n",BBTOOL,BBTOOL_VERSION);
	fprintf(stderr, "%s [-display <display name>] [-config <filename>] [-bbconfig <filename>]\n",BBTOOL);
	fprintf(stderr, "         [-default] [-version] [-help] [-position <position>] [-withdrawn] [-shape]\n\n");
	fprintf(stderr, "-d[isplay] <display name>   X server to connect to\n");
	fprintf(stderr, "-c[onfig] <filename>        Alternate config file\n");
	fprintf(stderr, "-bb[config] <filename>      Alternate Blackbox config file\n");
	fprintf(stderr, "-def[ault]                  Fall back on default configuration\n");
	fprintf(stderr, "-v[ersion]                  Display version number\n");
	fprintf(stderr, "-h[elp]                     Display this help\n");
	fprintf(stderr, "-p[osition] <position>      Set position of window\n");
	fprintf(stderr, "            <position> = [+]|[-]x[+]|[-]y\n");
	fprintf(stderr, "-w[withdrawn]               Place bbtool in the Slit\n");
	fprintf(stderr, "-s[hape]                    Don't display groundplate\n\n");	
}

