/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= environment.h =--------*/
#include "projecttypes.h"
#include "window.h"
#include "environment.h"
#include "systemlog.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

int set_configname( char path[] )
	{
		int success = 0;

		systemlog( 3, "set_configname" );

		if( !access( path, R_OK ) )
			{
				strcpy( CONFIG_NAME[ CONFIG_NR  ], path );
				CONFIG_NR++;
				success |= 1;
			}
		else
			{
				systemlog( 1, "WARNING: cant open config" );
				success = -1;
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else	
			{
				systemlog( 93, "\t[FAILD] (" );
				systemlog( 93, path );
				systemlog( 93, ")");
			}

		return success;
	}

int set_terminalname( char terminal[] )
	{
		int success = 0;

		char termpath[256];

		systemlog( 3, "set_terminalname" );

		strcpy( termpath, "/bin/" );
		strcat( termpath, terminal );
		
		if( !access( termpath, X_OK ) )
			{
				strcpy( TERMINALNAME, terminal );
				success |= 1;
			}
		else
			{
				systemlog( 1, terminal );
				systemlog( 1, "ERROR: there is no sush terminal. fallback set to xterm.\n" );
				strcpy( TERMINALNAME, "xterm" );
				success = -1;
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD] (" );
				systemlog( 93, terminal );
				systemlog( 93, ")" );
			}

		return success;
	}

int set_logpriority( int prio )
	{
		int success = 0;
		char error[ 10 ];

		systemlog( 3, "set_logpriority" );

		if( LOGPRIO == -1 )
			{
				LOGPRIO = prio;
				success |= 1;
			}
		else
			{
				systemlog( 4, "set_logpriority: LOGPRIO already set!" );
			}

		if( success > -1 )
			{
				systemlog( 93, "\t[DONE]" );
			}
		else
			{
				systemlog( 93, "\t[FAILD]" );
				sprintf( error, "(%i)", prio );
				systemlog( 93, error );
				
			}

		return success;
	}

int set_dialogwinreactions( int dw_reactions )
	{
		int success = 0;
		char error[ 10 ];

		systemlog( 3, "set_dialogwinreactions" );

		if( dw_reactions > 28 )
			{
				systemlog( 1, "ERROR: set_dialogwinreactions: to large ->nonvalid" );
				success = -1;
			}
		else if( dw_reactions < 0 )
			{
				systemlog( 1, "ERROR: set_dialogwinreactions: negativ value ->nonvalid" );
				success = -1;
			}
		else if( (dw_reactions & 4) && (dw_reactions & 8) )
			{
				systemlog( 1, "ERROR: set_dialogwinreactions: xx11xx are in conflict ->nonvalid" );
				success = -1;
			}
		else if( (dw_reactions & 16) && (dw_reactions & 32) )
			{
				systemlog( 1, "ERROR: set_dialogwinreactions: 11xxxx are in conflict ->nonvalid" );
				success = -1;
			}
		else
			{
				DW_REACTION = dw_reactions;
				success |= 1;
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD] " );
				sprintf( error, "(%d)", dw_reactions ); systemlog( 93, error );
			}

		return success;
	}

int set_logpath( char logpath[] )
	{
		int success = 0;

		systemlog( 3, "set_logpath" );
		if( FILEPATH_LOG[0] != '\0' )
			{
				systemlog( 4, "ATENTION: set_logpath: path alredy set, NULLify before resetting" );
				success = -1;
			}

		if( success >= 0 && !access( logpath, W_OK ) )
			{
				strcpy( FILEPATH_LOG, logpath );
				if( logpath[strlen(logpath)] != '/' )
					strcat( FILEPATH_LOG, "/" );
 
				success |= 1;
			}
		else if( success >= 0 )
			{
				systemlog( 1, "ERROR: set_logpath: cant wrie logfile in: " );
				systemlog( 91, logpath );

				success = -1;
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD] (" );
				systemlog( 93, logpath );
				systemlog( 93, ")" );
			}

		return success;
	}

int set_win_workdir_type( Windowtype *win, int type, int orientation )
	{
		int success = 0;
		char errors[ 30 ];

		systemlog( 3, "set_win_shortcut_type"  );

		if( orientation < 1 || orientation > 15 )
			{
				systemlog( 1, "ERROR: set_win_workdir_type: orientation is none valid" );
				success |= -1;
			}

		if( success != -1 )
			{
				win->type_workdir = type & 1;
				success |= 1;

				if( ( type & 2 ) == 0 )
					{//costom windows
						win->orientation_workdir = orientation;
						success |= 2;
					}
				else if( ( type & 2 ) == 2 )
					{//mirror windows
						if( win->x == 0 )
							{
								win->orientation_workdir = orientation;
								success |= 2;
							}
						else
							{
								if( ( orientation & 12 ) == 12   || orientation < 4 )
									{
										if( (orientation & 3) == 3 )
											{
												win->orientation_workdir = orientation;
											}
										else if( orientation & 2 )
											{
												win->orientation_workdir = 1 + ( orientation & 12 );
											}
										else if( orientation & 1 )
											{
												win->orientation_workdir = 2 + ( orientation & 12 );
											}
										else
											{
												win->orientation_workdir = orientation;
											}
									}
								else
									{
									}
							}
					}
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD]");
				sprintf( errors, "(%d)(%d)", type, orientation );
			}

		return success;
	}

int set_win_shortcut_type( Windowtype *win, int type, int orientation )
	{
		int success = 0;
		char error[ 30 ];

		systemlog( 3, "set_win_shortcut_type" );

		if( orientation < 1 || orientation > 15 )
			{
				systemlog( 1, "ERROR: set_win_shortcut_type: orientation is nonevalid" );
				success = -1;
			}

		if( success != -1 ) 
			{
				if( ( type & 1 ) == 0 )
					{ //costom windows
						win->orientation_shortcuts = orientation;
						success |= 1;
					}
				else if( ( type & 1 ) == 1 )
					{ //mirror windows
						if( win->x == 0 )
							{
								win->orientation_shortcuts = orientation;
							}
						else
							{
								if( ( orientation & 12 ) == 12 || orientation < 4 )
									{
										if( ( orientation & 3 ) == 3 )
											{
												win->orientation_shortcuts = orientation;
											}
										else if(  orientation & 2 )
											{
												win->orientation_shortcuts = 1 + ( orientation & 12 );
											}
										else if( orientation & 1 )
											{
												win ->orientation_shortcuts = 2 + ( orientation & 12 );
											}
										else
											{
												win ->orientation_shortcuts = orientation;
											}
									}
							}

						success |= 1;
					}
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD]");
				sprintf( error, "(%d)(%d)", type, orientation );
			}
		return success;
	}

