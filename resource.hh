//  resource.hh for bbuptime - a tool to display uptime in X11.
//
//  Copyright (c) 2002 by Keith Fancher <discostoo@punkass.com>
//
//  this program is free software; you can redistribute it and/or modify
//  it under the terms of the gnu general public license as published by
//  the free software foundation; either version 2 of the license, or
//  (at your option) any later version.
//
//  this program is distributed in the hope that it will be useful,
//  but without any warranty; without even the implied warranty of
//  merchantability or fitness for a particular purpose.  see the
//  gnu general public license for more details.
//
//  you should have received a copy of the gnu general public license
//  along with this program; if not, write to the free software
//  foundation, inc., 675 mass ave, cambridge, ma 02139, usa.
//
// (see the included file copying / gpl-2.0)
//


#ifndef __RESOURCE_HH
#define __RESOURCE_HH

#include "bbuptime.hh"
#include "Baseresource.hh"

#define BBTOOLS 1
#define DEFAULT 2
#define BLACKBOX 3

class BaseResource;

struct FRAME {
  int width;
  int height;
  BTexture texture;
  int bevelWidth;
  XFontStruct *font;
};

struct POSITION {
  int x;
  int y;
  int mask;
};

struct LABEL {
  int width;
  int width0, width1, width2, width3;
  int height;
  bool transparent;
  BTexture texture;
  XFontStruct *font;
  BColor textColor;

  //ADDED BY KEITH
  char launch1[120];
  char launch2[120];
  char launch3[120];
};


struct SHOW {
  bool newmail_counter;
  bool totalmail_counter;
  bool label;
  bool envelope;
  bool onlyAtNewMail;
};

struct REPORT {
  bool auto_raise;
  unsigned int check_delay;
  bool euStyle;
};


class Resource : public BaseResource
{

  public:
  	Resource(ToolWindow *);
	~Resource(void);

	struct FRAME frame; 
	struct LABEL label;
	struct POSITION position;
	struct SHOW show;
	struct REPORT report;

  protected:
  	virtual void  LoadBBToolResource(void);

  private:
	void Frame(void);
	void SizeAndPosition(void);
	void MailCheckSettings(void);
	void Label(void);
	void Envelope(void);
	void MailboxDep(void);
	void Menu(void);
	
	void Clean(void);
};
#endif /* __RESOURCE_HH */
