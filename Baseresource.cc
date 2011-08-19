//  Baseresource.cc for bbuptime - a tool to display uptime in X11.
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
#include "Baseresource.hh"
#include "blackboxstyle.hh"

BaseResource::BaseResource(ToolWindow *toolwindow)
{

	bbtool=toolwindow;

  if (bbtool->default_config)
  {
			ReadDefaultResource();
			ResourceType=DEFAULT;
  }
  else
  {
  	if (bbtool->config_filename == NULL)
	  {
		  char *homedir = getenv("HOME");
  		bbtool->config_filename = new char[strlen(homedir) + 32];
	  	sprintf(bbtool->config_filename, "%s/.bbtoolsrc", homedir);
  	}
	  if ((resource_db = XrmGetFileDatabase(bbtool->config_filename))!=NULL)
  	{
	  	/* .bbtoolsrc */
		  ReadBBtoolResource();
  		ResourceType=BBTOOLS;
	  }	
  	else
	  {
  	/* !bbtools */
	  	delete [] bbtool->config_filename;
	    char *homedir = getenv("HOME");
  		bbtool->config_filename = new char[strlen(homedir) + 32];
	  	sprintf(bbtool->config_filename, "%s/.blackboxrc", homedir);
	
		  if ((resource_db = XrmGetFileDatabase(bbtool->config_filename))==NULL)
  		{
	  		/* no blackbox, no bbtools */
		  	ReadDefaultResource();
			  ResourceType=DEFAULT;
  		}
	  	else
		  {
			  /* blackbox */
  			ReadBlackboxResource();
	  		ResourceType=BLACKBOX;			
		  }
    }
	}
	if (bbtool->image_dither && bbtool->v->c_class == TrueColor && bbtool->depth >= 24)
    	bbtool->image_dither = False;

	/* Need to do this here */
	bbtool->setupImageControl();
}

void BaseResource::Load()
{
	struct stat file_status;

	LoadBBToolResource();

	if ((bbtool->config_filename!=NULL)&(style.auto_config))
	{
		if (stat(bbtool->config_filename,&file_status)!=0)
		{
			fprintf(stderr,"Can't use autoconfig");
			style.auto_config=false;
			style.mtime=0;
		}
		else
		  style.mtime=file_status.st_mtime;
	}	
	
	XrmDestroyDatabase(resource_db);
}

BaseResource::~BaseResource()
{
	delete [] style.style_filename;
	delete [] style.conf_filename;
}

void BaseResource::CopyColor(BColor Color1,BColor *Color2)
{
	Color2->pixel=Color1.pixel;
	Color2->red=Color1.red;
	Color2->green=Color1.green;
	Color2->blue=Color1.blue;
  Color2->allocated=False;
}

void BaseResource::CopyTexture(BTexture Texture1,BTexture *Texture2)
{
  CopyColor(Texture1.color,&Texture2->color);
  CopyColor(Texture1.colorTo,&Texture2->colorTo);
  CopyColor(Texture1.hiColor,&Texture2->hiColor);
  CopyColor(Texture1.loColor,&Texture2->loColor);
  Texture2->texture=Texture1.texture;
}

void BaseResource::Reload()
{
	struct stat file_status;

	if ((resource_db = XrmGetFileDatabase(bbtool->config_filename))!=NULL)
	{	
		switch (ResourceType)
		{
	 		case BBTOOLS:
		 	{
				ReadBBtoolResource();
			}
		 	break;
	 		case DEFAULT:
		 	{
				ReadDefaultResource();
			}
		 	break;
	 		case BLACKBOX:
		 	{
				ReadBlackboxResource();	
			}
		 	break;	 	
		}
		LoadBBToolResource();
	
		if ((bbtool->config_filename!=NULL)&(style.auto_config))
		{
			if (stat(bbtool->config_filename,&file_status)!=0)
			{
				fprintf(stderr,"Can't use autoconfig");
				style.auto_config=false;
				style.mtime=0;
			}
			else
			  style.mtime=file_status.st_mtime;
		}
		
		XrmDestroyDatabase(resource_db);
	}
	else
	{
		fprintf(stderr,"Can't read resource filename: %s",bbtool->config_filename);
		exit(2);
	}	
}

