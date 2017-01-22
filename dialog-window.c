/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "projecttypes.h"
#include "dialog-window.h"
#include "directorys.h"
#include "systemlog.h"

dialogwindowtype *new_dialogwindow( int h, int w, int y, int x )
	{
		dialogwindowtype *dialogwin;

		dialogwin = malloc( sizeof(dialogwindowtype) );
			dialogwin->win = newwin( h, w, y, x );
			dialogwin->h = h;
			dialogwin->w = w;
			dialogwin->y = y;
			dialogwin->x = x;
			dialogwin->hidden = 1;

		if( h < 6 || w < 18 )
			systemlog( 1, "WARNING: dialogbox to smal for yes and no boxes" );

			dialogwin->yes = newwin( 3, 11, y+h-4, x+w/2-11-1 );
			dialogwin->no = newwin( 3, 11, y+h-4, x+w/2+1 );

		return dialogwin;
	}

int print_dialogwin( dialogwindowtype *dialogwin, Windowtype *awin, char destiwd[], int permisson_requierd, char question[], char repasswd[] )
	{
		int x,y;
		int confirmed = 0;
		int exit = 0;
		int ch;

		MEVENT event;

		systemlog( 2, "print_dialogwin");

		if( ( permisson_requierd & test_rw_permissiondenieds( awin, destiwd )) == 0 )
			{
				confirmed = 1;
			}

		if( ( (DW_REACTION & 1) && !confirmed ) || (DW_REACTION & 2) )
			{
				dialogwin->hidden = 0;
				wattron( dialogwin->win, COLOR_PAIR(1) );
				
				//decorations
				wattron( dialogwin->win, COLOR_PAIR( 11 ) );
				box( dialogwin->win, 0, 0 );
				wattron( dialogwin->win, COLOR_PAIR( 1 ) );

				//if( !confirmed && (DW_REACTION & 8 ) )
				//	{
				//		strcat( question, " This requires su or sudo" );
				//	}

				//question
				for( y = 1; y < dialogwin->h -1; y++ )
					{
						for( x = 1; x < dialogwin->w -1; x++ )
							{
								if( (x-2)+(y-1)*(dialogwin->w-2) <= strlen(question) && (x-2)+(y-1)*(dialogwin->w-2) >= 0 && x > 1 && x < dialogwin->w -2 && y <= dialogwin->h -6 && !( y == dialogwin->h - 6 &&  x >= dialogwin->w -5 ))
									{ //above yes and no boxes and inside frame
										mvwprintw( dialogwin->win, y, x, "%c", question[(x-2)+(y-1)*(dialogwin->w-3)] );
									}
								else if( y == dialogwin->h - 6 &&  x >= dialogwin->w -5 && x < dialogwin->w -2 && (x-2)+(y-1)*(dialogwin->w-2) <= strlen(question) )
									{
										mvwprintw( dialogwin->win, y, x, "." );
									}
								else
									{ //fill whole window
										mvwprintw( dialogwin->win, y, x, " " );
									}
							}
					}

				//when password is requierd
				if( !confirmed && (DW_REACTION & 8 ) )
					{
						wattron( dialogwin->win, COLOR_PAIR(9) );
						y = dialogwin->h -5;
						mvwprintw( dialogwin->win, y, 2, "sudo password:_" );
						x=0;
						wattron( dialogwin->win, COLOR_PAIR(9) );
					}

				wrefresh( dialogwin->win );

				//decorate yes and no boxes
			 	wattron( dialogwin->yes, COLOR_PAIR(11) );
				wattron( dialogwin->no, COLOR_PAIR(11) );
				box( dialogwin->yes, 0, 0 );
				box( dialogwin->no, 0, 0 );

				keypad( dialogwin->win, TRUE );
				while( !exit )
					{
						//colorisation
						if( confirmed )
							{
								wattron( dialogwin->yes, COLOR_PAIR(2) );
								wattron( dialogwin->no, COLOR_PAIR(1) );
							}
						else
							{
								wattron( dialogwin->yes, COLOR_PAIR(1) );
								wattron( dialogwin->no, COLOR_PAIR(2) );
							}

						//print text
						mvwprintw( dialogwin->yes, 1, 1, " Confirm " );
						mvwprintw( dialogwin->no, 1, 1, "  Cancel " );
						wrefresh( dialogwin->yes );
						wrefresh( dialogwin->no );

						//get user input
						ch = wgetch( dialogwin->win );

						//navigation system
						switch( ch )
							{
								//Enter
								case 0xA:
										dialogwin->hidden = 1;
										exit = 1;
										break;

								//Esc
								case 0x1B:
										confirmed = 0;
										dialogwin->hidden = 1;
										exit = 1;
										break;

								case KEY_RIGHT:
										confirmed = 0;
										break;

								case KEY_LEFT:
										if( x )
											confirmed = 2;
										else
											confirmed = 1;
										break;

								case KEY_MOUSE:
										if( getmouse( &event ) == OK )
											{
												if( event.bstate & BUTTON1_PRESSED )
													{
														if( insidedialogwin( dialogwin->yes, event.y, event.x ) )
															{
																if( x )
																	confirmed = 2;
																else
																	confirmed = 1;
															}
														else if( insidedialogwin( dialogwin->no, event.y, event.x ) )
															{
																confirmed = 0;
															}
														else if( insidedialogwin( dialogwin->win, event.y, event.x ) )
															{   
																//do nothing
															}
														else
															{
																confirmed = 0;
																dialogwin->hidden = 1;
																exit = 1;
															}
													}
												else if( event.bstate & BUTTON1_RELEASED )
													{
														if( insidedialogwin( dialogwin->yes, event.y, event.x ) )
															{
																if( confirmed )
																	{
																		dialogwin->hidden = 1;
																		exit = 1;
																	}
																else
																	{
																		if( x )
																			confirmed = 2;
																		else
																			confirmed = 1;
																	}
															}
														else if( insidedialogwin( dialogwin->no, event.y, event.x ) )
															{
																if( !confirmed )
																	{
																		dialogwin->hidden = 1;
																		exit = 1;
																	}
																else
																	{
																		confirmed = 0;
																	}
															}
														else if( insidedialogwin( dialogwin->win, event.y, event.x ) )
															{   
																//do nothing
															}
														else
															{
																//do nothing
															}

													}
											}
										break;

								//backspace
								case 0x7f:
										if( confirmed != 1 && x > 0 && (DW_REACTION & 8 ) && !(DW_REACTION & 32 ) )
											{
												repasswd[x] = '\0';
												x--;
												mvwprintw( dialogwin->win, y, x+16, "_ " );
												wrefresh( dialogwin->win );
											}
										else if( confirmed != 1 && x > 0 && (DW_REACTION & 8 ) && (DW_REACTION & 32) )
											{
												repasswd[x] = '\0';
												x--;
											}

										if( !x )
											confirmed = 0;
										
									break;

								default:
										if( confirmed != 1 && (DW_REACTION & 24 ) == 24 && !(DW_REACTION & 32) )
											{
												repasswd[x] = ch;
												mvwprintw( dialogwin->win, y, x+16, "*_" );
												wrefresh( dialogwin->win ); 
												x++;
											}
										else if( confirmed != 1 && (DW_REACTION & 8 ) && !(DW_REACTION & 32) )
											{
												repasswd[x] = ch;
												mvwprintw( dialogwin->win, y, x+16, "%c_", ch );
												wrefresh( dialogwin->win ); 
												x++;
											}
										else if( confirmed != 1 && (DW_REACTION & 8 ) && (DW_REACTION & 32) )
											{
												repasswd[x] = ch;
												x++;
											}

										if( x )
											confirmed = 2;

										break;
							}	
					}
			}	

		keypad( dialogwin->win, FALSE );
		clear_dialogwin( dialogwin );

		if( confirmed == 2 )
			repasswd[x] = '\0';

		return confirmed;
	}

