/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= navigation.h =-------*/
#include "projecttypes.h"

#ifndef _NAVIGATION_
	#define _NAVIGATION_

	void init_navigation();
	void redefine_navigation();
	void destroy_navigation(); 
	int scrollupp( Windowtype *win, soundeffectType *sounds );
	int scrolldown( Windowtype *win, soundeffectType *sounds );
	int stepupp( Windowtype *win, soundeffectType *sounds );
	int stepdown( Windowtype *win, soundeffectType *sounds );
	int selectfile( Windowtype *win, soundeffectType *sounds );
	int enter( Windowtype *win, filetypeAction *fileaction, soundeffectType *sounds );
	int handleshortcut( Windowtype *awin ,Windowtype *pwin, soundeffectType *sounds );
	int copyfiles( Windowtype *awin, Windowtype *pwin, soundeffectType *sounds );
	int movefiles( Windowtype *awin, Windowtype *pwin, soundeffectType *sounds );
	int removefiles( Windowtype *awin, soundeffectType *sounds );

#endif