bool BaseResource::ReadResourceFromFilename(char *rname, char *rclass)
{
	struct stat file_status;
	char *filename=NULL;
	XrmValue value;
	char *value_type;
		
	if (XrmGetResource(resource_db,rname,rclass, &value_type, &value))
	{
  		int len = strlen(value.addr);
		delete [] filename;
		filename = new char[len + 1];
	  	memset(filename, 0, len + 1);
	    strncpy(filename, value.addr, len);
		if (stat(filename,&file_status)!=0)
		{
			db=NULL;
			delete [] filename;			
			return(False);
		}
		db = XrmGetFileDatabase(filename);
		delete [] filename;
		return(True);
  	}
	db=NULL;
	delete [] filename;	
	return(False);
}

void BaseResource::ReadBBtoolResource()
{
	XrmValue value;
	char *value_type;	

	if (!ReadResourceFromFilename("bbdate.styleFile","Bbdate.StyleFile"))
	{
		/* use default stylefile */
		delete [] style.conf_filename;
		int len=strlen(DEFAULT_CONF);
		style.conf_filename = new char[len + 1];
  	memset(style.conf_filename, 0, len + 1);
	  strncpy(style.conf_filename,DEFAULT_CONF, len);			
		if ((resource_db = XrmGetFileDatabase(style.conf_filename))==NULL)
		{
			fprintf(stderr,"Could not open default config file: %s\n",
					style.conf_filename);
			fprintf(stderr,"Using internal defaults.\n");
			return;
		}
		else
			XrmCombineDatabase(db,&resource_db,False);
	}
	else
		XrmCombineDatabase(db,&resource_db,False);
		
	if (XrmGetResource(resource_db, "bbtools.colorsPerChannel",
		     "Bbtools.ColorsPerChannel", &value_type, &value))
	{
  		if (sscanf(value.addr, "%d", &bbtool->colors_per_channel) != 1)
		{
    	  bbtool->colors_per_channel = 4;
	    }
		else
		{
			if (bbtool->colors_per_channel < 2) bbtool->colors_per_channel = 2;
			if (bbtool->colors_per_channel > 6) bbtool->colors_per_channel = 6;
	    }
	}	
	else
    	bbtool->colors_per_channel = 4;
    	
    	
	if (XrmGetResource(resource_db, "bbtools.imageDither",
		     "Bbtools.ImageDither", &value_type, &value))
	{
		if (! strncasecmp("true", value.addr, value.size))
			bbtool->image_dither = True;
	    else
			bbtool->image_dither = False;
	}	
	else
		bbtool->image_dither = True;
    	
  	
}

