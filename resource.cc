//  resource.cc for bbuptime - a tool to display uptime in X11.
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

#include "resource.hh"
#include "blackboxstyle.hh"

Resource::Resource(ToolWindow *toolwindow): 
  BaseResource(toolwindow)
{
  Load();
}

Resource::~Resource()
{
  Clean();
}

void Resource::Clean()
{
  if (label.font) 
    XFreeFont(bbtool->dpy, label.font);
  if (frame.font) 
    XFreeFont(bbtool->dpy, frame.font);
}


void Resource::LoadBBToolResource(void)
{
  XrmValue value;
  char *value_type;

  if (XrmGetResource(resource_db, "bbuptime.autoConfig",
         "Bbuptime.Autoconfig", &value_type, &value))
  {
      if (! strncasecmp("true", value.addr, value.size))
      {
        style.auto_config = True;
      }
      else
        style.auto_config = False;
  }
  else
    style.auto_config = False;

  SizeAndPosition();
  
  Frame();
  
  Label();

}


void Resource::Frame()
{
  XrmValue value;
  char *value_type;

  readDatabaseTexture("bbuptime.frame","Bbuptime.Frame",BB_FRAME,"Toolbar",
                      "slategrey","darkslategrey",
                      BImage_Raised|BImage_Gradient|BImage_Vertical|
                      BImage_Bevel1,&frame.texture);
  
  if (XrmGetResource(resource_db, "bbuptime.bevelWidth","Bbuptime.BevelWidth",
            &value_type, &value))
  {
    if (sscanf(value.addr, "%u", &frame.bevelWidth) != 1)
      frame.bevelWidth = 4;
    else if (frame.bevelWidth == 0)
      frame.bevelWidth = 4;
  }
  else if (XrmGetResource(resource_db, BB_BEVELWIDTH,"BevelWidth", &value_type,
                          &value))
  {
    if (sscanf(value.addr, "%u", &frame.bevelWidth) != 1)
      frame.bevelWidth = 4;
    else if (frame.bevelWidth == 0)
      frame.bevelWidth = 4;
  }
  else
      frame.bevelWidth = 4;
}


void Resource::SizeAndPosition()
{
  XrmValue value;
  char *value_type;
  unsigned int w,h;
  char positionstring[11];

  if (!(bbtool->withdrawn))
  {
    if (XrmGetResource(resource_db, "bbuptime.withdrawn",
         "Bbuptime.Withdrawn", &value_type, &value))
    {
      if (! strncasecmp("true", value.addr, value.size))
        bbtool->withdrawn = True;
      else
        bbtool->withdrawn = False;
    }  
    else
      bbtool->withdrawn = False;
  }

  if (!(bbtool->shape))
  {
    if (XrmGetResource(resource_db, "bbuptime.shape",
         "Bbuptime.Shape", &value_type, &value))
    {
      if (! strncasecmp("true", value.addr, value.size))
        bbtool->shape = True;
      else
        bbtool->shape = False;
    }  
    else
      bbtool->shape = bbtool->withdrawn;
  }

  
  if (!(bbtool->position))
  {
    if (!(XrmGetResource(resource_db, "bbuptime.position","Bbuptime.Position",
              &value_type, &value)))
      strncpy(positionstring, "-0-0", 5);
    else
      strncpy(positionstring, value.addr, strlen(value.addr)+1);
  }
  else
    strncpy(positionstring, bbtool->position, strlen(bbtool->position)+1);


  position.mask=XParseGeometry(positionstring, &position.x, &position.y, &w, &h);
  if (!(position.mask & XValue))
    position.x=0;
  if (!(position.mask & YValue))
    position.y=0;

  /* need this to compute the height */
  const char *defaultFont = "-*-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*";
  
  if (frame.font)
  {
      XFreeFont(bbtool->dpy, frame.font);
    frame.font = 0;
  }
  
  if (XrmGetResource(resource_db, "bbuptime.heightBy.font","Bbuptime.heightBy.Font",
            &value_type, &value))
  {
      if ((frame.font = XLoadQueryFont(bbtool->dpy, value.addr)) == NULL)
    {
        fprintf(stderr, " blackbox: couldn't load font '%s'\n"
                  " ...  reverting to default font.", value.addr);
        if ((frame.font = XLoadQueryFont(bbtool->dpy, defaultFont)) == NULL)
      {
        fprintf(stderr,"blackbox: couldn't load default font.  please check to\n"
                "make sure the necessary font is installed '%s'\n",
          defaultFont);
        exit(2);
      }
    }
  }
  else if (XrmGetResource(resource_db, BB_FONT,"TitleFont", &value_type, &value))
  {
    if ((frame.font = XLoadQueryFont(bbtool->dpy, value.addr)) == NULL)
    {
        fprintf(stderr, " blackbox: couldn't load font '%s'\n"
                  " ...  reverting to default font.", value.addr);
      if ((frame.font = XLoadQueryFont(bbtool->dpy, defaultFont)) == NULL)
      {
        fprintf(stderr,
          "blackbox: couldn't load default font.  please check to\n"
          "make sure the necessary font is installed '%s'\n",
          defaultFont);
        exit(2);
      }
    }
  }
  else 
  {
      if ((frame.font = XLoadQueryFont(bbtool->dpy, defaultFont)) == NULL)
    {
      fprintf(stderr,
              "blackbox: couldn't load default font.  please check to\n"
              "make sure the necessary font is installed '%s'\n", defaultFont);
      exit(2);
    }
  }
}
  
