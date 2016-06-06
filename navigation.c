/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*-- navigation.c -----*/
#include "navigation.h"
#include "projecttypes.h"
#include "handleflags.h"
#include "window.h"
#include "environment.h"
#include "directorys.h"
#include "autocomplete.h"
#include "soundeffects.h"
#include "systemlog.h"
#include "dialog-window.h"

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

static dialogwindowtype *dialogwin;

void init_navigation()
	{
		systemlog( 2, "init_navigation");

		dialogwin = new_dialogwindow( 8, 29, (LINES/2)-4, (COLS/2)-14 );	
	}
void redefine_navigation()
	{
		systemlog( 2, "redefine_navigation" );

		destroy_dialogwin( dialogwin );
		dialogwin = new_dialogwindow( 8, 29, (LINES/2)-4, (COLS/2)-14 );
		clear_dialogwin( dialogwin );
	}
void destroy_navigation()
	{
		systemlog( 2, "destroy navigation" );

		destroy_dialogwin( dialogwin );
	}

int scrollupp( Windowtype *win, soundeffectType *sounds )
	{
		systemlog( 3, "scrollupp" );

		if( win->slide[win->mlevel] > 0 )
			{
				win->slide[win->mlevel]--;
				playsound( sounds, 5 );
				return 1;
			}
		else
			{
				//slide reached the top
				return 0;
			}
	}

int scrolldown( Windowtype *win, soundeffectType *sounds )
	{
		systemlog( 3, "scrolldown" );

		if( win->slide[win->mlevel] + win->h -4 < printtotalnr( win->filelist ) )
			{
				win->slide[win->mlevel]++;
				playsound( sounds, 6 );
				return 1;
			}
		else
			{
				//slide reached bottom
				return 0;
			}
	}

int stepupp( Windowtype *win, soundeffectType *sounds )
	{
		systemlog( 3, "stepupp" );

		if( win->marker[win->mlevel]-1 <= win->slide[win->mlevel] && win->slide[win->mlevel] > 0 )
			{
				win->marker[win->mlevel]--;
				win->slide[win->mlevel]--;
				playsound( sounds, 7 );
				return 1;
			}
		else if( win->marker[win->mlevel]-1 > 0 && win->marker[win->mlevel] > win->slide[win->mlevel] )
			{
				//slide is at the top
				win->marker[win->mlevel]--;
				playsound( sounds, 7 );
				return 1;
			}
		else
			{
				//marker reached the top
				return 0;
			}
	}

int stepdown( Windowtype *win, soundeffectType *sounds )
	{
		systemlog( 3, "stepdown" );

		if( win->marker[win->mlevel] != printtotalnr( win->filelist ) && win->marker[win->mlevel]-1 < win->h + win->slide[win->mlevel] -4 )
			{
				//slide reached the bottom
				win->marker[win->mlevel]++;
				playsound( sounds, 8 );
				return 1;
			}
		else if( win->marker[win->mlevel] != printtotalnr( win->filelist ) && win->marker[win->mlevel]-1 >= win->h + win->slide[win->mlevel] -4 )
			{
	
				win->marker[win->mlevel]++;
				win->slide[win->mlevel]++;
				playsound( sounds, 8 );
				return 1;
			}
		else
			{
				//marker reatched bottom
				return 0;
			}
	}

int selectfile( Windowtype *win, soundeffectType *sounds )
	{
		systemlog( 3, "selectfile" );

		win->filelist = gotoEntry( win->filelist, win->marker[win->mlevel] );

		//toggle select
		if( win->filelist->selected == 0 )
			{
				win->filelist->selected = 1;
			}
		else if( win->filelist->selected == 1 )
			{
				win->filelist->selected = 0;
			}
		else
			{
				return 0;
			}
		playsound( sounds, 9 );
		return 1;
	}

