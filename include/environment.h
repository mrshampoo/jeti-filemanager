/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= environment.h =--------*/
#include "projecttypes.h"

#ifndef _environment_
	#define _environment_

	int set_configname( char path[] );
	int set_terminalname( char terminal[] );
	int set_dialogwinreactions( int dw_reactions );
	int set_logpriority( int prio );
	int set_logpath( char logpath[] );
	int set_win_workdir_type( Windowtype *win, int type, int orientation );
	int set_win_shortcut_type( Windowtype *win, int type, int orientation );
	int set_win_filelist_type( Windowtype *win, int type, int orientation );
	int set_win_color( Windowtype *win, int type, int color );
	int set_tab_tolist( tabtype **listptr, printoption opt, int centration, int length );
	int set_win_workdir( Windowtype *win, char workdir[] );
	int set_win_showhiddenfiles( Windowtype *win, int boolean );
	int set_filetypeAction_tolist( filetypeAction **listptr, char filetype[], char command[] );
	int set_shortcut_tolist( shortcutType **listptr, char label[], char dir_command[] );
	int set_soundtype_tolist( soundeffectType **listptr, char sound[], int action );
	int set_animationis( int onoff );

	int put_default_terminalname();
	int put_default_dialogwinreactions();
	int put_default_logpriority();
	int put_default_logpath();
	int put_default_workdir_type( Windowtype *win );
	int put_default_shortcut_type( Windowtype *win );
	int put_default_filelist_type( Windowtype *win );
	int put_default_win_colors( Windowtype *win );
	int put_default_list_tabs( tabtype **listptr );
	int put_default_win_workdir( Windowtype *win );
	int put_default_win_showhidden( Windowtype *win );
	int put_default_list_filetypeAction( filetypeAction **listptr );
	int put_default_list_shortcuts( shortcutType **listptr );
	int put_default_list_sounds( soundeffectType **listptr );
	int put_default_animations();
	int put_start_totablist( tabtype *list, int win_widh );

	tabtype *find_tab( tabtype *tab, int n );

	int pget_configname();
	int fget_terminalname( char buff[] );
	int fget_dialogwinreactions( char buff[] );
	int fget_logpath(char buff[] );
	int fget_win_workdir_type( char buff[], Windowtype *win );
	int fget_win_shortcut_type( char buff[], Windowtype *win );
	int fget_win_filelist_type( char buff[], Windowtype *win );
	int fget_win_colors( char buff[], Windowtype *win );
	int fget_tabs( char buff[], tabtype **listptr, int widh );
	int fget_win_workdir( char buff[], Windowtype *win );
	int fget_win_showhidden( char buff[], Windowtype *win );
	int fget_fileAction( char buff[], filetypeAction **listptr );
	int fget_shortcuts( char buff[], shortcutType **listptr );
	int fget_soundeffects( char buff[], soundeffectType **listptr );
	int fget_animations( char buff[] );

	void clearEnvironment_tabs( tabtype **tabs );
	void clearEnvironment_actions( filetypeAction **actions );
	void clearEnvironment_shortcuts( shortcutType **shortCuts );
	void clearEnvironmett_sounds( soundeffectType **sounds );

#endif
