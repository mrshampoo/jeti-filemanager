/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= directorys.h =-------*/
#include "projecttypes.h"

#ifndef _dir_IO_
	#define _dir_IO_

	dirEntry *getlast( dirEntry *filelist );
	dirEntry *getlast_and_add( dirEntry *filelist );
	void clearEntrys( dirEntry *filelist );
	dirEntry *getEntrys( DIR *dir, char wd[], dirEntry *filelist, int SHOWHIDDEN );
	dirEntry *bubbeladd( dirEntry *filelist, dirEntry *obj );
	dirEntry *gotoEntry( dirEntry *filelist, int filenr );

	dirEntry *getid3tags( dirEntry *obj, char path[] );

	int printtotalnr( dirEntry *filelist );
	int printCurrentnr( dirEntry *filelist );
	int printfiletypelenght( dirEntry *filelist, int filenr );

	char printCurrentfiletype( dirEntry *filelist, int c, int filenr );
	char printfilename( dirEntry *filelist, int c, int filenr );
	
	int isoffiletype( dirEntry *filelist, int filenr, char type[] );

#endif