int enter( Windowtype *win, filetypeAction *fileaction, soundeffectType *sounds )
	{
		int i = 0;
		int repaint = 0;
		int notexec = 0;

		char dubbledot[] = "..";
		char tmpcmd[250];
		char cmd[250];

		cmd[0] = '\0';

		systemlog( 2, "enter" );

		win->filelist = gotoEntry( win->filelist, win->marker[win->mlevel] );

		if( isoffiletype( win->filelist, win->marker[win->mlevel], "<DIR>" ) || isoffiletype( win->filelist, win->marker[win->mlevel], "<DIR L>" ) || isoffiletype( win->filelist, win->marker[win->mlevel], "<DIR LE>") )
			{
  				while( win->filelist->file->d_name[i] == dubbledot[i] && i < 2 )
   	 				{/*check for dubbledot*/ i++; }

    			if( i == 2 )
					{ /*dubbledot was found, go back one folder*/
						if( strlen( win->wd ) != 1 )
							win->wd[strlen(win->wd)-1] = '\0';

						while( win->wd[strlen(win->wd)-1] != '/')
							{
								win->wd[strlen(win->wd)-1] = '\0';
							}

						i = 0;

						if( win->mlevel > 0 )
							{
								win->mlevel--;
							}
						else
							{
								win->marker[win->mlevel] = -1;
							}

						strcpy( cmd, win->wd );
						loadnewdir( win, cmd );
						playsound( sounds, 1 );
					}
				else if( i == 1 && strlen(win->filelist->file->d_name) == 1 )
					{/*singledot*/
						win->marker[win->mlevel] = -1;
						strcpy( cmd, win->wd );
						loadnewdir( win, cmd );
						playsound( sounds, 2 );
					}
				else
    				{/*regular folder*/
						strcpy( cmd, win->wd );
						if( win->wd[strlen(win->wd)-1] != '/')
							strcat( cmd, "/");

						strcat( cmd, win->filelist->file->d_name );
						strcat( cmd, "/");

						if( loadnewdir( win, cmd ) )
							{
								win->mlevel++;
								win->marker[win->mlevel] = -1;
								playsound( sounds, 3 );
							}
      				}
				repaint = 1;
			}
		else
			{/*regular file*/

				//rewind fileactions
				while( fileaction->next != NULL )
					{	fileaction = fileaction->next;	}

				//copy fileactions				
				while( !isoffiletype( win->filelist, win->marker[win->mlevel], fileaction->filetype ) && fileaction->prev != NULL )
					{ fileaction = fileaction->prev; }
					strcpy( cmd, fileaction->Action );

				//check executable or if no atribute was found
				if( S_IEXEC & win->filelist->status && win->noexe == 0 )
					{
						strcpy( cmd, TERMINALNAME );
						strcat( cmd, " -e " );
					}
				else if( !isoffiletype( win->filelist, win->marker[win->mlevel], fileaction->filetype ) )
					{
						memset( cmd, 0, sizeof(cmd));
						cmd[0] = '\0';
						notexec = 1;
					}
				else
					{
						notexec = 1;
					}

				find_and_add_fp( cmd, cmd, gotoEntry( win->filelist, win->marker[win->mlevel] )->file->d_name );

				if( cmd[0] != '\0' )
					{
						//add path
						strcpy( tmpcmd, win->wd );
        				if( win->wd[strlen(win->wd)-1] != '/' )
          					{
            						strcat( tmpcmd, "/" );
          					}
        				strcat( tmpcmd, gotoEntry( win->filelist, win->marker[win->mlevel] )->file->d_name );

						addslash( cmd, tmpcmd );

						//make silent and free
						strcat( cmd, " > /dev/null 2>&1 &" );
						win->marker[win->mlevel] = -1;

						if( notexec )
							playsound( sounds, 0 );
						else
							playsound( sounds, 4 );

        				system( cmd );
						repaint = 1;

					}

			}

		return repaint;

	}