int set_win_filelist_type( Windowtype *win, int type, int orientation )
	{
		int success = 0;
		char error[ 30 ];

		systemlog( 3, "set_win_filelist_type" );

		if( orientation < 1 || orientation > 4 )
			{
				systemlog( 1, "ERROR: set_win_filelist_type, orientation is none valid" );
				success |= -1;
			}
		if( type < 0 )
			{
				systemlog( 1, "ERROR: set_win_filelist_type: type is none valid" );
				success |= 2;
			}

		if( success != -1 ) 
			{
				if( ( type & 1 ) == 0 )
					{ //costome windows
						win->orientation_filelist = orientation;
						success |= 1;
					}
				else if( ( type & 1) == 1 )
					{//mirror windows
						if( win->x == 0 )
							{
								win->orientation_filelist = orientation;
							}
						else
							{
								if( (orientation & 3) == 3 || orientation == 0 )
									{
										win->orientation_filelist = orientation;
									}
								else if( orientation & 2 )
									{
										win->orientation_filelist = 1;
									}
								else
									{
										win->orientation_filelist = 2;
									}
							}

						success |= 1;
					}
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD]");
				sprintf( error, "(%d)(%d)", type, orientation );
			}
		return success;
	}

int set_wincolor( Windowtype *win, int type, int color )
	{
		int success = 0;
		char error[10];

		systemlog( 3, "set_wincolor" );

		if( type < NUMBEROFCOLORS )
			{
				win->color[type] = color;
				success |= 1;
			}
		else
			{
				systemlog( 1, "ERROR: set_wincolor: type is out of range");
				success = -1;
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD]" );
				sprintf( error, "(%d) (%d)", type, color );
				systemlog( 93, error );
			}

		return success;
	}