void clear_dialogwin( dialogwindowtype *dialogwin )
	{
		int x,y;

		systemlog( 3, "clear_dialogwin");

		if( dialogwin->hidden )
			{ //wipe all including decorations
				//yes and no boxes
				for( y = 0; y < 3; y++ ){
					for( x = 0; x < 7; x++ ){
						mvwprintw( dialogwin->yes, y, x, " " );
						mvwprintw( dialogwin->no, y, x, " " );
					}
				}
				
				//rest of dialogwindow
				for( y = 0; y < dialogwin->h; y++ ){
					for( x = 0; x < dialogwin->w; x++ ){
						mvwprintw( dialogwin->win, y, x, " " );
					}		
				}
			}
		else
			{ //just wipe the window content
				//yes and no boxes
				for( y = 0; y < 3; y++ ){
					for( x = 0; x < 7; x++ ){
						mvwprintw( dialogwin->yes, y, x, " " );
						mvwprintw( dialogwin->no, y, x, " " );
					}
				}
				//rest of dialogwindow
				for( y = 1; y < dialogwin->h -1; y++ ){
					for( x = 1; x < dialogwin->w -1; x++ ){
						mvwprintw( dialogwin->win, y, x, " " );
					}   
				}
			}
		
		wrefresh( dialogwin->yes );
		wrefresh( dialogwin->no );
		wrefresh( dialogwin->win );
	}

int insidedialogwin( WINDOW *win, int y, int x )
	{
		int inside = 0;
		systemlog( 3, "insidedialogwin?");

			if( x > win->_begx && x < win->_begx + win->_maxx && y > win->_begy && y < win->_begy + win->_maxy )
				inside = 1;

		return inside;
	}

int test_rw_permissiondenieds( Windowtype *win, char destiwd[] )
	{
		int f;
		int permisson_denied = 0;

		systemlog( 3, "test_rw_permissiondenieds");

		if( access( destiwd, W_OK ) )
			permisson_denied |= 2;

		for( f = 1; f <= printtotalnr( win->filelist ); f++ )
			{
				if( gotoEntry( win->filelist, f )->selected && access( gotoEntry( win->filelist, f )->file->d_name, W_OK ) )
					{
						permisson_denied |= 1;
					}
			}

		return permisson_denied;
	}

void destroy_dialogwin( dialogwindowtype *dialogwin )
	{ systemlog( 2, "destroing dialog window" );

		dialogwin->hidden = 1;
		clear_dialogwin( dialogwin );
		delwin( dialogwin->yes );
		delwin( dialogwin->no );
		delwin( dialogwin->win );
			free( dialogwin );

		systemlog( 2, "dialogwindow destroid" ); 
	}
