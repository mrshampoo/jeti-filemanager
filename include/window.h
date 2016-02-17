/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= windows.h =-------*/
#include "projecttypes.h"

#ifndef _filewindow_
	#define _filewindow_

	Windowtype *newwindow( int h, int w, int y, int x );
	Windowtype *redefinewindow( Windowtype *win, int h, int w, int y, int x );
	int loadnewdir( Windowtype *win, char wd[] );
	void printwindow( Windowtype *win );
	void clearwindow( Windowtype *win );
	int toglehidewin( Windowtype *win );
	int insidewindow( Windowtype *win, int y, int x );
	int onshortcutrow( Windowtype *win, int y, int x );
	shortcutType *getshortcut( shortcutType *shorty, int pos );
	void destroywindow( Windowtype *win );

#endif
