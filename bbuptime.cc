//  bbuptime.cc for bbuptime - a tool for displaying uptime in X11.
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

#include "bbuptime.hh"
#include "version.h"

ToolWindow::ToolWindow(int argc,char **argv) :
  Basewindow(argc,argv)
{
  XrmInitialize();

   resource = new Resource(this);

  MakeWindow(False);
  Redraw();
}

ToolWindow::~ToolWindow()
{
  XUnmapWindow(dpy,framewin);

  /* destroy pixmaps */
  if (pixmap.frame) image_control->removeImage(pixmap.frame);
  if (pixmap.label) image_control->removeImage(pixmap.label);
  
  /* destroy windows */
  XDestroyWindow(dpy,labelwin);
  XDestroyWindow(dpy,framewin);
}

void ToolWindow::Reconfigure(void)
{
  /* destroy pixmaps */
  image_control->removeImage(pixmap.frame);
  image_control->removeImage(pixmap.label);

  resource->Reload();

  MakeWindow(True);

   XClearWindow(dpy, labelwin);
  XClearWindow(dpy, framewin);
  Redraw();
}


void ToolWindow::MakeWindow(bool reconfigure)
{
  XSetWindowAttributes attrib;
  XWMHints wmhints;

  unsigned long create_mask = CWBackPixmap | CWOverrideRedirect | CWCursor | CWEventMask; 

  // CHANGED BY KEITH
  frame.width = XTextWidth(resource->label.font," xx days xx:xx ", strlen(" xx days xx:xx ")) + resource->frame.bevelWidth * 4;
  
  label.width=frame.width - 2 * resource->frame.bevelWidth;
  frame.height=resource->label.font->ascent + resource->label.font->descent + 4 * resource->frame.bevelWidth;
  label.height=frame.height - 2 * resource->frame.bevelWidth;

  if (resource->position.mask & XNegative) 
  {
    resource->position.x = display_width + resource->position.x - frame.width;
  }
  if (resource->position.mask & YNegative)
  {
    resource->position.y = display_height + resource->position.y - frame.height;
  }
  

  if(withdrawn)
  {
    attrib.override_redirect = False;
     wmhints.initial_state = WithdrawnState;
  }
  else
  {
    attrib.override_redirect = True;
    wmhints.initial_state = NormalState;
  }

  attrib.background_pixmap = ParentRelative;

  pixmap.frame = image_control->renderImage(frame.width, frame.height, &resource->frame.texture);
  pixmap.label = image_control->renderImage(label.width, label.height, &resource->label.texture); 
                      
  attrib.cursor = cursor;
  attrib.event_mask = ButtonPressMask | ButtonReleaseMask | ExposureMask | FocusChangeMask | KeyPressMask | StructureNotifyMask;

  if (!reconfigure)
  {
    framewin = XCreateWindow(dpy, root, resource->position.x, resource->position.y, frame.width,
                             frame.height, 0, depth, InputOutput, v, create_mask, &attrib);
  }
  else if (!withdrawn)
  {
    XMoveResizeWindow(dpy, framewin, resource->position.x, resource->position.y, frame.width, frame.height);
  }
  else
  {
    XResizeWindow(dpy,framewin,frame.width,frame.height);
  }

  wmhints.flags = IconWindowHint | StateHint;
  wmhints.icon_window = framewin;
  XSetWMHints(dpy, framewin, &wmhints);

  if (!shape)
  {
    XSetWindowBackgroundPixmap(dpy, framewin, pixmap.frame);
  }

  if (!reconfigure)
  {
    labelwin = XCreateWindow(dpy, framewin, resource->frame.bevelWidth, resource->frame.bevelWidth, label.width, 
                             label.height, 0, depth, InputOutput, v, create_mask, &attrib);
  }
  else
  {
     XMoveResizeWindow(dpy, labelwin, resource->frame.bevelWidth, resource->frame.bevelWidth, label.width, label.height);
  }

  if (!resource->label.transparent)
    XSetWindowBackgroundPixmap(dpy, labelwin, pixmap.label);

  if (!reconfigure)
  {
    gcv.font = resource->label.font->fid;
    gcv.foreground = resource->label.textColor.pixel;
    frameGC = XCreateGC(dpy, labelwin, GCFont | GCForeground, &gcv);
  }
  else
  {
    gcv.font = resource->label.font->fid;
    gcv.foreground = resource->label.textColor.pixel;
    XChangeGC(dpy, frameGC, GCFont | GCForeground, &gcv);
  }

  if (!reconfigure)
  {
    XClearWindow(dpy, framewin);
    XMapWindow(dpy, framewin);  
    XMapSubwindows(dpy, framewin);
  }
  
}