int handleshortcut( Windowtype *awin ,Windowtype *pwin, soundeffectType *sounds )
	{
		int d = 0;
		int x = 0;
		int selected = 0;
		const char *homedir;
		char cmd[256];
		char tcmd[256];
		char dir[SIZE_WORKDIREKTORY];

		systemlog( 2, "handleshortcut");

		if( !strncmp( awin->shortcuts->dir, "$passiv", 7 ) )
			{
				systemlog( 4, "$passiv");
				strcpy( dir, pwin->wd );
			}
		else if( !strncmp( awin->shortcuts->dir, "$home", 5 ) )
			{
				systemlog( 4, "$home" );
				if((homedir = getenv("HOME")) == NULL )
					{
						homedir = getpwuid( getuid() )->pw_dir;
					}
				strcpy( dir, homedir );
			}
		else if( !strncmp( awin->shortcuts->dir, "$hidden", 7 ) )
			{
				systemlog( 4, "$hidden" );
				if( awin->showhidden )
					awin->showhidden = 0;
				else
					awin->showhidden = 1;

				strcpy( dir, awin->wd );
			}
		else if( !strncmp( awin->shortcuts->dir, "$noexe", 5 ) )
			{
				systemlog( 4, "$noexe");
				if( awin->noexe )
					awin->noexe = 0;
				else
					awin->noexe = 1;

				strcpy( dir, awin->wd );
			}
		else if( !strncmp( awin->shortcuts->dir, "$mute", 5 ) )
			{
				systemlog( 4, "$mute" );
                		toggle_mute();

                		strcpy( dir, awin->wd );
            		}
		else if( !strncmp( awin->shortcuts->dir, "$exec", 5 ) )
			{d+=6;
				systemlog( 4, "$exec" );
				systemlog( 95, awin->shortcuts->dir +d );
				systemlog( 95, awin->wd );

				find_and_add_dir( cmd, awin->shortcuts->dir +d, awin->wd );
				strcat( cmd ," > /dev/null 2>&1 &" );

				for( x = 1; x <= printtotalnr( awin->filelist ); x++ )
					{
						awin->filelist = gotoEntry( awin->filelist, x );
						if( awin->filelist->selected && find_and_add_fp( tcmd, cmd, awin->filelist->file->d_name ) )
							{
								if( !selected )
									chdir( awin->wd );

								selected++;
								awin->filelist->selected = 0;

								systemlog( 5, "file#");
								systemlog( 95, tcmd );
								system( tcmd );
							}
					}
				if( !selected )
					{
						systemlog( 5, "simple#");
						systemlog( 95, cmd );
						system( cmd );
					}

				strcpy( dir, awin->wd );
			}
		else if( !strncmp( awin->shortcuts->dir, "_", 1 ) )
			{
				systemlog( 4, "noshortcut-shortcut");
				//do nothing: this is the noshortcuts-shortcut
			}
		else
			{
				systemlog( 4,"goto dir:" );
				systemlog( 94, awin->shortcuts->dir );
				strcpy( dir, awin->shortcuts->dir );
			}

		awin->mlevel = 0;    
		awin->marker[awin->mlevel] = -1;
		//awin->slide[awin->mlevel] = 0;

		playsound( sounds, 13 );
		loadnewdir( awin, dir );
		return 1;
	}

int copyfiles( Windowtype *awin, Windowtype *pwin, soundeffectType *sounds )
	{
		int x = 0;
		int repaint = 1;
		int quote = 0;
		int pass = 0;
		char cmd[COMMANDLENGTH];
		char passwd[PASSWD_LENGTH];

		systemlog( 2, "copyfiles" );

		chdir( awin->wd );

		keypad( awin->win, FALSE );
		if( ( pass = print_dialogwin( dialogwin, awin, pwin->wd, 2, "Are you shure you want to copy these?", passwd ) ) )
			{
				for( x = 1; x <= printtotalnr( awin->filelist ); x++ )
					{
						awin->filelist = gotoEntry( awin->filelist, x );
						if( awin->filelist->selected )
							{
								if( pass == 2 && access( pwin->wd, W_OK ) && (DW_REACTION & 8) )
									{
										strcpy( cmd, "echo " );
										strcat( cmd, passwd );
										strcat( cmd, " | su -c \"cp ");
										quote = 1;
									}
								else if( access( pwin->wd, W_OK ) && (DW_REACTION & 4) )
									{
										strcpy( cmd, "sudo cp " );
									}
								else
									{
										strcpy( cmd, "cp ");
									}

								if( isoffiletype( awin->filelist, awin->filelist->number, "<DIR>" ) )
									strcat( cmd, "-r " );

								addslash( cmd, awin->filelist->file->d_name );
								strcat( cmd, " " );

								addslash( cmd, pwin->wd );

								if( quote )
									{
										strcat( cmd, "\"" );
										quote = 0;
									}

								strcat( cmd, " >/dev/null 2>/dev/null" );

								system( cmd );
						
								awin->filelist->selected = 0;
								repaint++;
							}
					}
			}

		keypad( awin->win, TRUE );

		if( repaint )
			{
				loadnewdir( awin, awin->wd );
				loadnewdir( pwin, pwin->wd );
				playsound( sounds, 10 );
				repaint++;
			}

		return repaint;
	}