// CHANGED BY KEITH - this whole goddam function needed overhauling for to read local nobb file
void BaseResource::ReadDefaultResource()
{
	XrmValue value;
	char *value_type;

  if(bbtool->bbconfig_filename != NULL)
  {
    if ((db = XrmGetFileDatabase(bbtool->bbconfig_filename)) == NULL)			
  	{
  		fprintf(stderr,"Could not open config file: %s\n", bbtool->bbconfig_filename);
    	delete [] style.conf_filename;
		  int len=strlen(DEFAULT_NOBB);
  		style.conf_filename = new char[len + 1];
	  	memset(style.conf_filename, 0, len + 1);
      strncpy(style.conf_filename,DEFAULT_NOBB, len);							
		  if ((db = XrmGetFileDatabase(style.conf_filename))==NULL)			
  		{
	  		fprintf(stderr,"Could not open default config file: %s\n", style.conf_filename);
        return;
  		}
  		else
		  	XrmCombineDatabase(db,&resource_db,True);
		}
		else
			XrmCombineDatabase(db,&resource_db,True);
  }
  else
  {
    if(style.conf_filename)
    delete [] style.conf_filename;
    char * homedir = getenv("HOME");
    int    len     = strlen(homedir) + strlen(LOCAL_NOBB);
    style.conf_filename = new char[len + 2];
    memset(style.conf_filename, 0, len + 2);
    sprintf(style.conf_filename, "%s/%s", homedir, LOCAL_NOBB);
    if((db = XrmGetFileDatabase(style.conf_filename)) == NULL)
    {
      delete [] style.conf_filename;
      int len = strlen(DEFAULT_NOBB);
      style.conf_filename = new char[len + 1];
      memset(style.conf_filename, 0, len + 1);
     	strncpy(style.conf_filename, DEFAULT_NOBB, len);
      if((db = XrmGetFileDatabase(style.conf_filename)) == NULL)
      {
        fprintf(stderr, "Could not open default config file: %s\n", style.conf_filename);  
        fprintf(stderr, "Using internal defaults.\n");
        return;
      }
      else
        XrmCombineDatabase(db, &resource_db, True);

    }
    else
      XrmCombineDatabase(db, &resource_db, True);
  }
	
  /* REMOVED BY KEITH (re: that style file idea blows goats for free)
  if (!ReadResourceFromFilename("bbdate.styleFile","Bbdate.StyleFile"))
  {
	  fprintf(stderr,"Could not open default style file bbdate.styleFile: in %s\n",
	  style.conf_filename);
		fprintf(stderr,"Using internal defaults.\n");
	}
	else
		XrmCombineDatabase(db,&resource_db,False);				*/

	
	if (XrmGetResource(resource_db, "bbtools.colorsPerChannel", "Bbtools.ColorsPerChannel", &value_type, &value))
	{
 	  if (sscanf(value.addr, "%d", &bbtool->colors_per_channel) != 1)
		{
   	  bbtool->colors_per_channel = 4;
	  }
		else
		{
			if (bbtool->colors_per_channel < 2) bbtool->colors_per_channel = 2;
			if (bbtool->colors_per_channel > 6) bbtool->colors_per_channel = 6;
	  }
	}	
	else
    bbtool->colors_per_channel = 4;
   	
	if (XrmGetResource(resource_db, "bbtools.imageDither", "Bbtools.ImageDither", &value_type, &value))
	{
		if (! strncasecmp("true", value.addr, value.size))
			bbtool->image_dither = True;
	  else
			bbtool->image_dither = False;
	}	
	else
		bbtool->image_dither = True;
}

void BaseResource::ReadBlackboxResource()
{
	XrmValue value;
	char *value_type;
	
  if (!ReadResourceFromFilename("session.styleFile","Session.StyleFile"))
	{
		fprintf(stderr,"Could not open blackbox style file\n");
	}
	else
		XrmCombineDatabase(db,&resource_db,False); 

  // CHANGED BY KEITH
  if (bbtool->bbconfig_filename != NULL)
  {
    if ((db = XrmGetFileDatabase(bbtool->bbconfig_filename)) == NULL)			
  	{
  		fprintf(stderr,"Could not open config file: %s\n", bbtool->bbconfig_filename);
      return;
		}
		else
			XrmCombineDatabase(db,&resource_db,True);
  }
  
  //COMPLETELY WHACKED BY KEITH (re: to make it able to load local files)
  else
  {
    if(style.conf_filename)
      delete [] style.conf_filename;
    char * homedir = getenv("HOME");
    int    len     = strlen(homedir) + strlen(DEFAULT_LOCAL);
    style.conf_filename = new char[len + 2];
    memset(style.conf_filename, 0, len + 2);
    sprintf(style.conf_filename, "%s/%s", homedir, DEFAULT_LOCAL);
    if((db = XrmGetFileDatabase(style.conf_filename)) == NULL)
    {
      delete [] style.conf_filename;
      int len = strlen(DEFAULT_CONF);
      style.conf_filename = new char[len + 1];
      memset(style.conf_filename, 0, len + 1);
     	strncpy(style.conf_filename, DEFAULT_CONF, len);
      if((db = XrmGetFileDatabase(style.conf_filename)) == NULL)
      {
        fprintf(stderr, "Could not open default config file: %s\n", style.conf_filename);  
        fprintf(stderr, "Using internal defaults.\n");
        return;
      }
      else
        XrmCombineDatabase(db, &resource_db, True);

    }
    else
      XrmCombineDatabase(db, &resource_db, True);
  }
  
  /*  ::CHANGED TO ABOVE::
  else if (!ReadResourceFromFilename("bbdate.configFile","Bbdate.ConfigFile"))
	{
		delete [] style.conf_filename;
		int len=strlen(DEFAULT_CONF);
		style.conf_filename = new char[len + 1];
	  memset(style.conf_filename, 0, len + 1);
    strncpy(style.conf_filename,DEFAULT_CONF, len);							
		if ((db = XrmGetFileDatabase(style.conf_filename))==NULL)			
		{
			fprintf(stderr,"Could not open default config file: %s\n",
					style.conf_filename);
		}
		else
			XrmCombineDatabase(db,&resource_db,True);
	}
	else XrmCombineDatabase(db,&resource_db,True); */
	
	if (XrmGetResource(resource_db, "session.colorsPerChannel",
		     "Session.ColorsPerChannel", &value_type, &value))
	{
  		if (sscanf(value.addr, "%d", &bbtool->colors_per_channel) != 1)
		{
    	  bbtool->colors_per_channel = 4;
	  }
		else
		{
			if (bbtool->colors_per_channel < 2) bbtool->colors_per_channel = 2;
			if (bbtool->colors_per_channel > 6) bbtool->colors_per_channel = 6;
	   }
	}
  else
 	  bbtool->colors_per_channel = 4;

	if (XrmGetResource(resource_db, "session.imageDither",
		     "session.ImageDither", &value_type, &value))
	{
		if (! strncasecmp("true", value.addr, value.size))
			bbtool->image_dither = True;
	    else
			bbtool->image_dither = False;
	}
}


