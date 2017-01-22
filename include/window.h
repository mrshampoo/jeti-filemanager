/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= windows.h =-------*/
#include "projecttypes.h"

#ifndef _filewindow_
	#define _filewindow_

	void init_colors( Windowtype *win );
	Windowtype *newwindow( int h, int w, int y, int x );
	Windowtype *redefinewindow( Windowtype *win, int h, int w, int y, int x );
	int loadnewdir( Windowtype *win, char wd[] );
	int print_win_workdirectory( Windowtype *win, int orientation, int highlight, int procent_spaceing );
	int print_win_shortcut( Windowtype *win, int num, int orientation, int highlight, int procent_spaceing );
	int print_win_filetabs_line( Windowtype *win, int num, int orientation, int highlight, int procent_spaceing );
	void printwindow( Windowtype *win, int movement, int activ );
	void clearwindow( Windowtype *win );
	int toglehidewin( Windowtype *win );
	int insidewindow( Windowtype *win, int y, int x );
	int onshortcut( Windowtype *win, int y, int x );
	shortcutType *getshortcut( shortcutType *shorty, int pos );
	void destroywindow( Windowtype *win );

#endif
