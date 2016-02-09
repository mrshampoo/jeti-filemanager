/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= command-window.h =------------------*/
#include "projecttypes.h"

#ifndef _command_window_
	#define _command_window_

	cmdWindowtype *new_cmdwindow( int h, int w, int y, int x, char wd[] );
	void printcmdwin( cmdWindowtype *cw, Windowtype *awin, Windowtype *pwin );
	void clearcmd( cmdWindowtype *cw );
	void putnewdir( cmdWindowtype *cw, char wd[], char ddir[] );
	int togglehide( cmdWindowtype *cw );
	int trowcmd( cmdWindowtype *cw, Windowtype *awin, Windowtype *pwin );
	void destroycmdwin( cmdWindowtype *cw );

#endif