void BaseResource::readDatabaseTexture(char *rname, char *rclass,
							                  	 char *bbrname,char *bbrclass,
                  								 BColor defaultcolor,BColor defaultcolorto,
									                 unsigned long defaulttexture, 
                                   BTexture *texture)
{
  XrmValue value;
  char *value_type;

  texture->texture = 0;

  if (!XrmGetResource(resource_db, rname, rclass, &value_type,&value))
	  if (!XrmGetResource(resource_db,bbrname,bbrclass,&value_type,&value))
  		texture->texture=defaulttexture;

  if (texture->texture==0)
  {
    if (strstr(value.addr, "Solid")) 
    {
      texture->texture |= BImage_Solid;
    } 
    else if (strstr(value.addr, "Gradient")) 
    {
      texture->texture |= BImage_Gradient;

      if (strstr(value.addr, "Diagonal")) 
      {
      	texture->texture |= BImage_Diagonal;
      } 
      else if (strstr(value.addr, "Horizontal")) 
      {
      	texture->texture |= BImage_Horizontal;
      } 
      else if (strstr(value.addr, "Vertical")) 
      {
      	texture->texture |= BImage_Vertical;
      } else
      	texture->texture |= BImage_Diagonal;
    } 
    else
      texture->texture |= BImage_Solid;

    if (strstr(value.addr, "Raised"))
      texture->texture |= BImage_Raised;
    else if (strstr(value.addr, "Sunken"))
      texture->texture |= BImage_Sunken;
    else if (strstr(value.addr, "Flat"))
      texture->texture |= BImage_Flat;
    else
      texture->texture |= BImage_Raised;

    if (! (texture->texture & BImage_Flat))
      if (strstr(value.addr, "Bevel"))
         if (strstr(value.addr, "Bevel1"))
            texture->texture |= BImage_Bevel1;
	else if (strstr(value.addr, "Bevel2"))
	  texture->texture |= BImage_Bevel2;
	else
	  texture->texture |= BImage_Bevel1;
  }

  if (texture->texture & BImage_Solid) {
    int clen = strlen(rclass) + 8, nlen = strlen(rname) + 8;
    char *colorclass = new char[clen], *colorname = new char[nlen];

    sprintf(colorclass, "%s.Color", rclass);
    sprintf(colorname,  "%s.color", rname);
  
    char *bbcolorclass=0;
    char *bbcolorname=0;
    if (bbrclass&&bbrname)
    {
     	clen = strlen(bbrclass) + 8, nlen = strlen(bbrname) + 8;
     	bbcolorclass = new char[clen];
      bbcolorname = new char[nlen];

      sprintf(bbcolorclass, "%s.Color", bbrclass);
    	sprintf(bbcolorname,  "%s.color", bbrname);
    }

    if (!readDatabaseColor(colorname, colorclass,bbcolorname,bbcolorclass,
                          &(texture->color)))  
      CopyColor(defaultcolor,&(texture->color));

    delete [] colorclass;
    delete [] colorname;
    delete [] bbcolorclass;
    delete [] bbcolorname;

    if ((! texture->color.allocated) ||
	(texture->texture & BImage_Flat))
      return;

    XColor xcol;

    xcol.red = (unsigned int) (texture->color.red +
			       (texture->color.red >> 1));
    if (xcol.red >= 0xff) xcol.red = 0xffff;
    else xcol.red *= 0xff;
    xcol.green = (unsigned int) (texture->color.green +
				 (texture->color.green >> 1));
    if (xcol.green >= 0xff) xcol.green = 0xffff;
    else xcol.green *= 0xff;
    xcol.blue = (unsigned int) (texture->color.blue +
				(texture->color.blue >> 1));
    if (xcol.blue >= 0xff) xcol.blue = 0xffff;
    else xcol.blue *= 0xff;

    if (! XAllocColor(bbtool->dpy, bbtool->image_control->getColormap(), &xcol))
      xcol.pixel = 0;

    texture->hiColor.pixel = xcol.pixel;

    xcol.red = (unsigned int) ((texture->color.red >> 2) +
			       (texture->color.red >> 1)) * 0xff;
    xcol.green = (unsigned int) ((texture->color.green >> 2) +
				 (texture->color.green >> 1)) * 0xff;
    xcol.blue = (unsigned int) ((texture->color.blue >> 2) +
				(texture->color.blue >> 1)) * 0xff;

    if (! XAllocColor(bbtool->dpy, bbtool->image_control->getColormap(), &xcol))
      xcol.pixel = 0;

    texture->loColor.pixel = xcol.pixel;
  } 
  else if (texture->texture & BImage_Gradient) 
  {
    int clen = strlen(rclass) + 10, nlen = strlen(rname) + 10;
    char *colorclass = new char[clen], *colorname = new char[nlen],
    *colortoclass = new char[clen], *colortoname = new char[nlen];
	  sprintf(colorclass, "%s.Color", rclass);
    sprintf(colorname,  "%s.color", rname);

    sprintf(colortoclass, "%s.ColorTo", rclass);
    sprintf(colortoname,  "%s.colorTo", rname);

    char *bbcolorclass=0;
    char *bbcolorname=0;
    char *bbcolortoclass=0;
    char *bbcolortoname=0;
    if (bbrclass&&bbrname)
    {
  	  clen = strlen(bbrclass) + 10;
      nlen = strlen(bbrname) + 10;
      bbcolorclass = new char[clen];
      bbcolorname = new char[nlen];
      bbcolortoclass = new char[clen];
      bbcolortoname = new char[nlen];
    
      sprintf(bbcolorclass, "%s.Color", bbrclass);
      sprintf(bbcolorname,  "%s.color", bbrname);
      sprintf(bbcolortoclass, "%s.ColorTo", bbrclass);
      sprintf(bbcolortoname,  "%s.colorTo", bbrname);
    }

   	if (!readDatabaseColor(colorname, colorclass,bbcolorname,bbcolorclass, 
                      &(texture->color)))
      CopyColor(defaultcolor,&texture->color);

    if (!readDatabaseColor(colortoname, colortoclass,bbcolortoname,
                           bbcolortoclass,&(texture->colorTo)))
       CopyColor(defaultcolorto,&texture->colorTo);
     

    delete [] colorclass;
    delete [] colorname;
    delete [] bbcolorclass;
    delete [] bbcolorname;
    delete [] colortoclass;
    delete [] colortoname;
    delete [] bbcolortoclass;
    delete [] bbcolortoname;
  }
}






