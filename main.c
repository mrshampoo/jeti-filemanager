/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= main./c =-------*/
#include <ncurses.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "projecttypes.h"
#include "systemlog.h"
#include "navigation.h"
#include "environment.h"
#include "window.h"
#include "directorys.h"
#include "command-window.h"
#include "handleflags.h"

int main( int argc, char *argv[] )
  {
		/*booleans*/
		int press_action_one = 0;
		int press_action_two = 0;
		int press_action_shortcuts = 0;
		int repaint = 0;
		int EXIT = 0;

		/*integers*/
		int ch;

		MEVENT event;
		filetypeAction *actions;

		soundeffectType *sounds;		

		Windowtype *win_one;
		Windowtype *win_two;
		Windowtype *activwin;
		Windowtype *passivwin;
		cmdWindowtype *cmdwin;

		void catchresize( int dummy )
			{
				endwin();
				refresh();
				clear();
				win_one = redefinewindow( win_one, LINES, COLS/2, 0, 0 );
				win_two = redefinewindow( win_two, LINES, COLS/2, 0, COLS/2 );
				printwindow( win_one );
				printwindow( win_two );
				keypad( activwin->win, TRUE );
			}
		
		void catchexit( int dummy )
			{
				jetilog( 3, "catched exit..\n" );
				EXIT = 1;
			}

		handle_flags( argc, argv );

		/*Set consoletitle*/
		printf("%c]0;%s%c", '\033', "Jeti!", '\007');

		initscr();

			cbreak();
			noecho();
			nodelay(stdscr, TRUE);
			curs_set(0);

			signal( SIGINT, catchexit ); //Ctrl-C
			signal( SIGQUIT, catchexit ); //ex. errors
			signal( SIGTERM, catchexit ); //got a polite exit request
			signal( SIGHUP, catchexit ); //terminal exit (the X button)
			signal( SIGWINCH, catchresize ); //terminal rezised

			actions = init_fileAction();
			sounds = init_soundeffects();

			win_one = newwindow( LINES, COLS/2, 0, 0 );
			win_two = newwindow( LINES, COLS/2, 0, COLS/2 );
			activwin = win_one;
			passivwin = win_two;
			cmdwin = new_cmdwindow( 3, COLS-4, LINES/2, 2, activwin->wd );

				keypad( activwin->win, TRUE );
				mousemask( ALL_MOUSE_EVENTS, NULL );
				mouseinterval(0); /*makes mousewheel flouting*/

				printwindow( win_one );
				printwindow( win_two );
	 			
				while( !EXIT )
					{	
						ch = wgetch( activwin->win );
						switch( ch )
							{
								case KEY_MOUSE:
												if( getmouse( &event ) == OK )
													{ 
														if( event.bstate & BUTTON4_PRESSED )
															{/*scroll_up*/
																press_action_one = 0;
																press_action_two = 0;

																if( insidewindow( activwin, event.y, event.x ) )
																	{
																		repaint = scrollupp( activwin, sounds );
																	}
																else if( insidewindow( passivwin, event.y, event.x ) )
																	{
																		repaint = scrollupp( passivwin, sounds );
																	}
															}
														else if( event.bstate & BUTTON5_PRESSED )
															{/*scroll_down*/
																press_action_one = 0;
																press_action_two = 0;

																if( insidewindow( activwin, event.y, event.x ) )
																	{
																		repaint = scrolldown( activwin, sounds );
																	}
																else if( insidewindow( passivwin, event.y, event.x ) )
																	{
																		repaint = scrolldown( passivwin, sounds );
																	}
															}
														else if( event.bstate & BUTTON1_PRESSED )
															{/*left_singlepressed*/
																if( insidewindow( passivwin, event.y, event.x ) )
																	{
																		if( activwin != win_one )
																			{
																				activwin = win_one;
																				passivwin = win_two;
																			}
																		else
																			{
																				activwin = win_two;
                                              									passivwin = win_one;
																			}
																		keypad( activwin->win, TRUE );
																		press_action_one = activwin->y + event.y + activwin->slide[activwin->mlevel];
																	}
																else if( insidewindow( activwin, event.y, event.x ) )
																	{
																		press_action_one = activwin->y + event.y + activwin->slide[activwin->mlevel];
																	}
																else if( onshortcutrow( passivwin, event.y, event.x ) )
																	{
																		if( activwin != win_one )
																			{
																				activwin = win_one;
																				passivwin = win_two;
																			}
																		else
																			{
																				activwin = win_two;
																				passivwin = win_one;
																			}
																		keypad( activwin->win, TRUE );
																		press_action_shortcuts = event.y;
																	}
																else if( onshortcutrow( activwin, event.y, event.x ) )
																	{
																		press_action_shortcuts = event.y;	
																	}
																				
																if( press_action_one == activwin->marker[activwin->mlevel] )
																	{/*left_doublepressed*/
																		press_action_one = -press_action_one;
																	}

																press_action_two = 0;
															}
														else if( event.bstate & BUTTON1_RELEASED )
															{
																if( insidewindow( activwin, event.y, event.x ) && press_action_one > 0 && press_action_one <= printtotalnr( activwin->filelist ) )
																	{/*left_singlereleased*/
																		activwin->marker[activwin->mlevel] = press_action_one;
																		repaint = 1;
																	}
																else if( insidewindow( passivwin, event.y, event.x ) && press_action_one != 0 )
																	{/*draged*/
																		repaint = copyfiles( activwin, passivwin, sounds );
																	}
																else if( insidewindow( activwin, event.y, event.x ) && press_action_one < 0 )
																	{/*left_doublereleased*/
																		repaint = enter( activwin, actions, sounds );
																	}
																else if( onshortcutrow( activwin, event.y, event.x ) && event.y == press_action_shortcuts )
																	{
																		activwin->shortcuts = getshortcut( activwin->shortcuts, press_action_shortcuts );
																		repaint = handleshortcut( activwin, passivwin, sounds );
																	}

																press_action_one = 0;
																press_action_two = 0;
																press_action_shortcuts = 0;
															}
														else if( event.bstate & BUTTON3_PRESSED )
															{/*right_singlepressed*/
																if( insidewindow( passivwin, event.y, event.x ) ) 
																	{
																		if( activwin != win_one ) 
																			{
																				activwin = win_one;
																				passivwin = win_two;
																			}
																		else
																			{
																				activwin = win_two;
																				passivwin = win_one;
																			}
																		keypad( activwin->win, TRUE );
																	}

																press_action_two = activwin->y + event.y + activwin->slide[activwin->mlevel];

																if( press_action_two == activwin->marker[activwin->mlevel] )
																	{/*right_doublepressed*/
																		press_action_two = -press_action_two;
																	}

																press_action_one = 0;
															}
														else if( event.bstate & BUTTON3_RELEASED )
															{
																if( insidewindow( activwin, event.y, event.x ) && press_action_two > 0 && press_action_two <= printtotalnr( activwin->filelist ))
																	{/*right_singlereleased*/
																		activwin->marker[activwin->mlevel] = press_action_two;
																		repaint = selectfile( activwin, sounds );
																	}
																else if( insidewindow( passivwin, event.y, event.x ) && press_action_two != 0 )
																	{/*draged*/
																		repaint = movefiles( activwin, passivwin, sounds );
																	}
																else if( insidewindow( activwin, event.y, event.x ) && press_action_two < 0 )
																	{/*left_doublereleased*/
																		repaint = selectfile( activwin, sounds );
																	}

																press_action_two = 0;
																press_action_one = 0;
															}
													}
												break;

								case KEY_UP:	repaint = stepupp( activwin, sounds );
												break;

								case KEY_DOWN:	repaint = stepdown( activwin, sounds );
												break;

								case KEY_RIGHT: activwin = win_two;
												passivwin = win_one;
												keypad( activwin->win, TRUE ); 
												break;

								case KEY_LEFT: 	activwin = win_one;
												passivwin = win_two;
												keypad( activwin->win, TRUE );
												break;

								case 0xA:		repaint = enter( activwin, actions, sounds );
												break;

								case 0x1B: 		activwin->marker[activwin->mlevel] = -1;
												activwin->slide[activwin->mlevel] = 0;
												repaint = 1;
												//EXIT = 1;
												break;

								case ':':		keypad( activwin->win, FALSE );
												togglehide( cmdwin );
												putnewdir( cmdwin, activwin->wd, passivwin->wd );
												repaint = printcmdwin( cmdwin, activwin, passivwin );
												keypad( activwin->win, TRUE );
												break;

								case ' ':		repaint = selectfile( activwin, sounds );
												break;

								case 'c': 		repaint = copyfiles( activwin, passivwin, sounds );
												break;

								case 'm':     	repaint = movefiles( activwin, passivwin, sounds );
												break;

								case KEY_DC:  	repaint = removefiles( activwin, sounds );
												break;
																
								default: 
												break;
							}
						
						if( repaint )
							{
								clearwindow( activwin );
                				printwindow( activwin );
								
								clearwindow( passivwin );
                				printwindow( passivwin );

                				repaint = 0;
							}
						
					}

			clearEnvironmett_sounds( sounds );
			clearEnvironment_actions( actions );
			clearEnvironment_shortcuts( win_one->shortcuts );
            clearEnvironment_shortcuts( win_two->shortcuts );
			destroycmdwin( cmdwin );
			destroywindow( win_one );
			destroywindow( win_two );
		endwin();
	return 0;
  }
