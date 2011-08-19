//  Baseresource.hh for bbuptime - a tool to display uptime in X11.
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


#ifndef __BASERESOURCE_HH
#define __BASERESOURCE_HH

class ToolWindow;
class BImageControl;

struct STYLE {
	bool auto_config;
	char *conf_filename;
	char *style_filename;	
	time_t  mtime;
};

class BaseResource {

        public:
                BaseResource(ToolWindow *);
                virtual ~BaseResource(void);

                void CopyColor(BColor ,BColor *);
		void CopyTexture(BTexture ,BTexture *);
		void Reload(void);
		STYLE style;

	protected:
		void Load(void);
		void readDatabaseTexture(char *, char *,char *,char *,
					 BColor ,BColor ,unsigned long, 
	                                 BTexture *);

		void readDatabaseTexture(char *, char *,char *,char *,
					 char *, char *,unsigned long, 
	                                 BTexture *texture);


		void readDatabaseColor(char *, char *, char *,char *,
				       char *, BColor *);

		bool readDatabaseColor(char *, char *, char *,char *, 
				       BColor *);

		ToolWindow *bbtool;
		XrmDatabase resource_db;
		XrmDatabase db;
		time_t mtime;
		bool auto_config;

		virtual void LoadBBToolResource(void) = 0; 
        private:
		bool ReadResourceFromFilename(char *, char *);
		void ReadBBtoolResource(void);
		void ReadDefaultResource(void);
		void ReadBlackboxResource(void);

		int ResourceType;
};

#endif /* __BASERESOURCE_HH */