void Resource::Label(void)
{
  XrmValue value;
  char *value_type;

  //ADDED BY KEITH - LABEL CLICKING SHIT
  if (XrmGetResource(resource_db, "bbuptime.launch1", "Bbuptime.Launch1", &value_type, &value))
  {
    sprintf(label.launch1, "%s", value.addr);
  }
  else
    memset(label.launch1, 0, sizeof(label.launch1));
  
  if (XrmGetResource(resource_db, "bbuptime.launch2", "Bbuptime.Launch2", &value_type, &value))
  {
    sprintf(label.launch2, "%s", value.addr);
  }
  else
    memset(label.launch2, 0, sizeof(label.launch2));
  
  if (XrmGetResource(resource_db, "bbuptime.launch3", "Bbuptime.Launch3", &value_type, &value))
  {
    sprintf(label.launch3, "%s", value.addr);
  }
  else
    memset(label.launch3, 0, sizeof(label.launch3));


  
  /* text-label resources */
  if (XrmGetResource(resource_db, "bbuptime.label.transparent",
         "Bbuptime.label.Transparent", &value_type, &value))
  {
    if (! strncasecmp("true", value.addr, value.size))
      label.transparent = True;
      else
      label.transparent = False;
  }  
  else
    label.transparent = False;
  
  readDatabaseTexture("bbuptime.label", "Bbuptime.Label",BB_LABEL,"Toolbar.Label",
                      "slategrey","darkslategrey",
                      BImage_Sunken|BImage_Gradient|BImage_Vertical|
                      BImage_Bevel1,&label.texture);

    readDatabaseColor("bbuptime.textColor",
                      "Bbuptime.TextColor",
                      BB_LABEL_TEXTCOLOR, "Toolbar.TextColor",
                      "LightGrey",&label.textColor);

    
  const char *defaultFont = "-*-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*";
  
  if (label.font)
  {
    XFreeFont(bbtool->dpy, label.font);
    label.font = 0;
  }
  
  if (XrmGetResource(resource_db, "bbuptime.label.font", "Bbuptime.Label.Font",
            &value_type, &value))
  {
    if ((label.font = XLoadQueryFont(bbtool->dpy, value.addr)) == NULL)
    {
      fprintf(stderr, " blackbox: couldn't load font '%s'\n"
                      " ...  reverting to default font.", value.addr);
        if ((label.font = XLoadQueryFont(bbtool->dpy, defaultFont)) == NULL)
      {
        fprintf(stderr,
          "blackbox: couldn't load default font.  please check to\n"
          "make sure the necessary font is installed '%s'\n",
          defaultFont);
      exit(2);
      }
    }
  }
  else if  (XrmGetResource(resource_db, BB_FONT, "TitleFont", &value_type, &value))
  {
    if ((label.font = XLoadQueryFont(bbtool->dpy, value.addr)) == NULL)
    {
      fprintf(stderr, " blackbox: couldn't load font '%s'\n"
                  " ...  reverting to default font.", value.addr);
      if ((label.font = XLoadQueryFont(bbtool->dpy, defaultFont)) == NULL)
      {
        fprintf(stderr,
          "blackbox: couldn't load default font.  please check to\n"
          "make sure the necessary font is installed '%s'\n",
          defaultFont);
        exit(2);
      }
    }
  }
  else
  {
    if ((label.font = XLoadQueryFont(bbtool->dpy, defaultFont)) == NULL)
    {
      fprintf(stderr,"blackbox: couldn't load default font.  please check to\n"
            "make sure the necessary font is installed '%s'\n", defaultFont);
      exit(2);
    }
  }
}