int movefiles( Windowtype *awin, Windowtype *pwin, soundeffectType *sounds )
	{
		int x = 0;
		int repaint = 1;
		int quote = 0;
		int pass = 0;
		char cmd[COMMANDLENGTH];
		char passwd[PASSWD_LENGTH];

		systemlog( 2, "movefiles" );

		chdir( awin->wd );

		keypad( awin->win, FALSE );
		if( ( pass = print_dialogwin( dialogwin, awin, pwin->wd, 3, "Are you shure you want to move these?", passwd ) ) )
			{
				for( x = 1; x <= printtotalnr( awin->filelist ); x++ )
					{
						awin->filelist = gotoEntry( awin->filelist, x );
						if( awin->filelist->selected )
							{
								if( pass == 2 && ( access( awin->filelist->file->d_name, W_OK ) || access( awin->filelist->file->d_name, W_OK )) && (DW_REACTION & 8) )
									{
										strcpy( cmd, "echo " );
										strcat( cmd, passwd );
										strcat( cmd, " | su -c \"mv " );
										quote = 1;
									}
								else if( ( access( awin->filelist->file->d_name, W_OK ) || access( pwin->wd, W_OK ) ) && (DW_REACTION & 4) )
									{
										strcpy( cmd, "sudo mv " );
									}
								else
									{
										strcpy( cmd, "mv ");
									}

								addslash( cmd, awin->filelist->file->d_name );
								strcat( cmd, " " );
								addslash( cmd, pwin->wd );

								if( quote )
									{
										strcat( cmd, "\"" );
										quote = 0;
									}

								strcat( cmd, " >/dev/null 2>/dev/null" );

								system( cmd );

								awin->filelist->selected = 0;
								repaint++;
							}
					}
			}

		keypad( awin->win, TRUE );

		if( repaint )
			{
				loadnewdir( awin, awin->wd );
				loadnewdir( pwin, pwin->wd );
				playsound( sounds, 11 );
				repaint++;
			}

		return repaint;
	}

int removefiles( Windowtype *awin, soundeffectType *sounds )
	{
		int x = 0;
		int repaint = 1;
		int quote = 0;
		int pass = 0;
		char cmd[COMMANDLENGTH];
		char passwd[PASSWD_LENGTH];

		systemlog( 2, "removefiles" );

		chdir( awin->wd );

		keypad( awin->win, FALSE );
		if( ( pass = print_dialogwin( dialogwin, awin, awin->wd, 1, "Are you shure you want to remove these?", passwd ) ) )
			{
				for( x = 1; x <= printtotalnr( awin->filelist ); x++ )
					{
						awin->filelist = gotoEntry( awin->filelist, x );
						if( awin->filelist->selected )
							{
								if( pass == 2 && access( awin->filelist->file->d_name, W_OK ) && (DW_REACTION & 8) )
									{
										strcpy( cmd, "echo " );
										strcat( cmd, passwd );
										strcat( cmd, " | su -c \"rm ");
										quote = 1;
									}
								else if( access( awin->filelist->file->d_name, W_OK ) && (DW_REACTION & 4) )
									{
										strcpy( cmd, "sudo rm " );
									}
								else
									{
										strcpy( cmd, "rm ");
									}

								if( isoffiletype( awin->filelist, awin->filelist->number, "<DIR>" ) )
									strcat( cmd, "-r " );

								addslash( cmd, awin->filelist->file->d_name );
							
								if( quote )
									{
										strcat( cmd, "\"" );
										quote = 0;
									}

								strcat( cmd, " >/dev/null 2>/dev/null" );

								system( cmd );

								awin->filelist->selected = 0;
								repaint++;
							}
					}
			}

		keypad( awin->win, TRUE );

		if( repaint )
			{
				playsound( sounds, 12 );
				loadnewdir( awin, awin->wd );
			}

		return repaint;
	}

