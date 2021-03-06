/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= command-window.c =--*/
#include "command-window.h"
#include "projecttypes.h"
#include "autocomplete.h"
#include "directorys.h"
#include "window.h"
#include "systemlog.h"
#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

cmdWindowtype *new_cmdwindow( int h, int w, int y, int x, char wd[] )
	{
		int i = 0;
		cmdWindowtype *cmdwin;

		systemlog( 3, "new_cmdwindow" );

		cmdwin = malloc( sizeof(cmdWindowtype) );

		cmdwin->win = newwin( h, w, y, x );
			cmdwin->h = h;
			cmdwin->w = w;
			cmdwin->x = x;
			cmdwin->y = y;
			cmdwin->hidden = 1;
			cmdwin->cruser = 0;
			
		while( i != strlen(wd) )
			{
				cmdwin->wd[i] = wd[i];
				i++;
			}
		cmdwin->wd[i] = '\0';

		for( i = 0; i <= CMDHISTORY+1; i++ )
			{
				cmdwin->currentcmd[i][0] = '\0';
			}

		cmdwin->ddir[0] = '\0';

		return cmdwin;
	}

int printcmdwin( cmdWindowtype *cw, Windowtype *awin, Windowtype *pwin )
	{
		int ch;
		int x = 0;
		int EXIT = 0;
		int cc = 1;

		systemlog( 3, "printcmdwin" );

		if( cw->hidden )
			{
				clearcmd( cw );
			}
		else
			{ 
				wattron( cw->win, COLOR_PAIR(11) );
				box( cw->win, 0, 0 );

				//topp of border, workdirectory of activ window
				wattron( cw->win, COLOR_PAIR(12) );
				mvwprintw( cw->win, 0,+2,"Current($d1): %s", cw->wd );

				//bottom of border, destinationdir, workdirectory of passiv window
				mvwprintw( cw->win, 2,2,"Destination($d2): %s", cw->ddir );

				wattron( cw->win, COLOR_PAIR(1) );

				//print a ':' at the start of the line
				mvwprintw( cw->win, 1, 1, ":" );
			}

		wrefresh( cw->win );
		keypad( cw->win, TRUE );

		/*get commands*/
		while( !EXIT )
			{
				//printing (updateing)
				for( x = 0; x < cw->w-3; x++ )
					{
						if( x == cw->cruser )
							wattron( cw->win, A_UNDERLINE );
						else
							wattroff( cw->win, A_UNDERLINE );

						if( x < strlen(cw->currentcmd[0]) )
							mvwprintw( cw->win, 1, x+2, "%c", cw->currentcmd[0][x] );
						else
							mvwprintw( cw->win, 1, x+2, " " );
					}
				wrefresh( cw->win );

				//detect keystroks
				ch = wgetch( cw->win );
				switch ( ch )
					{
						case 0x7f: //backspace
							if( cw->cruser > 0 )
								{
									if( strlen(cw->currentcmd[0]) > cw->cruser )
										{
											strcpy( cw->currentcmd[0]+cw->cruser-1, cw->currentcmd[0]+cw->cruser );
										}
									else
										{
											mvwprintw(cw->win, 1, cw->cruser+2, " ");
											cw->currentcmd[0][cw->cruser-1] = '\0';
											if( cc == 1 )
												cw->currentcmd[1][cw->cruser-1] = '\0';
										}
									cw->cruser--;
								}
							break;

						case KEY_UP:
							if( cc < CMDHISTORY )
								cc++;

							clearcmd( cw );

							for( x = 0; x < strlen(cw->currentcmd[cc]); x++ )
								{
									cw->currentcmd[0][x] = cw->currentcmd[cc][x];
								}
							cw->cruser = x;
							break;

						case KEY_DOWN:
							if( cc > 0 )
								cc--;

							clearcmd( cw );

							for( x = 0; x < strlen(cw->currentcmd[cc]); x++ )
								{
									cw->currentcmd[0][x] = cw->currentcmd[cc][x];
								}
							cw->cruser = x;
							break;

						case KEY_LEFT:
							if( cw->cruser > 0 )
								cw->cruser--;
							break;
						case KEY_RIGHT:
							if( cw->cruser <= strlen(cw->currentcmd[0] +1 ) )
								cw->cruser++;
							break;

						case 0xA: //Enter
							cw->currentcmd[0][strlen( cw->currentcmd[0] )] = '\0';
							cw->currentcmd[1][strlen( cw->currentcmd[1] )] = '\0';
							trowcmd( cw, awin, pwin );
							clearcmd( cw );
							loadnewdir( awin, awin->wd );
							EXIT = 1;
							break;

						case 0x1B: //Escape
							EXIT = 1; 
							break;

						default: 		
							if( strlen(cw->currentcmd[0]) > cw->cruser )
								{
									strcpy( cw->currentcmd[0]+cw->cruser, cw->currentcmd[0]+cw->cruser-1 );
									strcpy( cw->currentcmd[1]+cw->cruser, cw->currentcmd[1]+cw->cruser-1 );
									cw->currentcmd[0][cw->cruser] = ch;
									cw->currentcmd[1][cw->cruser] = ch;
								}
							else
								{
									cw->currentcmd[0][cw->cruser] = ch;
									cw->currentcmd[0][cw->cruser+1] = '\0';
									cw->currentcmd[1][cw->cruser] = ch;
									cw->currentcmd[1][cw->cruser+1] = '\0';
								}

							cw->cruser++;
							break;
					}
			}

		if( EXIT )
			{
				togglehide( cw );
			}

		clearcmd( cw );

		return 1;
	}