void BaseResource::readDatabaseColor(char *rname, char *rclass, char *bbrname,
								char *bbrclass,char * default_color, BColor *color)
{
	XrmValue value;
	char *value_type;

	if (color->allocated)
	{
    	XFreeColors(bbtool->dpy, bbtool->image_control->getColormap(),
					&(color->pixel), 1, 0);
	    color->allocated = False;
	}

	if (XrmGetResource(resource_db, rname, rclass, &value_type,&value))
	{
    	color->pixel = bbtool->image_control->getColor(value.addr, &color->red,
											   &color->green, &color->blue);
	    color->allocated = 1;
	} 
  else if (bbrname && bbrclass)
	{
		if (XrmGetResource(resource_db, bbrname, bbrclass, &value_type,
		     &value))
		{
		    color->pixel = bbtool->image_control->getColor(value.addr, &color->red,
												   &color->green, &color->blue);
			color->allocated = 1;
		}
	 	else if (default_color)
    {
		    color->pixel = bbtool->image_control->getColor(default_color, 
                                     &color->red,
												             &color->green, &color->blue);
       	color->allocated = 1;

     }
	}
	else if (default_color)
  {
		color->pixel = bbtool->image_control->getColor(default_color, &color->red,
											   &color->green, &color->blue);
   	color->allocated = 1;

  }
}

