/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/
#ifndef _dialogwindows_
	#define _dialogwindows_
	dialogwindowtype *new_dialogwindow( int h, int w, int y, int x );
	int print_dialogwin( dialogwindowtype *dialogwin, Windowtype *awin, char destiwd[], int permisson_requierd, char question[], char repasswd[] );
	void clear_dialogwin( dialogwindowtype *dialogwin );
	int insidedialogwin( WINDOW *win, int y, int x );
	int test_rw_permissiondenieds( Windowtype *win, char destiwd[] );
	void destroy_dialogwin( dialogwindowtype *dialogwin );
#endif
