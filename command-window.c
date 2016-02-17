/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= command-window.c =--*/
#include "command-window.h"
#include "projecttypes.h"
#include "autocomplete.h"
#include "directorys.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

cmdWindowtype *new_cmdwindow( int h, int w, int y, int x, char wd[] )
	{
			int i = 0;
			cmdWindowtype *cmdwin;

			cmdwin = malloc( sizeof(cmdWindowtype) );

			cmdwin->win = newwin( h, w, y, x );
				cmdwin->h = h;
				cmdwin->w = w;
				cmdwin->x = x;
				cmdwin->y = y;
				cmdwin->hiden = 1;
				cmdwin->cruser = 0;
			
			while( i != strlen(wd) )
        {
          cmdwin->wd[i] = wd[i];
          i++;
        }
        cmdwin->wd[i] = '\0';

			return cmdwin;
	}

void printcmdwin( cmdWindowtype *cw, Windowtype *awin, Windowtype *pwin )
	{
		int ch;
		int x = 0;
		int EXIT = 0;
		int cc = 1;

		if( cw->hiden )
			{
				clearcmd( cw );
			}
		else
			{ /*dekorations*/
				box( cw->win, 0, 0 );
				for( x = 0; cw->wd[x] != '\0' && x+2 < cw->w -2; x++ )
          {
            mvwprintw( cw->win, 0,x+2,"%c", cw->wd[x] );
          }
				for( x = 0; cw->ddir[x] != '\0' && x+2 < cw->w -2; x++ )
					{
						mvwprintw( cw->win, 2,x+2,"%c", cw->wd[x] );
					}
				mvwprintw( cw->win, 1, 1, ":_" );

				for( x = 0; x < cw->cruser; x++ )
					{
						mvwprintw( cw->win, 1,x+2,"%c_", cw->currentcmd[0][x] );
					}
			}

		wrefresh( cw->win );
		keypad( cw->win, TRUE );

		/*get commands*/
		while( !EXIT && (ch = wgetch(cw->win)) )
			{ switch ( ch )
					{
						case 0x7f 	: if( cw->cruser > 0 )
					/*backspace*/			{
															mvwprintw(cw->win, 1, 1+cw->cruser, "_ ");
															cw->cruser--;
															cw->currentcmd[0][cw->cruser] = ' ';
															if( cc == 1 )
																cw->currentcmd[1][cw->cruser] = ' ';
														}
													break;

						case KEY_UP: 	if( cc < CMDHISTORY )
					/*up*/						cc++;
													
													clearcmd( cw );

													for( x = 0; x < strlen(cw->currentcmd[cc]); x++ )
														{
															mvwprintw( cw->win, 1,x+2,"%c_", cw->currentcmd[cc][x] );
															cw->currentcmd[0][x] = cw->currentcmd[cc][x];
														}
													cw->cruser = x;
													wrefresh( cw->win );
													break;

						case KEY_DOWN:  if( cc > 0 )
           /*down*/        	 cc--;

													clearcmd( cw );

                          for( x = 0; x < strlen(cw->currentcmd[cc]); x++ )
                            {
                              mvwprintw( cw->win, 1,x+2,"%c_", cw->currentcmd[cc][x] );
                              cw->currentcmd[0][x] = cw->currentcmd[cc][x];
                            }
                          cw->cruser = x;
                          wrefresh( cw->win );
                          break;

						case 0xA 		:	trowcmd( cw, awin, pwin );
					/*Enter*/				clearcmd( cw );
													EXIT = 1;
													break;

						case 0x1B 	:  	EXIT = 1; 
					/*Escape*/			break;

						default 		: 	mvwprintw( cw->win, 1, 2+cw->cruser, "%c_", ch );
													cw->currentcmd[0][cw->cruser] = ch;
													cw->currentcmd[1][cw->cruser] = ch;
													cw->cruser++;
													wrefresh( cw->win );
													break;
					}
			}
			
		if( EXIT )
			{
				togglehide( cw );
			}
		
			clearcmd( cw );
	}

void clearcmd( cmdWindowtype *cw )
	{
		int x,
				y;
		if( cw->hiden )
			{
				for( y = 0; y < cw->h; y++ ){
					for( x = 0; x < cw->w; x++ ){
	
						mvwprintw( cw->win, y, x, " " );

					}
				}
			}
		else
			{
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
		while( i != strlen(wd) )
       {
         cw->wd[i] = wd[i];
         i++;
       }
       cw->wd[i] = '\0';

		i= 0;

		while( i != strlen(ddir) )
			{
				cw->ddir[i] = ddir[i];
				i++;
			}
			cw->ddir[i] = '\0';
	}

int togglehide( cmdWindowtype *cw )
	{
		if( cw->hiden )
			{
				cw->hiden = 0;
			}
		else
			{
				cw->hiden = 1;
			}

		return cw->hiden;
	}

int trowcmd( cmdWindowtype *cw, Windowtype *awin, Windowtype *pwin )
	{
		int x = 0;
		int selections = 0;
		char tmpcmd[COMMANDLENGTH];
		int cc = CMDHISTORY; /*number of commands in memmory*/
		tmpcmd[0] = '\0';
		
		while( cc != 1 )
			{/*remember*/
				if(cc == 2 )
					{/*always leve a empty command at 0 => current at 1, memory at 2*/
						strcpy( cw->currentcmd[cc], cw->currentcmd[0] );
					}
				else
					{
						strcpy( cw->currentcmd[cc], cw->currentcmd[cc-1] );
					}

				cc--;
			}

		chdir( cw->wd );

		for( x = 0; x <= printtotalnr( awin->filelist ); x++ )
      {
				awin->filelist = gotoEntry( awin->filelist, x );
				if( awin->filelist->selected && find_and_add_fp( tmpcmd, cw->currentcmd[0], awin->filelist->file->d_name, pwin->wd ) )
					{ 
						selections++;
						system( tmpcmd );
					}
				//memset( tmpcmd, 0, sizeof(tmpcmd) );
			}

		if( !selections )
			system( cw->currentcmd[0] );

		memset( cw->currentcmd[1], 0, sizeof cw->currentcmd[1] );

		return 0;
	}

void destroycmdwin( cmdWindowtype *cw )
	{
		cw->hiden = 1;
		clearcmd( cw );
		delwin( cw->win );
			free( cw );
	}
