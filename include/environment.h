/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= environment.h =--------*/
#include "projecttypes.h"

#ifndef _environment_
	#define _environment_

	int get_configname();
	int set_configname( char path[] );
	int set_terminalname( char terminal[] );
	int get_terminalname();
	Windowtype *init_win_colors( Windowtype *win );
	tabtype *gettab( tabtype *tab, int n );
	tabtype *sorttabs( tabtype *tab, int wide );
	tabtype *init_wintabs_environment( tabtype *tab, int wide );
	Windowtype *init_workdir( Windowtype *win );
	filetypeAction *init_fileAction();
	shortcutType *init_shortcuts();
	soundeffectType *init_soundeffects();
	void clearEnvironment_tabs( tabtype *tabs );
	void clearEnvironment_actions( filetypeAction *actions );
	void clearEnvironment_shortcuts( shortcutType *shortCuts );
	void clearEnvironmett_sounds( soundeffectType *sounds );

#endif