void clearcmd( cmdWindowtype *cw )
	{
		int x,y;

		systemlog( 4, "clearcmd" );

		if( cw->hidden )
			{//wipe all including decorations
				for( y = 0; y < cw->h; y++ ){
					for( x = 0; x < cw->w; x++ ){
	
						mvwprintw( cw->win, y, x, " " );

					}
				}
			}
		else
			{//just wipe the window content
				cw->cruser = 0;
				memset( cw->currentcmd[0], 0, sizeof cw->currentcmd[0] );
				for( x = 2; x < cw->w -1; x++ )
					{
						mvwprintw( cw->win, 1, x, " " );
					}
			}
	}

void putnewdir( cmdWindowtype *cw, char wd[], char ddir[] )
	{
		int i = 0;

		systemlog( 4, "putnewdir" );

		//update current directory (activwin)
		while( i != strlen(wd) )
			{
				cw->wd[i] = wd[i];
				i++;
			}
		cw->wd[i] = '\0';

		i= 0;

		//update destination directory (passivwin)
		while( i != strlen(ddir) )
			{
				cw->ddir[i] = ddir[i];
				i++;
			}
		cw->ddir[i] = '\0';
	}

int togglehide( cmdWindowtype *cw )
	{
		systemlog( 4, "togglehide" );

		if( cw->hidden )
			{
				cw->hidden = 0;
			}
		else
			{
				cw->hidden = 1;
			}

		return cw->hidden;
	}

int trowcmd( cmdWindowtype *cw, Windowtype *awin, Windowtype *pwin )
	{
		int x = 0;
		int selections = 0;
		char cmd[COMMANDLENGTH];
		char tmpcmd[COMMANDLENGTH];
		int cc = CMDHISTORY; /*number of commands in memmory*/
		cmd[0] = '\0';
		tmpcmd[0] = '\0';

		systemlog( 3, "trowcmd" );
		
		//store commands in history
		while( cc != 1 )
			{
				//always leave a empty command at 0, current at 1, memory at 2
				if( cc == 2 )
					{
						strcpy( cw->currentcmd[cc], cw->currentcmd[0] );
					}
				else
					{
						strcpy( cw->currentcmd[cc], cw->currentcmd[cc-1] );
					}

				cc--;
			}

		chdir( cw->wd );
		
		find_and_add_dir( cmd, cw->currentcmd[0], awin->wd, pwin->wd );
		strcat( cmd, CMD_SILENCE );

		//trow the command for eatch selected file
		for( x = 1; x <= printtotalnr( awin->filelist ); x++ )
      		{
				awin->filelist = gotoEntry( awin->filelist, x );
				if( awin->filelist->selected && find_and_add_fp( tmpcmd, cmd, awin->filelist->file->d_name ) )
					{
						selections++;
						awin->filelist->selected = 0;
						system( tmpcmd );
							
					}
				memset( tmpcmd, 0, sizeof(tmpcmd) );
		}

		//raise quit signal if :q (vi like quit)
		if( strlen( cmd ) == 1+ strlen( CMD_SILENCE ) && cmd[0] == 'q' )
			{
				raise( SIGTERM );
				selections++;
			}

		//just a regular trow
		if( !selections )
			system( cmd );

		memset( cw->currentcmd[1], 0, sizeof cw->currentcmd[1] );

		return 0;
	}

void destroycmdwin( cmdWindowtype *cw )
	{
		systemlog( 2, "destroying command window" );
		cw->hidden = 1;
		clearcmd( cw );
		delwin( cw->win );
			free( cw );
		systemlog( 2, "command window destroyd");
	}
