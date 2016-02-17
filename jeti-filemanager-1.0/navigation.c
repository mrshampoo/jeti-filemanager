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

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

int scrollupp( Windowtype *win, soundeffectType *sounds )
	{
		if( win->slide[win->mlevel] > 0 )
    	{
      	win->slide[win->mlevel]--;
		playsound( sounds, 5 );
        return 1;
      }
		else
			{
				return 0;
			}
	}

int scrolldown( Windowtype *win, soundeffectType *sounds )
	{
		if( win->slide[win->mlevel] + win->h -4 < printtotalnr( win->filelist ) )
    	{
      	win->slide[win->mlevel]++;
		playsound( sounds, 6 );
        return 1;
      }
		else
			{
				return 0;
			}
	}

int stepupp( Windowtype *win, soundeffectType *sounds )
	{
    if( win->marker[win->mlevel]-1 <= win->slide[win->mlevel] && win->slide[win->mlevel] > 0 )
    	{
				win->marker[win->mlevel]--;
      	win->slide[win->mlevel]--;
		playsound( sounds, 7 );
        return 1;
      }
		else if( win->marker[win->mlevel]-1 > 0 && win->marker[win->mlevel] > win->slide[win->mlevel] )
      {
        win->marker[win->mlevel]--;
		playsound( sounds, 7 );
        return 1;
      }
		else
			{
				return 0;
			}
	}

int stepdown( Windowtype *win, soundeffectType *sounds )
	{
		if( win->marker[win->mlevel] != printtotalnr( win->filelist ) && win->marker[win->mlevel]-1 < win->h + win->slide[win->mlevel] -4 )
    	{
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
				return 0;
			}
	}

int selectfile( Windowtype *win, soundeffectType *sounds )
	{
		win->filelist = gotoEntry( win->filelist, win->marker[win->mlevel] );
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
		int ret;

		char dubbledot[] = "..";
		char tmpcmd[250];
		char cmd[250];

		cmd[0] = '\0';

		win->filelist = gotoEntry( win->filelist, win->marker[win->mlevel] );

		if( isoffiletype( win->filelist, win->marker[win->mlevel], "<KAT>" ) )
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
				ret = 1;
			}
		else
			{
				while( fileaction->next != NULL ) /*rewind fileactions*/
					{	fileaction = fileaction->next;	}
				
				while( !isoffiletype( win->filelist, win->marker[win->mlevel], fileaction->filetype ) && fileaction->prev != NULL )
					{ fileaction = fileaction->prev; }
					strcpy( cmd, fileaction->Action );

				if( S_IEXEC & win->filelist->status && win->noexe == 0 )
          			{ strcpy( cmd, "lxterminal -e " ); playsound( sounds, 4 ); }
				else
					playsound( sounds, 0 );

				find_and_add_filename( cmd, gotoEntry( win->filelist, win->marker[win->mlevel] )->file->d_name );

				if( cmd[0] != '\0' )
					{
						strcpy( tmpcmd, win->wd );
        		if( win->wd[strlen(win->wd)-1] != '/' )
          		{
            		strcat( tmpcmd, "/" );
          		}
        		strcat( tmpcmd, gotoEntry( win->filelist, win->marker[win->mlevel] )->file->d_name );

						addslash( cmd, tmpcmd );

						strcat( cmd, " > /dev/null 2>&1" );
						win->marker[win->mlevel] = -1;
        		system( cmd );

					}

        ret = 1;

			}

		return ret;

	}

int handleshortcut( Windowtype *awin ,Windowtype *pwin, soundeffectType *sounds )
	{
		const char *homedir;
		char dir[SIZE_WORKDIREKTORY];

		if( !strncmp( awin->shortcuts->dir, "$passiv", 7 ) )
			{
				strcpy( dir, pwin->wd );
			}
		else if( !strncmp( awin->shortcuts->dir, "$home", 5 ) )
			{
				if((homedir = getenv("HOME")) == NULL )
					{
						homedir = getpwuid( getuid() )->pw_dir;
					}
				strcpy( dir, homedir );
			}
		else if( !strncmp( awin->shortcuts->dir, "$hidden", 7 ) )
			{
				if( awin->showhidden )
					awin->showhidden = 0;
				else
					awin->showhidden = 1;

				strcpy( dir, awin->wd );
			}
		else if( !strncmp( awin->shortcuts->dir, "$noexe", 5 ) )
			{
				if( awin->noexe )
                    awin->noexe = 0;
                else
                    awin->noexe = 1;

                strcpy( dir, awin->wd );
			}
		else if( !strncmp( awin->shortcuts->dir, "$mute", 5 ) )
			{
                toggle_mute();

                strcpy( dir, awin->wd );
            }
		else if( !strncmp( awin->shortcuts->dir, "_", 1 ) )
			{
				//do nothing: this is the noshortcuts-shortcut
			}
		else
			{
				strcpy( dir, awin->shortcuts->dir );
			}

		awin->mlevel = 0;    
        awin->marker[awin->mlevel] = -1;
        awin->slide[awin->mlevel] = 0;

		playsound( sounds, 13 );
		loadnewdir( awin, dir );
		return 1;
	}

int copyfiles( Windowtype *awin, Windowtype *pwin, soundeffectType *sounds )
	{
		int x = 0;
		int repaint = 0;
		char cmd[COMMANDLENGTH];

		chdir( awin->wd );

		for( x = 0; x <= printtotalnr( awin->filelist ); x++ )
			{
				awin->filelist = gotoEntry( awin->filelist, x );
				if( awin->filelist->selected )
					{
						strcpy( cmd, "cp ");
						if( isoffiletype( awin->filelist, awin->filelist->number, "<KAT>" ) )
								strcat( cmd, "-r " );

						addslash( cmd, awin->filelist->file->d_name );
						strcat( cmd, " " );
						strcat( cmd, pwin->wd );

						system( cmd );
						
						awin->filelist->selected = 0;
						repaint++;
					}
			}

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
    int repaint = 0;
    char cmd[COMMANDLENGTH];

    chdir( awin->wd );

    for( x = 0; x <= printtotalnr( awin->filelist ); x++ )
      {
        awin->filelist = gotoEntry( awin->filelist, x );
        if( awin->filelist->selected )
          {
            strcpy( cmd, "mv ");

            addslash( cmd, awin->filelist->file->d_name );
            strcat( cmd, " " );
            strcat( cmd, pwin->wd );

            system( cmd );

            awin->filelist->selected = 0;
            repaint++;
          }
      }

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
    int repaint = 0;
    char cmd[COMMANDLENGTH];

    chdir( awin->wd );

    for( x = 0; x <= printtotalnr( awin->filelist ); x++ )
      {
        awin->filelist = gotoEntry( awin->filelist, x );
        if( awin->filelist->selected )
          {
            strcpy( cmd, "rm ");
            if( isoffiletype( awin->filelist, awin->filelist->number, "<KAT>" ) )
                strcat( cmd, "-r " );

            addslash( cmd, awin->filelist->file->d_name );

            system( cmd );

            awin->filelist->selected = 0;
            repaint++;
          }
      }

		if( repaint )
		  {
			playsound( sounds, 12 );
			loadnewdir( awin, awin->wd );
		  }

    return repaint;
	}

