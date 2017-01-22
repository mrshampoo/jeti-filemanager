/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= main./c =-------*/
#include <ncurses.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "projecttypes.h"
#include "systemlog.h"
#include "navigation.h"
#include "environment.h"
#include "window.h"
#include "directorys.h"
#include "command-window.h"
#include "handleflags.h"
#include "soundeffects.h"

#define _globals_

int main( int argc, char *argv[] )
  {
		/*3 states*/
		int press_action_one = 0; //left mouse action, 0=nothing, +=pressed once, -=pressed twise.
		int press_action_two = 0; //right mouse action, 0=nothing, +=pressed once, -=pressed twise.
		int press_action_shortcuts = 0; //0=nothing, +=pressed once.

		/*booleans*/
		int repaint = 3;
		int EXIT = 0;

		/*integers*/
		int ch; //charecter reading
		int n; //loopcounter for configs

		MEVENT event;
		filetypeAction *actions = NULL;
		soundeffectType *sounds = NULL;		

		Windowtype *win_one;
		Windowtype *win_two;
		Windowtype *activwin;
		Windowtype *passivwin;
		cmdWindowtype *cmdwin;

		char buff[256];
		FILE *fp;
		int l = 0;
		unsigned long int option_catch = 0;

		void catchresize( int dummy )
			{
				systemlog( 2, "catchresize");
				endwin();
				refresh();
				clear();
				win_one = redefinewindow( win_one, LINES, COLS/2, 0, 0 );
				win_two = redefinewindow( win_two, LINES, COLS/2, 0, COLS/2 );
				activwin = win_one;
				passivwin = win_two;
				nodelay( win_one->win, TRUE );
				nodelay( win_two->win, TRUE );
				redefine_navigation();
				printwindow( activwin, MOVEMENT, 1 );
				printwindow( passivwin, MOVEMENT, 0 );
				keypad( activwin->win, TRUE );
			}
		
		void catchexit( int dummy )
			{
				playsound( sounds, 15 );
				systemlog( 2, "--= catched exit =------------->\n" );
				repaint = -1;
				EXIT = 1;
			}

		/*Set consoletitle*/
		printf("%c]0;%s%c", '\033', LABEL_PROGRAM, '\007');


		/*signal handeling*/
		signal( SIGINT, catchexit ); //Ctrl-C
		signal( SIGQUIT, catchexit ); //ex. errors
		signal( SIGTERM, catchexit ); //got a polite exit request
		signal( SIGHUP, catchexit ); //terminal exit (the X button)
		signal( SIGWINCH, catchresize ); //terminal rezised

		//nullify stuff
		TERMINALNAME[0] = '\0';
		CONFIG_NAME[0][0] = '\0';
		CONFIG_NR = 0;
		FILEPATH_LOG[0] = '\0';
		DW_REACTION = -1;
		LOGPRIO = -1;
		STARTUP = 1;
		ANIMATING = 1;
		MOVEMENT = 100;
		
		//set basics
		handle_flags( argc, argv );

		initscr();

			/*setup curses*/
			systemlog( 4, "setup curses");
			cbreak();
			noecho();
			nodelay(stdscr, TRUE);
			curs_set(0);

			/*create windows*/
			systemlog( 4, "create windows");
			win_one = newwindow( LINES, COLS/2, 0, 0 );
				activwin = win_one;
				nodelay( win_one->win, TRUE );

			win_two = newwindow( LINES, COLS/2, 0, COLS/2 );
				passivwin = win_two;
				nodelay( win_two->win, TRUE );

			cmdwin = new_cmdwindow( 3, COLS-4, LINES/2, 2, activwin->wd );

			keypad( activwin->win, TRUE ); 
			mousemask( ALL_MOUSE_EVENTS, NULL ); 
			mouseinterval(0); /*makes mousewheel flouting*/

			//initiate stuff------------------------------------------------------>
			systemlog( 4, "initiate stuff" );
			for( n = 0; n <= CONFIG_NR -1; n++ )
				{
					systemlog( 4, "opening config: " ); systemlog( 94, CONFIG_NAME[ n ] );
					fp = fopen( CONFIG_NAME[ n ], "rb");

					while( (buff[l] = fgetc(fp)) != EOF )
						{
							if( buff[l] == '\n' )
								{
									if( fget_win_colors( buff, activwin ) )
										{
											fget_win_colors( buff, passivwin );
											option_catch |= 1;
										}
									else if( fget_tabs( buff, &activwin->tab, activwin->w ) )
										{
											fget_tabs( buff, &passivwin->tab, passivwin->w );
											option_catch |= 2;
										}
									else if( fget_fileAction( buff, &actions ) )
										{
											option_catch |= 4;
										}
									else if( fget_shortcuts( buff, &activwin->shortcuts ) )
										{
											fget_shortcuts( buff, &passivwin->shortcuts );
											option_catch |= 8;
										}
									else if( fget_soundeffects( buff, &sounds ) )
										{
											option_catch |= 16;
										}
									else if( fget_terminalname( buff ) )
										{
											option_catch |= 32;
										}
									else if( fget_dialogwinreactions( buff ) )
										{
											option_catch |= 64;
										}
									else if( fget_logpath( buff ) )
										{
											option_catch |= 128;
										}
									else if( fget_win_showhidden( buff, activwin ) )
										{
											fget_win_showhidden( buff, passivwin );
											option_catch |= 256;
										}
									else if( fget_win_workdir( buff, activwin ) )
										{
											fget_win_workdir( buff, passivwin );
											option_catch |= 512;
										}
									else if( fget_win_workdir_type( buff, activwin ) )
										{
											fget_win_workdir_type( buff, passivwin );
											option_catch |= 1024;
										}
									else if( fget_win_shortcut_type( buff, activwin ) )
										{
											fget_win_shortcut_type( buff, passivwin );
											option_catch |= 2048;
										}
									else if( fget_win_filelist_type( buff, activwin ) )
										{
											fget_win_filelist_type( buff, passivwin );
											option_catch |= 4096;
										}
									else if( fget_animations( buff ) )
										{
											option_catch |= 16384;
										}

									l = 0;
								}
							else
								l++;
						}
					fclose( fp );
				}

			if( !( option_catch & 2 ) )
				{
					put_default_list_tabs( &activwin->tab );
					put_default_list_tabs( &passivwin->tab );
				}
			if( !( option_catch & 4 ) )
				{
					put_default_list_filetypeAction( &actions );
				}
			if( !( option_catch & 8 ) )
				{
					put_default_list_shortcuts( &activwin->shortcuts );
					put_default_list_shortcuts( &passivwin->shortcuts );
				}
			if( !( option_catch & 16 ) )
				{
					put_default_list_sounds( &sounds );
				}
			if( !( option_catch & 32 ) )
				{
					put_default_terminalname();
				}
			if( !( option_catch & 64 ) )
				{
					put_default_dialogwinreactions();
				}
			if( !( option_catch & 128 ) )
				{
					put_default_logpath();
				}
			if( !( option_catch & 256 ) )
				{
					put_default_win_showhidden( activwin );
					put_default_win_showhidden( passivwin );
				}
			if( !( option_catch & 512 ) )
				{
					put_default_win_workdir( activwin );
					put_default_win_workdir( passivwin );
				}
			if( !( option_catch & 1024 ) )
				{
					put_default_workdir_type( activwin );
					put_default_workdir_type( passivwin );
				}
			if( !( option_catch & 2048 ) )
				{
					put_default_shortcut_type( activwin );
					put_default_shortcut_type( passivwin );
				}
			if( !( option_catch & 4096 ) )
				{
					put_default_filelist_type( activwin );
					put_default_filelist_type( passivwin );
				}
			if( !( option_catch & 8192 ) )
				{
					put_default_logpath();
				}
			if( !( option_catch & 16384 ) )
				{
					put_default_animations();
				}
			put_start_totablist( activwin->tab, activwin->w -2 );
			put_start_totablist( passivwin->tab, passivwin->w -2 );
			put_default_win_colors( activwin );
			put_default_win_colors( passivwin );
			init_colors( activwin );
			init_navigation();
			loadnewdir( activwin, activwin->wd );
			loadnewdir( passivwin, passivwin->wd );

			//welcome sound! ----------------------------------------------------------------------------------------->
			playsound( sounds, 14 );
	 			
			while( !EXIT  )
				{
					if( repaint )
						{
							systemlog( 3, "repaint\n" );
							if( ANIMATING && repaint & 1 )
								{
									clearwindow( activwin );
									printwindow( activwin, MOVEMENT, 1 );
								}
							else
								{
									clearwindow( activwin );
									printwindow( activwin, 0, 1 );
								}

							if( ANIMATING && repaint & 2 )
								{
									clearwindow( passivwin );
									printwindow( passivwin, MOVEMENT, 0 );
								}
							else
								{
									clearwindow( passivwin );
									printwindow( passivwin, 0, 0 );
								}

							if( MOVEMENT > 0 )
								MOVEMENT--;
							else if( MOVEMENT < 0 )
								MOVEMENT++;
							else
								repaint = 0;
						}

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
												if( insidewindow( activwin, event.y, event.x ) )
													{
														press_action_one = activwin->y + event.y + activwin->slide[activwin->mlevel];
													}
												else if( insidewindow( passivwin, event.y, event.x ) )
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
												else if( (press_action_shortcuts = onshortcut( activwin, event.y, event.x )) )
													{
													}
												else if( (press_action_shortcuts = onshortcut( passivwin, event.y, event.x )) )
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

												if( activwin->visible_marker && press_action_one == activwin->marker[activwin->mlevel] )
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
														if( !activwin->visible_marker )
															activwin->visible_marker = 1;
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
												else if( onshortcut( activwin, event.y, event.x ) == press_action_shortcuts && press_action_shortcuts != 0 )
													{
														activwin->shortcuts = getshortcut( activwin->shortcuts, press_action_shortcuts );
														repaint = handleshortcut( activwin, passivwin, sounds );
													}

												press_action_one = 0;
												press_action_two = 0;
												press_action_shortcuts = -1;
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

												if( activwin->visible_marker && press_action_two == activwin->marker[activwin->mlevel] )
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
														if( !activwin->visible_marker )
															activwin->visible_marker = 1;
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

							case KEY_UP:	
								repaint = stepupp( activwin, sounds );
								break;

							case KEY_DOWN:
								repaint = stepdown( activwin, sounds );
								break;

							case KEY_RIGHT:
								if( activwin == win_two )
									repaint = back_one_folder( activwin, sounds );
								else
									{
										activwin = win_two;
										passivwin = win_one;
										keypad( activwin->win, TRUE );
										repaint = 3;
									}
								break;

							case KEY_LEFT:
								if( activwin == win_one )
									repaint = back_one_folder( activwin, sounds );
								else
									{
										activwin = win_one;
										passivwin = win_two;
										keypad( activwin->win, TRUE );
										repaint = 3;
									}
								break;

							//Enter/Return
							case 0xA:
								repaint = enter( activwin, actions, sounds );
								break;

							//Esc
							case 0x1B:
								if( activwin->visible_marker )
									activwin->visible_marker = 0;
								else
									{
										activwin->marker[activwin->mlevel] = 0;
										activwin->slide[activwin->mlevel] = 0;
									}
								repaint = 1;
								break;

							case ':':
								keypad( activwin->win, FALSE );
								togglehide( cmdwin );
								putnewdir( cmdwin, activwin->wd, passivwin->wd );
								repaint = printcmdwin( cmdwin, activwin, passivwin );
								keypad( activwin->win, TRUE );
								break;

							case ' ':
								if( !EXIT )
									repaint = selectfile( activwin, sounds );
								break;

							case 'c':
								repaint = copyfiles( activwin, passivwin, sounds );
								break;

							case 'm':
								repaint = movefiles( activwin, passivwin, sounds );
								break;

							case KEY_DC:
								repaint = removefiles( activwin, sounds );
								if( strlen( activwin->wd ) == strlen( passivwin->wd )&& !strncmp( activwin->wd, passivwin->wd, strlen( passivwin->wd ) ) )
									loadnewdir( passivwin, passivwin->wd );
								break;
	
							default:
								if( !ANIMATING || MOVEMENT == 0)
									{
										sleep(0);
									}
								else
									{
										sleep(0.6);	
									}
								break;
											
						}
				}

			destroy_navigation();
			clearEnvironmett_sounds( &sounds );
			clearEnvironment_actions( &actions );
			clearEnvironment_shortcuts( &win_one->shortcuts );
			clearEnvironment_shortcuts( &win_two->shortcuts );
			destroycmdwin( cmdwin );
			destroywindow( win_one );
			destroywindow( win_two );
		endwin();
	return 0;
  }