bool BaseResource::readDatabaseColor(char *rname, char *rclass, char *bbrname,
								char *bbrclass, BColor *color)
{
	XrmValue value;
	char *value_type;

	if (color->allocated)
	{
    	XFreeColors(bbtool->dpy, bbtool->image_control->getColormap(),
					&(color->pixel), 1, 0);
	    color->allocated = False;
	}

	if (XrmGetResource(resource_db, rname, rclass, &value_type,&value))
	{
    	color->pixel = bbtool->image_control->getColor(value.addr, &color->red,
											   &color->green, &color->blue);
	    color->allocated = 1;
      return true;
	} 
  if (bbrname && bbrclass)
	{
		if (XrmGetResource(resource_db, bbrname, bbrclass, &value_type,
		     &value))
		{
		    color->pixel = bbtool->image_control->getColor(value.addr, &color->red,
												   &color->green, &color->blue);
			color->allocated = 1;
      return true;
		}
    return false;
  }
  return false;
}

void BaseResource::readDatabaseTexture(char *rname, char *rclass,
							                  	 char *bbrname,char *bbrclass,
                  								 char *defaultcolor, char *defaultcolorto,
									                 unsigned long defaulttexture, 
                                   BTexture *texture)
{
  XrmValue value;
  char *value_type;

  texture->texture = 0;

  if (!XrmGetResource(resource_db, rname, rclass, &value_type,&value))
	  if (!XrmGetResource(resource_db,bbrname,bbrclass,&value_type,&value))
  		texture->texture=defaulttexture;

  if (texture->texture==0)
  {
    if (strstr(value.addr, "Solid")) 
    {
      texture->texture |= BImage_Solid;
    } 
    else if (strstr(value.addr, "Gradient")) 
    {
      texture->texture |= BImage_Gradient;

      if (strstr(value.addr, "Diagonal")) 
      {
      	texture->texture |= BImage_Diagonal;
      } 
      else if (strstr(value.addr, "Horizontal")) 
      {
      	texture->texture |= BImage_Horizontal;
      } 
      else if (strstr(value.addr, "Vertical")) 
      {
      	texture->texture |= BImage_Vertical;
      } else
      	texture->texture |= BImage_Diagonal;
    } 
    else
      texture->texture |= BImage_Solid;

    if (strstr(value.addr, "Raised"))
      texture->texture |= BImage_Raised;
    else if (strstr(value.addr, "Sunken"))
      texture->texture |= BImage_Sunken;
    else if (strstr(value.addr, "Flat"))
      texture->texture |= BImage_Flat;
    else
      texture->texture |= BImage_Raised;

    if (! (texture->texture & BImage_Flat))
      if (strstr(value.addr, "Bevel"))
         if (strstr(value.addr, "Bevel1"))
            texture->texture |= BImage_Bevel1;
	else if (strstr(value.addr, "Bevel2"))
	  texture->texture |= BImage_Bevel2;
	else
	  texture->texture |= BImage_Bevel1;
  }

  if (texture->texture & BImage_Solid) {
    int clen = strlen(rclass) + 8, nlen = strlen(rname) + 8;
    char *colorclass = new char[clen], *colorname = new char[nlen];

    sprintf(colorclass, "%s.Color", rclass);
    sprintf(colorname,  "%s.color", rname);
  
    char *bbcolorclass=0;
    char *bbcolorname=0;
    if (bbrclass&&bbrname)
    {
     	clen = strlen(bbrclass) + 8, nlen = strlen(bbrname) + 8;
     	bbcolorclass = new char[clen];
      bbcolorname = new char[nlen];

      sprintf(bbcolorclass, "%s.Color", bbrclass);
    	sprintf(bbcolorname,  "%s.color", bbrname);
    }

    readDatabaseColor(colorname, colorclass,bbcolorname,bbcolorclass,
                      defaultcolor,&(texture->color));

    delete [] colorclass;
    delete [] colorname;
    delete [] bbcolorclass;
    delete [] bbcolorname;

    if ((! texture->color.allocated) ||
	(texture->texture & BImage_Flat))
      return;

    XColor xcol;

    xcol.red = (unsigned int) (texture->color.red +
			       (texture->color.red >> 1));
    if (xcol.red >= 0xff) xcol.red = 0xffff;
    else xcol.red *= 0xff;
    xcol.green = (unsigned int) (texture->color.green +
				 (texture->color.green >> 1));
    if (xcol.green >= 0xff) xcol.green = 0xffff;
    else xcol.green *= 0xff;
    xcol.blue = (unsigned int) (texture->color.blue +
				(texture->color.blue >> 1));
    if (xcol.blue >= 0xff) xcol.blue = 0xffff;
    else xcol.blue *= 0xff;

    if (! XAllocColor(bbtool->dpy, bbtool->image_control->getColormap(), &xcol))
      xcol.pixel = 0;

    texture->hiColor.pixel = xcol.pixel;

    xcol.red = (unsigned int) ((texture->color.red >> 2) +
			       (texture->color.red >> 1)) * 0xff;
    xcol.green = (unsigned int) ((texture->color.green >> 2) +
				 (texture->color.green >> 1)) * 0xff;
    xcol.blue = (unsigned int) ((texture->color.blue >> 2) +
				(texture->color.blue >> 1)) * 0xff;

    if (! XAllocColor(bbtool->dpy, bbtool->image_control->getColormap(), &xcol))
      xcol.pixel = 0;

    texture->loColor.pixel = xcol.pixel;
  } 
  else if (texture->texture & BImage_Gradient) 
  {
    int clen = strlen(rclass) + 10, nlen = strlen(rname) + 10;
    char *colorclass = new char[clen], *colorname = new char[nlen],
    *colortoclass = new char[clen], *colortoname = new char[nlen];
	  sprintf(colorclass, "%s.Color", rclass);
    sprintf(colorname,  "%s.color", rname);

    sprintf(colortoclass, "%s.ColorTo", rclass);
    sprintf(colortoname,  "%s.colorTo", rname);

    char *bbcolorclass=0;
    char *bbcolorname=0;
    char *bbcolortoclass=0;
    char *bbcolortoname=0;
    if (bbrclass&&bbrname)
    {
  	  clen = strlen(bbrclass) + 10;
      nlen = strlen(bbrname) + 10;
      bbcolorclass = new char[clen];
      bbcolorname = new char[nlen];
      bbcolortoclass = new char[clen];
      bbcolortoname = new char[nlen];
    
      sprintf(bbcolorclass, "%s.Color", bbrclass);
      sprintf(bbcolorname,  "%s.color", bbrname);
      sprintf(bbcolortoclass, "%s.ColorTo", bbrclass);
      sprintf(bbcolortoname,  "%s.colorTo", bbrname);
    }

   	readDatabaseColor(colorname, colorclass,bbcolorname,bbcolorclass, 
                      defaultcolor,&(texture->color));

    readDatabaseColor(colortoname, colortoclass,bbcolortoname,bbcolortoclass, 
                      defaultcolorto,&(texture->colorTo));

    delete [] colorclass;
    delete [] colorname;
    delete [] bbcolorclass;
    delete [] bbcolorname;
    delete [] colortoclass;
    delete [] colortoname;
    delete [] bbcolortoclass;
    delete [] bbcolortoname;
  }
}