int set_tab_tolist( tabtype **listptr, printoption opt, int centration, int length )
	{
		int success = 0;

		tabtype *list = NULL;

		systemlog( 3, "set_tab_tolist");

		if( *listptr == NULL )
			{
				systemlog( 4, "new tablist created");
				*listptr = malloc( sizeof(tabtype) );
				list = *listptr;
				list->prev = NULL;
				list->next = NULL;
				list->n = 0;
			}
		else
			{
				list = *listptr;

				while( list->next != NULL )
					list = list->next;

				list->next = malloc( sizeof(tabtype) );
				list->next->prev = list;
				list->next->next = NULL;
				list = list->next;
				list->n = list->prev->n +1;
			}

		//set values
		switch( opt )
			{
			case nothing:
				list->opt = opt;
				if( length > 0 )
					list->length = length;
				else
					list->length = 1;
				success |= 1;
				break;

			case selected: 
				list->opt = opt;
				if( length > 0 )
					list->length = length;
				else
					list->length = 1;
				success |= 1;
				break;

			case filename: 
				list->opt = opt;
				if( length > 0 )
					list->length = length;
				else
					list->length = PRESENTATION_LENGTH;
				success |= 1;
				break;

			case presentation: 
				list->opt = opt;
				if( length > 0 )
					list->length = length;
				else
					list->length = PRESENTATION_LENGTH;
				success |= 1;
				break;

			case filetype: 
				list->opt = opt;
				if( length > 0 )
					list->length = length;
				else
					list->length = TYPE_LENGTH;
				success |= 1;
				break;

			case mode: 
				list->opt = opt;
				if( length > 0 )
					list->length = length;
				else
					list->length = 10;
				success |= 1;
				break;

			case size: 
				list->opt = opt;
				if( length > 0 )
					list->length = length;
				else
					list->length = SIZE_SIZE;
				success |= 1;
				break;

			default: systemlog( 1, "ERROR: set_tab_tolist: unknown option");
				success = -1;
				break;
			}

		switch( centration )
			{
			case -1: //left centration
				list->centration = centration;
				list->start = 0;
				success |= 2;
				break;

			case 0: //centar centration
				list->centration = centration;
				list->start = 0;
				success |= 2;
				break;

			case 1: //right centerd
				list->centration = centration;
				list->start = 0;
				success |= 2;
				break;

			default: systemlog( 1, "ERROR: set_tab_tolist: unknown centration" );
				success = -2;
				break;
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int set_win_workdir( Windowtype *win, char workdir[] )
	{
		int success = 0;

		DIR *dir = NULL;

		systemlog( 3, "set_win_workdir");

		if( workdir == NULL || strlen( workdir ) < 1 )
			{
				systemlog( 1, "ERROR: set_win_workdir: workdir is smaler then one!" );
				success = -1;
			}

		if( success != -1 && (dir = opendir( workdir)) )
			{
				closedir( dir );
				strcpy( win->wd, workdir );
				success |= 1;
			}
		else
			{
				systemlog( 1, "ERROR: set_win_workdir: cant read this directory!" );
				success = -1;
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD] (" );
				systemlog( 93, workdir );
				systemlog( 93, ")" );
			}

		return success;
	}

int set_win_showhiddenfiles( Windowtype *win, int boolean )
	{
		int success = 0;
		char error[ 10 ];

		systemlog( 3, "set_win_showhiden" );

		if( boolean != 1 && boolean != 0 )
			{
				systemlog(1, "ERROR: set_win_showhiden: non boolean value!" );
				success = -1;
			}
		else
			{
				win->show_hidden_files = boolean;
				success |= 1;
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD] (" );
				sprintf( error, "%d", boolean );
				systemlog( 93, error );
				systemlog( 93, ")" );
			}

		return success;
	}

int set_filetypeAction_tolist( filetypeAction **listptr, char filetype[], char command[] )
	{
		int success = 0;

		int i = 0;
		int v = 0;
		char check[ COMMANDLENGTH ];

		filetypeAction *list = NULL;

		systemlog( 3, "set_filetypeAction_tolist");

		//test if read access
		if( command[0] != '/' )
			{
				strcpy( check, "/bin/" );
				v = 5;
			}
		while( command[i] != '\0' && command[i] != ' ' )
			{
				check[i+v] = command[i];
				i++;
			}
			check[i+v] = '\0';
		if( !access( check, X_OK ) )
			{
				success |= 1;
			}
		else
			{
				systemlog( 1,"ERROR: set_filetypeAction_tolist: no execution rights" );
				success = -1;
			}

		//create a nev action to list
		if( success != -1 && *listptr == NULL )
			{
				systemlog( 4, "new Actionlist" );
				*listptr = malloc( sizeof(filetypeAction) );
				list = *listptr;
				list->next = NULL;
				list->prev = NULL;
			}
		else if( success != -1 )
			{
				list = *listptr;
				//find end of list
				while( list->next != NULL )
					list = list->next;

				list->next = malloc( sizeof(filetypeAction) );
				list->next->prev = list;
				list->next->next = NULL;

				list = list->next;
			}

		//test and copy values
		if(  success != -1 )
			{
				strcpy( list->filetype, filetype );
				strcpy( list->Action, command );
				success |= 1;
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD] (" );
				systemlog( 93, filetype );
				systemlog( 93, ")(" );
				systemlog( 93, command );
				systemlog( 93, ")");
				systemlog( 93, check );
			}

		return success;
	}

int set_shortcut_tolist( shortcutType **listptr, char label[], char dir_command[] )
	{
		int success = 0;

		shortcutType *list = NULL;

		systemlog( 3, "set_shortcut_tolist" );

		if( strlen(label)  > SHORTCUTLABEL_LENGTH  )
			{
				systemlog( 1, "ERROR: set_shortcut_tolist: nonvalid label lenght!: " );
				systemlog( 91, label );
				success = -1;
			}

		if( strlen(dir_command) > COMMANDLENGTH )
			{
				systemlog( 1, "ERROR: set_shortcut_tolist: nonvalid dir/command lenght: " );
				systemlog( 91, dir_command );
				success = -1;
			}

		if( *listptr == NULL )
			{
				systemlog( 4, "new shortcutlist" );
				*listptr = malloc( sizeof(shortcutType) );
				list = *listptr;
				list->prev = NULL;
				//list->next = NULL;
				//list->n = 1;

				//add special shortcut (workdir)
				list->next = malloc( sizeof(shortcutType) );
				list->next->prev = list;
				list->next->next = NULL;
				list->n = 1;
				list->highlighted = 0;
				strcpy( list->label, "-");
				strcpy( list->dir, "$passiv" );
				list->l = 1;
				list = list->next;

				list->n = 2;
				
			}
		else
			{
				list = *listptr;

				while( list->next != NULL )
					list = list->next;

				list->next = malloc( sizeof(shortcutType) );
				list->next->prev = list;
				list->next->next = NULL;

				list = list->next;
				list->n = list->prev->n +1;
			}

		if( success != -1 )
			{
				strcpy( list->label, label );
				strcpy( list->dir, dir_command );
				list->l = strlen( label );
				list->highlighted = 0;
				success |= 1;
			}

		if( success < 1 )
			{
				if( list->prev == NULL && list->next == NULL )
					{
						free( list );
						list = NULL;
					}
				else if( list->prev == NULL )
					{
						list = list->next;
						free( list->prev );
						list->prev = NULL;

						systemlog( 1, "WARNING: set_shortcut_tolist: list was in wrong order, this shold not be able to happen: removed first element!" );
					}
				else
					{
						list = list->prev;
						free( list->next );
						list->next = NULL;
					}
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD] (" );
				systemlog( 93, label );
				systemlog( 93, ") (" );
				systemlog( 93, dir_command );
				systemlog( 93, ")" );
			}

		return success;
	}

int set_soundtype_tolist( soundeffectType **listptr, char sound[], int action )
	{
		int success = 0;

		soundeffectType *list = NULL;

		systemlog( 3, "set_soundtype_tolist" );

		if( access( sound, R_OK )  && action != -1 )
			success = -1;

		if( *listptr == NULL )
			{
				systemlog( 4, "new soundtype list" );
				*listptr = malloc( sizeof(soundeffectType) );
				list = *listptr;
				list->prev = NULL;
				list->next = NULL;
			}
		else
			{
				list = *listptr;

				while( list->next != NULL )
					list = list->next;

				list->next = malloc( sizeof(soundeffectType) );
				list->next->prev = list;
				list->next->next = NULL;

				list = list->next;
			}

		if( success != -1 )
			{
				list->action = action;
				strcpy( list->sound, sound );
				success |= 1;
			}

		if( !success )
			{
				if( list->prev == NULL && list->next == NULL )
					{
						free( list );
						list = NULL;
					}
				else if( list->prev == NULL )
					{
						list = list->next;
						free( list->prev );
						list->prev = NULL;
						systemlog( 1, "ERROR: set_soundtype_tolist: list was in wrong order, this shold not be able to happen: removed first element!" );
					}
				else
					{
						list = list->prev;
						free( list->next );
						list->next = NULL;
					}
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD] (" );
				systemlog( 93, sound );
				systemlog( 93, ")" );
			}

		return success;
	}

int set_animations( int onoff )
	{
		int success = 0;
		systemlog( 3, "set_animation");

		ANIMATING = onoff;
		success |= 1;

		if( success )
			systemlog( 93, "\t[DONE]" );
		else
			systemlog( 94, "\t[FAILD]" );

		return success;
	}

//----------------------------------------------------------------------------------------------------------------
int put_default_terminalname()
	{
		int success = 0;

		systemlog( 3, "put_default_terminalname" );

		//if( TERMINALNAME[0] == '\0' )
			success = set_terminalname( "xterm" );

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_dialogwinreactions()
	{
		int success = 0;

		systemlog( 3, "put_default_dialogwinreactions" );

		if( DW_REACTION == -1 )
			success = set_dialogwinreactions( 0 );

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_logpriority()
	{
		int success = 0;

		systemlog( 3, "put_default_logpriority" );

		success = set_logpriority( 0 );

		if( success > 0 )
			systemlog( 93, "\t[DONE]" );
		else
			systemlog( 93, "\t[FAILD]");

		return success;
	}

int put_default_logpath()
	{
		int success = 0;
		char workdir[ 1024 ];

		systemlog( 3, "put_default_logpath" );

		//if( FILEPATH_LOG[0] == '\0' )
			{
				if( !getcwd( workdir, sizeof(workdir) ) )
					{
						systemlog( 1, "WARNING: put_default_logpath: could not get current workdir, redirecting to /tmp" );
						success = set_logpath( "/tmp" );
					}
				else
					{
						success = set_logpath( workdir );
					}
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_workdir_type( Windowtype *win )
	{
		int success = 0;

		systemlog( 3, "put_default_workdir_type" );

		if( win->orientation_workdir == -1 || win->type_workdir == -1 )
			success = set_win_workdir_type( win, 0, 2 );

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_shortcut_type( Windowtype *win )
	{
		int success = 0;

		systemlog( 3, "put_default_shortcut_type" );

		if( win->orientation_shortcuts == -1 )
			success = set_win_shortcut_type( win, 0, 10 );

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_filelist_type( Windowtype *win )
	{
		int success = 0;
		systemlog( 3, "put_default_filelist_type" );

		if( win->orientation_filelist == -1 )
			success =  set_win_filelist_type( win, 0, 1);

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_win_colors( Windowtype *win )
	{
		int success = 0;
		int num = 0;

		systemlog( 3, "put_default_win_colors" );

		for( num = NUMBEROFCOLORS; num >= 0; num-- )
			{
				if( win->color[num] == -2 )
					{
						//success = set_win_color( win, num, -1 );
						win->color[num] = -1; success = 1;
					}
				else
					{
						//color is already set -> also success!! :D
						success = 1;
					}
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_list_tabs( tabtype **listptr )
	{
		int success = 0;

		systemlog( 3, "put_default_list_tabs" );

		if( *listptr == NULL )
			success = set_tab_tolist( listptr, filename, 0, 0 );

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_win_workdir( Windowtype *win )
	{
		int success = 0;
		char workdir[ 1024 ];

		systemlog( 3, "put_default_win_workdir" );

		//if( win->wd[0] == '\0' )
			{
				if( !getcwd( workdir, sizeof(workdir) ) )
					{
						systemlog( 1, "WARNING: put_default_win_workdir: could not get current workdir, redirecting to /" );
						success = set_win_workdir( win, "/" );
					}
				else
					{
						success = set_win_workdir( win, workdir );
					}
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			{
				systemlog( 93, "\t[FAILD]" );
				systemlog( 93, "("); systemlog( 93, win->wd ); systemlog( 93, ")("); systemlog( 93, workdir ); systemlog( 93, ")");
			}

		return success;
	}

int put_default_win_showhidden( Windowtype *win )
	{
		int success = 0;

		systemlog( 3, "put_default_win_showhidden");

		if( win->show_hidden_files == -1 )
			success = set_win_showhiddenfiles( win, 0 );

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_list_filetypeAction( filetypeAction **listptr )
	{
		int success = 0;

		systemlog( 3, "put_default_list_filetypeAction" );

		if( *listptr == NULL )
			success = set_filetypeAction_tolist( listptr, " ", " " );

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_list_shortcuts( shortcutType **listptr )
	{
		int success = 0;

		systemlog( 3, "put_default_list_shortcuts" );

		if( *listptr == NULL )
			{
				success = set_shortcut_tolist( listptr, "-", " " );
			}

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_list_sounds( soundeffectType **listptr )
	{
		int success = 0;

		systemlog( 3, "put_default_list_sounds" );

		if( *listptr == NULL )
			success = set_soundtype_tolist( listptr, " ", -1 );

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

int put_default_animations()
	{
		int success = 0;
		systemlog( 3, "put_default_animations" );

		success = set_animations( 0 );

		if( success )
			systemlog( 93, "\t[DONE]" );
		else
			systemlog( 93, "\t[FAILD]" );
		return success;
	}

int put_start_totablist( tabtype *list, int win_widh )
	{
		int success = 0;

		int tab_widh = 0;
		int spare_widh = 0;

		int last_spare_widh = 0;
		int objects_left = 0;
		int not_fitted[ N_MAX_TABS ];
		int i;

		char errortest[56];

		systemlog( 3, "cget_start_totablist" );

		for( i = N_MAX_TABS; i >= 0; i-- )
			not_fitted[i] = 1;

		if( list != NULL )
			{
				while( list->next != NULL )
					list = list->next;
			}
		else
			{
				systemlog( 1, "ERROR: cget_start_totablist: empty list was passed" );
				success = -1;
			}

		if( list->n <= N_MAX_TABS )
			{
				objects_left = list->n +1;
				//tab_widh = (win_widh - (win_widh % list->n) )/ list->n;
				tab_widh = (win_widh - (win_widh % (list->n+1)) )/ (list->n+1);
				//spare_widh = win_widh - tab_widh * list->n;
				spare_widh = win_widh - tab_widh * (list->n+1);

				do //get sparespace
					{
						last_spare_widh = spare_widh;

						if( list->next == NULL )
							{
								while( list->prev != NULL )
									{
										if( not_fitted[ list->n ] && list->length <= tab_widh + (spare_widh - spare_widh%objects_left) / objects_left )
											{
												spare_widh = spare_widh + tab_widh - list->length;
												not_fitted[ list->n ] = 0;
												objects_left--;
											}

										list = list->prev;
									}
								//test the last one too
								if( not_fitted[ list->n ] && list->length <= tab_widh + (spare_widh - spare_widh%objects_left) / objects_left )
									{
										spare_widh = spare_widh + tab_widh - list->length;
										not_fitted[ list->n ] = 0;
										objects_left--;
									}
							}
						else if( list->prev == NULL )
							{
								while( list->next != NULL )
									{
										if( not_fitted[ list->n ] && list->length <= tab_widh + (spare_widh - spare_widh%objects_left) / objects_left )
											{
												spare_widh = spare_widh + tab_widh - list->length;
												not_fitted[ list->n ] = 0;
												objects_left--;
											}

										list = list->next;
									}
								//test the last one too
								if( not_fitted[ list->n ] && list->length <= tab_widh + (spare_widh - spare_widh%objects_left) / objects_left )
									{
										spare_widh = spare_widh + tab_widh - list->length;
										not_fitted[ list->n ] = 0;
										objects_left--;
									}
							}
						else
							{
								systemlog( 1, "ERROR: calc_tabs_start: this alogaritm didn't work as planed." );
								success = -1;
							}

					} while( spare_widh > last_spare_widh && objects_left );
			}
		else
			{
				systemlog( 1, "ERROR: calc_tabs_start: to many tabs!" );
				success = -1;
			}

		sprintf( errortest, "tabwidth: %d sparewidth: %d objects_left: %d", tab_widh, spare_widh, objects_left );
		systemlog( 4, errortest );

		while( list->prev != NULL && success != -1 )
			list = list->prev;

		do
			{
				if( list->n != 0 )
					{
						if( 0 == not_fitted[list->prev->n] )
							{
								list->start = list->prev->start + list->prev->length;
							}
						else
							{
								list->start = list->prev->start + tab_widh  + spare_widh/ ( objects_left );
								spare_widh -= spare_widh / ( objects_left );
								objects_left--;
							}
						sprintf( errortest, "lenght: %d start: %d win_widh: %d not_fitted: %d", list->length, list->start, win_widh, not_fitted[list->n] );
						systemlog( 4, errortest );
					}
				else
					{
						list->start = 0;
						sprintf( errortest, "lenght: %d start: %d win_widh: %d not_fitted: %d", list->length, list->start, win_widh, not_fitted[list->n] );
						systemlog( 4, errortest );
					}

				if( list->next != NULL )
					list = list->next;
				else
					break;

			}while( success != -1 );

		if( success != -1 )
			success = 1;

		if( success > 0 )
			systemlog( 93, "\t[DONE]");
		else
			systemlog( 93, "\t[FAILD]" );

		return success;
	}

tabtype *find_tab( tabtype *tab, int n )
	{
		systemlog( 4, "find_tab" );

		while( tab->prev != NULL && tab->n != n )
			tab = tab->prev;

		while( tab->next != NULL && tab->n != n )
			tab = tab->next;

		if( tab->next == NULL && tab->n != n )
			systemlog( 1, "ERROR: find_tab: could not find tab\n" );

		return tab;
	}

//----------------------------------------------------------------------------------------------------------------

int pget_configname()
	{
		int success = 0;
		int c = 0;
		char buff[250];
		char tmp[250];
		FILE *fp;
		DIR *dir;
		struct dirent* file;
		struct stat buf;

		systemlog( 2, "pget_configname" );

		fp = popen( "echo $XDG_CONFIG_HOME/jeti.rc", "r" );
		while( ( buff[c] = fgetc( fp )) != EOF )
			{ c++; } buff[c-1] = '\0';

		if( !access( buff, R_OK ) )
			success = set_configname( buff );

		c = 0; fp = popen( "echo $XDG_CONFIG_HOME/jeti.d", "r" );
		while( ( buff[c] = fgetc( fp )) != EOF )
			{ c++; } buff[c-1] = '\0';

		if( !access( buff, R_OK ) )
			{
				dir = opendir( buff );
				while( ( file = readdir( dir ) ) )
					{
						strcpy( tmp, buff );
						strcat( tmp, "/"  );
						strcat( tmp, file->d_name );

						//only use regular files that are marked as executeble
						lstat( tmp, &buf );
						if(  S_ISREG(buf.st_mode) && ( S_IEXEC & buf.st_mode ) )
							{
								success += set_configname( tmp );
							}
					}
				closedir( dir );
			}

		if( success == 0 )
			{
				c = 0; fp = popen( "echo $HOME/.jeti.rc", "r" );
				while( ( buff[c] = fgetc( fp ) ) != EOF )
					c++; buff[c-1] = '\0';

				if( !access( buff, R_OK ) )
					success = set_configname( buff );
			}

		pclose( fp );

		if( success > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 92, "\t[FAILD]" );

		return success;
	}

int fget_terminalname( char buff[] )
	{
		int found_valid = 0;

		int i = 0; //instruction reading
		int v = 0; //valu reading

		char tmpterminalname[256];;

		systemlog( 4, "fget_terminalname" );

		tmpterminalname[0] = '\0';

		//iggnor blankspace in the beginning
		while( buff[i] == ' ' || buff[i] == '\t' )
			i++;

		if( !strncmp( buff+i, "terminal.name: ", 15 ) )
			{i+=15;

				//iggnor blankspace before variable value
				while( buff[i] == ' ' || buff[i] == '\t' )
					i++;

				//read value
				while( buff[i+v] != '\n' )
					{
						tmpterminalname[v] = buff[i+v];
						v++;
					}
					tmpterminalname[v] = '\0';

				//test and set value
				if( tmpterminalname[0] != '\0' )
					{
						if( (found_valid = set_terminalname( tmpterminalname )) == 0 )
							{
								systemlog( 1, "ERROR: fget_terminalname: set_terminalname: an unrecognized problem detected!" );
								found_valid = -1;
							}
					}
				else
					{
						systemlog(1, "WARNING: fget_terminalname: terminalname was blank!");
						found_valid = -1;
					}
			}

		if( found_valid > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_dialogwinreactions( char buff[] )
	{
		int found_valid = 0;

		int i = 0; //instruction reading

		int dw_reaction_temp = 0;

		systemlog( 4, "fget_dialogwinreactions" );

		//ignore blankspace in the beginning
		while( buff[i] == ' ' || buff[i] == '\t' )
			i++;

		if( !strncmp( buff+i, "dialogwindow.reactions: ", 24 ) )
			{i+=24;
				//ignore blankspace before value
				while( buff[i] == ' ' || buff[i] == '\t' )
					i++;

				//read value
				if( buff[i] != '\n' && buff[i] != '0' && buff[i+1] != 'x' )
					{
						dw_reaction_temp = (int) strtol( buff+i, NULL, 10 );
					}
				else if( buff[i] == '0' && buff[i+1] == 'x'  )
					{
						systemlog( 1, "ERROR: fget_dialogwinreactions: reading value in hex, is still under construction!" );
					}
				else
					{
						systemlog( 1, "ERROR: fget_dialogwinreactions: cant get value from config line, dialogwindow.reactions!" );
						found_valid = -1;
					}

				//test and set value
				if( (found_valid = set_dialogwinreactions( dw_reaction_temp )) == 0 )
					{
						systemlog( 1, "ERROR: fget_dialogwinreactions: set_dialogwinreactions: an unrecognized problem detected" );
						found_valid = -1;
					}
			}


		if( found_valid > 0)
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_logpath( char buff[] )
	{
		int found_valid = 0;

		int prio = 0;

		int i = 0; //instruction reading
		int v = 0; //valu reading

		char tmplogpath[256];

		systemlog( 4, "fget_logpath" );

		tmplogpath[0] = '\0';

		//ignore blankspace in the beginning
		while( buff[i] == ' ' || buff[i] == '\t' )
			i++;

		if( !strncmp( buff+i, "logfile.path&prio: ", 14 ) )
			{i+=14;
				while( buff[i] != '\n' && buff[i] != '\0' )
					{
						if( buff[i] >= 48 && buff[i] <= 57 )
							{ // an integer was found (logpriority)
								prio = (int) strtol( buff+i, NULL, 10 );

								while( buff[i] != ' ' && buff[i] != '\t' && buff[i] != '\n' && buff[i] != '\0' )
									i++;
							}
						else if( buff[i] == '/' )
							{ //path reading
								while( buff[i+v] != ' ' && buff[i+v] != '\n' && buff[i+v] != '\0' )
									{
										tmplogpath[v] = buff[i+v];
										v++;
									}
								tmplogpath[v] = '\0';
								i += v;
								v = 0; 
							}
						else if( buff[i] == '"' )
							{ i++; //path reading with citat
								while( buff[i+v] != '"' && buff[i+v] != '\n' && buff[i+v] != '\0' )
									{
										tmplogpath[v] = buff[i+v];
										v++;
									}
								tmplogpath[v] = '\0';
								i += v + 1;
								v = 0;
							}
						else
							i++;
					}

				//test and set
				if( tmplogpath[0] != '\0'  )
					{
						if( (found_valid = set_logpath( tmplogpath )) == 0 )
							{
								systemlog( 1, "ERROR: fget_logpath: set_logpath: a unrecognized problem detected!");
								found_valid = -1;
							}

						set_logpriority( prio );
					}
				else
					{
						systemlog( 1, "ERROR: fget_logpath: coud not find value!" );
					}
			}

		if( found_valid > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_win_workdir_type( char buff[], Windowtype *win )
	{
		int found_valid = 0;

		int type = 0;
		int orientation = -1;

		int i = 0; //instruction reading

		systemlog( 4, "fget_win_workdir_type" );

		//ignore blanks
		while( buff[i] == ' ' || buff[i] == '\t' )
			i++;

		if( !strncmp( buff+i, "window.workdir.orientation:",27 ) )
			{i+=27;
				while( buff[i] != '\n' && buff[i] != '\0' )
					{
						//ignore blanks between stuff
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;

						if( buff[i] >= 48 && buff[i] <= 57 )
							{//an int value was found/
								orientation = (int) strtol( buff +i, NULL, 10 );

								while( buff[i] != ' ' && buff[i] != '\t' && buff[i] != '\n' && buff[i] != '\0' )
									i++;
							}
						else if( !strncmp( buff+i, "mirror", 6 ) )
							{i+=6;
								type |= 2;
							}
						else if( !strncmp( buff+i, "leaf", 4 ) )
							{i+=4;
								type |= 1;
							}
						else if( !strncmp( buff+i, "fullpath", 8 ) )
							{i+=8;
								type = !( type & 1 );
							}
						else
							i++;
					}

				if( orientation != -1 )
					{
						if( (found_valid = set_win_workdir_type( win, type, orientation ) ) == 0 )
							systemlog( 1, "ERROR: fget_win_workdir_type: set_win_workdir_type: unknown error uccured" );
					}
				else
					{
						systemlog( 1, "ERROR: fget_win_workdir_type: no orientation value found");
						found_valid = -1;
					}
			}

		if( found_valid )
			systemlog( 92, "\t[DONE]" );
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_win_shortcut_type( char buff[], Windowtype *win )
	{
		int found_valid = 0;

		int type = 0;
		int orientation = -1;

		int i = 0; //instruction reading

		systemlog( 4, "fget_win_shortcut_type");

		if( !strncmp( buff+i, "window.shortcut.orientation:", 28) )
			{i += 28;
				while( buff[i] != '\n' && buff[i] != '\0' )
					{
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;

						if( buff[i] >= 48 && buff[i] <= 57 )
							{
								orientation = (int) strtol( buff +i, NULL, 10 );

								while( buff[i] != ' ' && buff[i] != '\t' && buff[i] != '\n' && buff[i] != '\0' )
									i++;

							}
						else if( !strncmp( buff+i, "mirror", 6 ) )
							{ i+=6;
								type |= 1;
							}
						else
							i++;
					}

				if( orientation != -1 )
					{
						if( ( found_valid = set_win_shortcut_type( win, type, orientation ) ) == 0 )
							systemlog( 1, "ERROR: fget_win_shortcut_type: set_win_shortcut_type: an unknown error detected" );
					}
				else
					{
						systemlog( 1, "ERROR: fget_win_shortcut_type: no orientation value found" );
						found_valid = -1;
					}
			}

		if( found_valid )
			systemlog( 92, "\t[DONE]" );
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_win_filelist_type( char buff[], Windowtype *win )
	{
		int found_valid = 0;

		int type = 0;
		int orientation = -1;

		int i = 0; //instruction reading

		systemlog( 4, "fget_win_filelist_type" );

		if(  !strncmp( buff+i, "window.files.orientation:", 25 ) )
			{i+=25;
				while( buff[i] != '\n' && buff[i] != '\0' )
					{
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;

						if( buff[i] >= 48 && buff[i] <= 57 )
							{
								orientation = (int) strtol( buff+i, NULL , 10 );

								while( buff[i] != ' ' && buff[i] != '\t' && buff[i] != '\n' && buff[i] != '\0' )
									i++;
							}
						else if( !strncmp( buff+i, "mirror", 6 ) )
							{i+=6;
								type |= 1;
							}
						else
							i++;
					}
				if( orientation != -1 )
					{
						if( ( found_valid = set_win_filelist_type( win, type, orientation ) ) == 0  )
							systemlog( 1, "ERROR: fget_win_filelist_type: set_win_filelist_type: an unknown error detected" );
					}
				else
					{
						systemlog( 1, "ERROR: fget_win_filelist_type: no orientation value found" );
						found_valid = -1;
					}
			}

		if( found_valid )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_win_colors( char buff[], Windowtype *win )
	{
		int found_valid = 0;

		int i = 0; //instruction reading
		int type = 0;

		systemlog( 4, "fget_win_colors" );

						//ignorre blankspace in the beginning
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;

						if( !strncmp( buff+i, "window.", 7 ) )
							{i+=7;
								if( !strncmp( buff+i, "color.", 6 ) )
									{i+=6;
										if( !strncmp( buff+i, "default:", 8 ) )   
											{i+=8;
												type = 0;
											}
										else if( !strncmp( buff+i, "executeble:", 11 ) )
											{i+=11;
												type = 1;
											}
										else if( !strncmp( buff+i, "katalog:", 8 ) )
											{i+=8;
												type = 2;
											}
										else if( !strncmp( buff+i, "link:", 5 ) )   
											{i+=5;
												type = 3;
											}
										else if( !strncmp( buff+i, "fifo:", 5 ) )   
											{i+=5;
												type = 4;
											}
										else if( !strncmp( buff+i, "char:", 5 ) )   
											{i+=5;
												type = 5;
											}
										else if( !strncmp( buff+i, "block:", 6 ) )   
											{i+=6;
												type = 6;
											}
										else if( !strncmp( buff+i, "error:", 6 ) )   
											{i+=6;
												type = 7;
											}
										else if( !strncmp( buff+i, "selected:", 9 ) )   
											{i+=9;
												type = 8;
											}
										else if( !strncmp( buff+i, "background:", 11 ) )
											{i+=11;
												type = 9;
											}
										else if( !strncmp( buff+i, "highlight:", 10 ) )
											{i+=10;
												type = 10;
											}
										else if( !strncmp( buff+i, "borders:",8 ) )
											{i+=8;
												type = 11;
											}
										else if( !strncmp( buff+i, "bordertext: ", 11 ) )
											{i+=11;
												type = 12;
											}
										else
											{
												systemlog( 1, "ERROR: fget_win_colors: unrecognized filetype" );
												found_valid = -1;
											}

										//ignore blankspace before value
										while( buff[i] == ' ' || buff[i] == '\t' )
											i++;

										//read, check and set value
										if( buff[i] != '\n' && buff[i] != '0' && buff[i+1] != 'x' )
											{
												if( (found_valid = set_wincolor( win, type, (int) strtol( buff+i, NULL, 10) )) == 0 )
													{
														systemlog( 1, "ERROR: fget_win_colors: set_wincolor: an unrecognized problem detected!" );
														found_valid = -1;
													}
											}
										else if( buff[i] == '0' && buff[i+1] == 'x' )
											{
												systemlog( 1, "ERROR: fget_win_colors: reading value in hex, is still under construction!" );
											}
										else
											{
												systemlog( 1, "ERROR: fget_win_colors: typ was found but no value was read!");
											}
									}
							}

		if( found_valid > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_tabs( char buff[], tabtype **listptr, int widh )
	{
		int found_valid = 0;

		int centration = 0;
		printoption opt = nothing;
		int length = 0;

		int i = 0; //instruction reading

		systemlog( 4, "fget_tabs" );
						//ignore blanks in the beginning
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;

						//read instructions
						if( !strncmp( buff+i, "window.tab.", 11 ) )
							{i+=11;

								//get centration
								if( !strncmp( buff+i, "left: ", 6 ) )
									{
										i += 6;
										centration = -1;
										found_valid += 1;
									}
								else if( !strncmp( buff+i, "center: ", 8 ) )
									{
										i += 8;
										centration = 0;
										found_valid += 1;
									}
								else if( !strncmp( buff+i, "right: ", 7 ) )
									{  
										i += 7;
										centration = 1;
										found_valid += 1;
									}
								else
									{ /*ERROR no tab-direction*/ 
										systemlog( 1, "ERROR: fget_tabs:no tab direction\n" );  
										found_valid = -1;
									}

								if( found_valid & 1 )
									systemlog( 4, "window.tab.<direction>: was found" );

								//ignore blanks
								while( buff[i] == ' ' || buff[i] == '\t' )
									i++;

								//get option
								if( !strncmp( buff+i, "selected", 8 ) )
									{  i+=8;
										opt = selected; 
										found_valid += 2;
									}
								else if( !strncmp( buff+i, "filename", 8) )
									{  i+=8;
										opt = filename;
										found_valid += 2;
									}
								else if( !strncmp( buff+i, "presentation", 12) )
									{  i+=12;
										opt = presentation;
										found_valid += 2;
									}
								else if( !strncmp( buff+i, "filetype", 8 ) )
									{  i+=8; 
										opt = filetype;
										found_valid += 2;
									}
								else if( !strncmp( buff+i, "size", 4 ) )
									{  i+=4; 
										opt = size;
										found_valid += 2;
									}
								else if( !strncmp( buff+i, "mode", 4 ) )
									{  i+=4;
										opt = mode;
										found_valid += 2;
									}
								else if( !strncmp( buff+i, "nothing", 7 ) )
									{  i+=7; 
										opt = nothing;
										found_valid += 2;
									}
								else 
									{ /*ERROR tab length*/ 
										opt = nothing;
										found_valid = -1;
										systemlog( 1, "ERROR: fget_tabs: unrecognized tab option, " ); 
									}

								//ignore blanks
								while( buff[i] == ' ' || buff[i] == '\t' )
									i++;

								//read lengt if there is oneh
								if( buff[i] != '\n' )
									{
										length = (int) strtol( buff+i, NULL, 10);
									}

								if( found_valid & 2 )
									systemlog( 4, "mode value was found" );

								if( found_valid != -1 )
									{
										if( (found_valid = set_tab_tolist( listptr, opt, centration, length ) ) == 0 )
											systemlog( 1, "ERROR: fget_tabs: set_tab_tolist: unknown error");

										if( *listptr == NULL )
											systemlog( 1," ERROR: fget_tabs: list got back as NULL" );
									}
							}

		if( found_valid > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_win_workdir( char buff[], Windowtype *win )
	{
		int found_valid = 0;

		int i = 0; //instruction reading
		int v = 0; //instruction values reading

		char workdir[1024];

		systemlog( 4, "fget_win_workdir" );

						/*ignorre blankspace in the beginning*/
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;

						//read instructions
						if( !strncmp( buff+i, "window.startdirectory: ", 23 ) )
							{ i+=23;

								systemlog( 4, "window.startdirectory: was found");

								//ignore blanks before value
								while( buff[i] == ' ' || buff[i] == '\t' )
									i++;

								//read values
								if( !strncmp( buff+i, "cwd", 3 ) )
									{
										//get workdirectory as start dir
										if( !getcwd( workdir, sizeof(workdir) ) )
											{
												systemlog( 1, "ERROR: fget_win_workdir: getcwd()");
												found_valid = -1;
											}
									}
								else if( buff[i] == '\"' )
									{i++;
										while( buff[i+v] != '\"' && buff[i+v] != '\n' )
											{
												workdir[v] = buff[i+v];
												v++;
											}
										workdir[v] = '\0';
									}
								else if( buff[i] == '/' )
									{i++;
										while( buff[i+v] != '\n')
											{
												workdir[v] = buff[i+v];
												v++;
											}
										workdir[v] = '\0';
									}
								else
									{
										systemlog( 1, "ERROR: fget_win_workdir: window.startdirectory found, but no valid value" );
										found_valid = -1;
									}

								//set value
								if( found_valid >= 0 )
									{
										if( (found_valid = set_win_workdir( win, workdir )) == 0 )
											systemlog( 1, "ERROR: fget_win_workdir: set_win_workdir: unknown error accurred" );

										if( strlen( win->wd ) < 1 )
											systemlog( 1, "ERROR: fget_win_workdir: set_win_workdir: returned NULL" );
									}
							}

		if( found_valid > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_win_showhidden( char buff[], Windowtype *win )
	{
		int found_valid = 0;

		int i = 0;

		systemlog( 4, "fget_win_workdir" );

						/*ignorre blankspace in the beginning*/
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;

						//read instruction
						if( !strncmp( buff+i, "window.showhiden: ", 18 ) )
							{ i+=18;
								systemlog( 4, "window.showhiden: was found" );

								//ignore blanks before value
								while( buff[i] == ' ' || buff[i] == '\t' )
									i++;

								//read values
								if( buff[i] == '\n' )
									{
										systemlog( 1, "ERROR: fget_win_workdir: win.showhidden was found but no valid value" );
										found_valid = -1;
									}
								else if( !strncmp(  buff+i, "true", 4 ) )
									{
										found_valid = set_win_showhiddenfiles( win, 1 );
									}
								else if( !strncmp( buff+i, "false", 5 ) )
									{
										found_valid = set_win_showhiddenfiles( win, 0 );
									}
								else
									{
										found_valid = set_win_showhiddenfiles( win, (int) strtol( buff+i, NULL, 10 ) );
									}
							}

		if( found_valid > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_fileAction( char buff[], filetypeAction **listptr )
	{
		int found_valid = 0;

		char filetype[TYPE_LENGTH];
		char command[COMMANDLENGTH];

		int i = 0; //instruction reading
		int v = 0; //instruction values reading

		systemlog( 4, "fget_fileAction" );;

						/*ignore blankspace in the beginning*/
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;
					
						//read instructions
						if( !strncmp( buff+i, "file.type: ", 11 ) )
							{ i+=11; systemlog( 4, "found file.type" );

								while( buff[i] != '\"' )
									i++;
								i++;

								//reset command
								memset( command, 0, sizeof( command ) );

								//read values: command
								while( buff[i+v] != '\"' )
									{
										command[v] = buff[i+v];
										v++;
									}
									command[v] = '\0';
								if( command == NULL )
									{
										systemlog( 1, "ERROR: fget_fileAction: invalid command" );
										found_valid = -1;
									}

								//compensat for end "
								i += v + 1;
								v = 0;

								//ignore blanks
								while( buff[i] == ' ' || buff[i] == '\t' )
									i++;

								//read values: filetypes
								while( buff[i+v] != '\n' )
									{
										//read part
										if( buff[i+v] == '\'' && buff[i+v+1] == ' ' && buff[i+v+2] == '\'' )
											{ //the no filetype, filetype
												strcpy( filetype, " " );
												v += 2;
											}
										else
											filetype[v] = buff[i+v];

										v++;

										//set part
										if( buff[i+v] == ',' || buff[i+v] == '\n' )
											{
												filetype[v] = '\0';
												if( filetype == NULL )
													{
														systemlog( 1, "ERROR: fget_fileAction: invalid filetype");
														found_valid = -1;
													}
												else
													{
														if( (found_valid = set_filetypeAction_tolist( listptr, filetype, command )) == 0 )
															systemlog( 1, "ERROR: fget_fileAction: unknown error accured" );

														if( *listptr == NULL )
															systemlog( 1, "ERROR: fget_fileAction: list got back NULL" );
													}

												memset( filetype, 0, sizeof( filetype ) );

												if( buff[i+v] == '\n' )
													i += v; 
												else
													i += v + 1;    
												v = 0;
											}
									}
							}

		if( found_valid > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_shortcuts( char buff[], shortcutType **listptr )
	{
		int found_valid = 0;

		char label[ SHORTCUTLABEL_LENGTH +1];
		char dir_command[ COMMANDLENGTH +1];

		int i = 0; //instruction reading
		int v = 0; //instruction values reading

		systemlog( 4, "fget_shortcuts" );

		 				//ignore blankspace in the beginning
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;

						//read instruction
						if( !strncmp( buff+i, "window.shortcut: ", 17 ) )
							{ i+=17;
								//find label
								while( buff[i] != '\'' && buff[i] != '\n')
									i++;

								i += 1;

								//read label
								v = 0;
								while( buff[i+v] != '\'' && buff[i+v] != '\n' && v <= SHORTCUTLABEL_LENGTH )
									{
										label[v] = buff[i+v];
										v++;
									}
								label[v] = '\0';
								i += v;

								//find dir/command
								while( buff[i] != '\"' && buff[i] != '\n' )
									i++;

								i += 1;

								//read dir/command
								v = 0;
								while( buff[i+v] != '\"' && buff[i+v] != '\n' && v <= COMMANDLENGTH )
									{	
										dir_command[v] = buff[i+v];
										v++;
									}
								dir_command[v] = '\0';

								//test and set
								if( label == NULL )
									{
										systemlog( 1, "ERROR: fget_shortcuts: no label found");
										found_valid = -1;
									}
								else if( dir_command == NULL )
									{
										systemlog( 1, "ERROR: fget_shortcuts: no directory or command found" );
										found_valid = -1;
									}
								else
									{
										if( (found_valid = set_shortcut_tolist( listptr, label, dir_command )) == 0 )
											systemlog( 1, "ERROR: fget_shortcuts: an unexpected error accured");

										if( *listptr == NULL )
											systemlog( 1, "ERROR: fget_shortcuts: list got back as NULL!" );
									}

							}	

		if( found_valid > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

int fget_soundeffects( char buff[], soundeffectType **listptr )
	{
		int found_valid = 0;

		char sound[ LENGTH_PATH ];
		int action = 0;

		int i = 0; //instruction reading
		int v = 0; //instruction values reading

		systemlog( 4, "fget_soundeffects" );

						//ignore blankspace in the beginning
						while( buff[i] == ' ' || buff[i] == '\t' )
							i++;

						//read instructions
						if( !strncmp( buff, "sound.", 6 ) )
							{ i+=6;
								if( !strncmp( buff+i, "enter.", 6 ) )
									{ i+=6;
										if( !strncmp( buff+i, "doubledot: ", 10 ) )
											{ i+=10; action = 1; }
										else if( !strncmp( buff+i, "singledot: ", 11 ) )
											{ i+=11; action = 2;}
										else if( !strncmp( buff+i, "kat: ", 5 ) )
											{ i+=5; action = 3; }
										else if( !strncmp( buff+i, "executeble: ", 12 ) )
											{ i+=12; action = 4; }
										else if( !strncmp( buff+i, "scrollupp: ", 11 ) )
											{ i+=11; action = 5; }
										else if( !strncmp( buff+i, "scrolldown: ", 12 ) )
											{ i+=12; action = 6; }
										else if( !strncmp( buff+i, "stepupp: ", 9 ) )
											{ i+=9; action = 7; }
										else if( !strncmp( buff+i, "stepdown: ", 10 ) )
											{ i+=10; action = 8; }
										else if( !strncmp( buff+i, "selectfile: ", 12 ) )
											{ i+=12; action = 9; }
										else if( !strncmp( buff+i, "copyfiles: ", 11 ) )
											{ i+=11; action = 10; }
										else if( !strncmp( buff+i, "movefiles: ", 11 ) )
											{ i+=11; action = 11; }
										else if( !strncmp( buff+i, "removefiles: ", 13 ) )
											{ i+=13; action = 12; }
										else if( !strncmp( buff+i, "shortcut: ", 10 ) )
											{ i+=10; action = 13; }
										else if( !strncmp( buff+i, "startup: ", 9 ) )
											{ i+=9; action = 14; }
										else if( !strncmp( buff+i, "quit: ", 6 ) )
											{ i+=6; action = 15; }
										else if( !strncmp( buff+i, "fileaction: ", 12 ) )
											{ i+=12; action = 16; }

										//ignore blankspace
										while( buff[i] == ' ' || buff[i] == '\t' || buff[i] == '\"' )
											i++;

										//get values
										while( buff[i+v] != '\"' && buff[i+v] != '\n' )
											{
												sound[v] = buff[i+v];
												v++;
											}
										sound[v] = '\0';

										if( !action )
											{
												systemlog( 1, "ERROR: fget_soundeffects: got no action!" );
												found_valid = -1;
											}
										else if ( sound == NULL )
											{
												systemlog( 1, "ERROR: fget_soundeffects: got no soundpath" );
												found_valid = -1;
											}
										else
											{
												if( (found_valid = set_soundtype_tolist( listptr, sound, action )) == 0 )
													systemlog( 1, "ERROR: fget_soundeffects: unespected error acurred" );

												if( *listptr == NULL )
													systemlog( 1, "ERROR: fget_soundeffects: list got back as NULL!" );
											}
									}
							}

		if( found_valid > 0 )
			systemlog( 92, "\t[DONE]");
		else
			systemlog( 94, "\t[FAILD] " );

		return found_valid;
	}

int fget_animations( char buff[] )
	{
		int found_valid = 0;

		int i = 0; //instruction reading
		int value = -1;

		systemlog( 4, "fget_animations" );

		//ignore blankspace in the beginning
		while( buff[i] == ' ' || buff[i] == '\t' )
			i++;

		if( !strncmp( buff+i, "window.animations:", 18 ) )
			{ i += 18;
				while( buff[i] != '\n' && buff[i] != '\0' )
					{
						if( buff[i] >= 48 && buff[i] <= 57 )
							{ //an int value was found
								value = (int)strtol( buff +i, NULL, 10 );

								while( buff[i] != ' ' && buff[i] != '\t' && buff[i] != '\n' && buff[i] != '\0' )
									i++;
							}
						else if( !strncmp( buff+i, "true", 4 ) )
							{ i += 4;
								value = 1;
							}
						else if( !strncmp( buff+i, "false", 5 ) )
							{ i += 4;
								value = 0;
							}
						else
							i++;
					}

				if( ( found_valid = set_animations( value ) ) == 0 )
					systemlog( 1, "ERROR: fget_animations: set_animations: an unknown error accured");
			}

		if( found_valid )
			systemlog( 93, "\t[DONE]" );
		else
			systemlog( 94, "\t[FAILD]" );

		return found_valid;
	}

void clearEnvironment_tabs( tabtype **tabs )
	{
		systemlog( 3, "clearing tabs..\n");

		while( (*tabs)->next != NULL )
			*tabs = (*tabs)->next;
		while( (*tabs)->prev != NULL )
			{
				*tabs = (*tabs)->prev;
				free( (*tabs)->next );
				(*tabs)->next = NULL;
			}

		free( *tabs );
		*tabs = NULL;

		systemlog( 3, "tabs cleard\n");
	}

void clearEnvironment_actions( filetypeAction **actions )
	{
		systemlog( 3, "clearing actions..\n");

		while( (*actions)->next != NULL )
				*actions = (*actions)->next;

		while( (*actions)->prev != NULL )
			{
				*actions = (*actions)->prev;
				free( (*actions)->next );
				(*actions)->next = NULL;
			}

		free( *actions );
		*actions = NULL;

		systemlog( 3, "actions cleard\n");
	}

void clearEnvironment_shortcuts( shortcutType **shortCuts )
	{
		systemlog( 3, "clearing shortcuts..\n");

		while( (*shortCuts)->next != NULL )
			*shortCuts = (*shortCuts)->next;

		while( (*shortCuts)->prev != NULL )
			{
				*shortCuts = (*shortCuts)->prev;
				free( (*shortCuts)->next );
				(*shortCuts)->next = NULL;
			}

		free( *shortCuts );
		*shortCuts = NULL;

		systemlog( 3, "shortcuts cleard\n");
	}

void clearEnvironmett_sounds( soundeffectType **sounds )
	{
		systemlog( 3, "clearing sounds..\n");

		while( (*sounds)->next != NULL )
			*sounds = (*sounds)->next;

		while( (*sounds)->prev != NULL )
			{
				*sounds = (*sounds)->prev;
				free( (*sounds)->next );
				(*sounds)->next = NULL;
			}
		
		free( *sounds );
		*sounds = NULL;

		systemlog( 3, "sounds cleard\n");
	}