void ToolWindow::CheckDate(bool redraw)
{
  //  ADDED BY KEITH
  FILE * fpUptime = NULL;
  float  fUptime  = 0.0f;
  
  fpUptime = fopen("/proc/uptime", "r");
  if(fpUptime)
  {
    fscanf(fpUptime, "%f", &fUptime);
    fclose(fpUptime);

    days    = (int)(fUptime / 86400);
    hours   = ((int)(fUptime / 3600)) - (days * 24);
    minutes = ((int)(fUptime / 60)) - (days * 1440 + hours * 60);
    
    Redraw();
  }
  else
    fprintf(stderr, "bbuptime error!: Unable to open /proc/uptime for reading!\n");
  
}
  
void ToolWindow::Redraw()
{
    // ADDED BY KEITH
    char szUptimeString[80] = {0};  

    memset(szUptimeString, 0, sizeof(szUptimeString)); //just in case...
    sprintf(szUptimeString, " %02d days %02d:%02d ", days, hours, minutes);
  
    XClearWindow(dpy, labelwin);
    XDrawString(dpy, labelwin, frameGC, resource->frame.bevelWidth,
                (label.height+resource->label.font->ascent-
                 resource->label.font->descent) / 2,
                 szUptimeString, strlen(szUptimeString));
}

void ToolWindow::EventLoop(void) 
{
  int xfd = ConnectionNumber(dpy);
  time_t lastTime = time(NULL);
  bool shutdown=False;

  lastTime = ((lastTime / 60) * 60);

  while (! shutdown) 
  {
    if (XPending(dpy)) 
    {
      XEvent Event;
      XNextEvent(dpy, &Event);
      /* process events */
      switch (Event.type)
      {
        case Expose:
        {
          CheckDate(True);
        }
        break;
        case ButtonPress:
        {
          if (Event.xbutton.button == LEFT_BUTTON)
          {
            if (!(raised))
            {
              XRaiseWindow(dpy,framewin);
              raised=True;            
            }
            
            //ADDED BY KEITH
            system(resource->label.launch1);  //<-- button one launch app
          }
          
          //ADDED BY KEITH
          else if(Event.xbutton.button == RIGHT_BUTTON)
          {
            system(resource->label.launch2); //<-- button two launch app
          }
          
          else if (Event.xbutton.button == MIDDLE_BUTTON)
          {
            /*REMOVED BY KEITH (re: annoying as fuck)
            if (raised) 
            {
              XLowerWindow(dpy,framewin);
              raised=False;
            }*/

            //ADDED BY KEITH
            system(resource->label.launch3); //<-- button three launch app
          }
        }
        break;
         case ConfigureNotify:
         {
            if (Event.xconfigure.window==framewin && Event.xconfigure.send_event)
            {
            Reconfigure();
              int parent_x,parent_y;
              Window parent_root;
              unsigned int parent_width;
            unsigned int parent_height;
            unsigned int parent_border_width;
            unsigned int parent_depth;
              XGetGeometry(dpy,Event.xconfigure.above,&parent_root,
                            &parent_x,&parent_y,&parent_width,&parent_height,
                            &parent_border_width,&parent_depth);
              frame.x=Event.xconfigure.x+parent_x;
              frame.y=Event.xconfigure.y+parent_y;
            }
         }
      }
      
      if (time(NULL) - lastTime > 59) 
      {
        CheckDate(True);
        lastTime = time(NULL);
      } 
    } 
    else
    {
      if (time(NULL) - lastTime < 60) 
      {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(xfd, &rfds);
  
        struct timeval tv;
        tv.tv_sec = 60 - (time(NULL) - lastTime);
        tv.tv_usec = 0;
  
        select(xfd + 1, &rfds, 0, 0, &tv);
      } 
      else 
      {
        CheckDate(True);
        lastTime = time(NULL);
      }
    }
  }
}

int main(int argc,char **argv)
{
  ToolWindow *AppWindow = new ToolWindow (argc,argv) ;

  AppWindow->EventLoop();
  delete AppWindow;
}
