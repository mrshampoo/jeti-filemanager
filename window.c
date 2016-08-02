/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= window.c =-------*/
#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "environment.h"
#include "window.h"
#include "directorys.h"
#include "systemlog.h"
#include "projecttypes.h"

Windowtype *newwindow( int h, int w, int y, int x )
	{
		Windowtype *win;
		
		systemlog( 2, "creating new window..." );

		win = malloc(sizeof(Windowtype));

			win->win = newwin( h, w, y, x );
				win->h = h;
				win->w = w;
				win->x = x;
				win->y = y;

			win = init_win_colors( win );
			win->tab = init_wintabs_environment( win->tab, win->w );
			win = init_workdir( win );
			win->shortcuts = init_shortcuts();

			win->dir = opendir( win->wd );
				win->filelist = getEntrys( win->dir, win->wd, win->filelist, win->showhidden );

			win->mlevel = 0;	
			win->marker[win->mlevel] = 0;
			win->visible_marker = 0;
			win->slide[win->mlevel] = 0;
			win->hidden = 0;
			win->noexe = 0;

		systemlog( 2, "window created");
		return win;
	}

Windowtype *redefinewindow( Windowtype *win, int h, int w, int y, int x )
	{
		systemlog( 3, "redefining window.." );

		win->hidden = 1;
		clearwindow( win );
		delwin( win->win );
		clearEnvironment_tabs( win->tab );
		win->win = newwin( h, w, y, x);
		win->h = h;
		win->w = w;
		win->x = x;
		win->y = y;
		win->hidden = 0;
		win = init_win_colors( win );
		win->tab = init_wintabs_environment( win->tab, win->w );

		systemlog( 3, "window redefined" );
		return win;
	}

int loadnewdir( Windowtype *win, char wd[] )
	{
		int success = 1;
		int i = 0;

		if( !access(wd, R_OK) )
			{
				systemlog( 3, "loading new dir.." );

				closedir( win->dir );

				while( i != strlen(wd) )
      				{
        				win->wd[i] = wd[i];
        				i++;
      				}
				win->wd[i] = '\0';
					systemlog( 5, "enterd new pathname successfuly." );

				win->dir = opendir( wd );
					systemlog( 5, "dir was able to open successfuly." );
				win->filelist = getEntrys( win->dir, wd, win->filelist, win->showhidden );

				if( !strncmp( TERMINALNAME, "st", 2 ) )
					chdir( wd );

				systemlog( 3, "new dir loaded" );
			}
		else
			{
				success = 0;
				systemlog( 1, "ERROR: loadnewdir: no read access:" );
				systemlog( 91, wd );
			}

		systemlog( 3, "new dir loaded successfuly" );
		systemlog( 93, win->wd );
		return success;
	}

int filesize_length( int fsize )
	{
		int size = 1;

		systemlog( 4, "enterd fsize");

			while( fsize > 1000 )
				{
					fsize /= 1000;

					if( size == 1 )
						size = 2;
				}

			while( fsize > 10 )
				{
					fsize/=10;
					size++;
				}

		return size+1;
	}

static int colors_not_started = 1;

void printwindow( Windowtype *win )
	{
		int x = 0,
				y = 0,
				filecolor = 0,
				listsize,
				endofname = 0;

		systemlog( 2, "trying to prit window..." );

		//only start thees onse, else there will be a paint leakage
		if( colors_not_started )
			{
				start_color();
				assume_default_colors( -1, -1 );
				colors_not_started = 0;
			}

		init_pair(1, win->color[0], win->color[9] );
		init_pair(2, win->color[1], win->color[9] ); /*executebles*/
 		init_pair(3, win->color[2], win->color[9] ); /*katalogs*/
		init_pair(4, win->color[3], win->color[9] ); /*links*/
		init_pair(5, win->color[4], win->color[9] ); /*fifo*/
    	init_pair(6, win->color[5], win->color[9] ); /*char*/
    	init_pair(7, win->color[6], win->color[9] ); /*block*/
    	init_pair(8, win->color[7], win->color[9] ); /*error*/
		init_pair(9, win->color[8], win->color[9] ); /*selected*/

		if( !win->hidden )
			{

				/*get number of printeble objects*/
				systemlog( 4,"get number of printeble objects" );
				if( win->h-1 > (getlast( win->filelist))->number + 1 )
					{
						listsize = (getlast( win->filelist ))->number + 1;
					}
				else
					{
						listsize = win->h-1;
					}

				/* how far down in the filelist are we?*/
				systemlog( 4,"how far down in the filelist are we?" );
				if( ((getlast(win->filelist))->number - win->slide[win->mlevel] < win->h -2) && win->slide[win->mlevel] != 0 )
					{
						if( (getlast(win->filelist))->number +2 - win->h < 0 )
							win->slide[win->mlevel] = 0;
						else
							win->slide[win->mlevel] = (getlast(win->filelist))->number +2 -win->h;
					}

				/* window decorations */
				systemlog( 4,"window decorations" );
				wattron( win->win, COLOR_PAIR(1) );
				wattroff( win->win, A_REVERSE );
				clear();
				box( win->win, 0, 0 );
				for( x = 0; win->wd[x] != '\0' && x+2 < win->w -2; x++ )
					{
						mvwprintw( win->win, 0,x+2,"%c", win->wd[x] );
					}

				//rewind shourtcuts
				systemlog( 4,"rewind shourtcuts" );
				while( win->shortcuts->prev != NULL )
					win->shortcuts = win->shortcuts->prev;
				//Decorate with shortcuts
				systemlog( 4,"Decorate with shortcuts" );
				for( y = 1; ( y < win->h-1) && ( win->shortcuts->dir[0] != '_' ); y++ )
					{
						if( win->shortcuts->next != NULL )
							{
								systemlog( 5,"printing shourtcuts," );
								mvwprintw( win->win, y, 0, "%c", win->shortcuts->label );
								win->shortcuts = win->shortcuts->next;
							}
						else if( win->shortcuts->next == NULL )
							{
								systemlog( 5,"printing last shourtcut." );
								mvwprintw( win->win, y, 0, "%c", win->shortcuts->label );
								break;
							}
					}
			//--= window substans  =----------------------------------------------------->
    		for( y = 1; y < win->h-1; y++ )
					{
						systemlog( 6,"--= inside window substans =--" );

						//test that we are not out of file_range before calling these funktions
						if( y+win->slide[win->mlevel] <= getlast( win->filelist )->number )
							{
								//makes shure selections gets right file
								win->filelist = gotoEntry( win->filelist, y+win->slide[win->mlevel] );

								//collorate row tru filetype
								systemlog( 6,"collorate row tru filetype" );
								if( win->filelist->selected )
									filecolor = 9;
								else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "ERROR") || isoffiletype( win->filelist, y+win->slide[win->mlevel], "linkER"))
            						filecolor = 8;
            					else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "block") )
              						filecolor = 7;
            					else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "char") )
              						filecolor = 6;
            					else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "fifo") )
              						filecolor = 5;
            					else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "link") || isoffiletype( win->filelist, y+win->slide[win->mlevel], "<DIR L>") )
              						filecolor = 4;
            					else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "<DIR>") )
              						filecolor = 3;
            					else if( S_IEXEC & win->filelist->status )
              						filecolor = 2;
            					else
              						filecolor = 1;
							}

						//make line where marker is, white.
						systemlog( 6,"make line where marker is, white\n" );
						if( win->visible_marker && y+win->slide[win->mlevel] == win->marker[win->mlevel] )
			 				{
								filecolor = 1;
								wattron( win->win, A_REVERSE  );
							}
						else
							{ 
								wattroff( win->win, A_REVERSE );
							}
							

						//rewind tabs
						while( win->tab->prev != NULL )
							win->tab = win->tab->prev;

    					for( x = 1; x <= (win->w -2) ; x++ )
							{
								//as long as there are files to print
								systemlog( 7,"1" );
								if( y < listsize )
           							{
										//keep printibg while not in next tabfeeld, then shange to next tabfeeld
										systemlog( 7,"2" );
										if( win->tab->next != NULL && x >= win->tab->next->start )
                      						{ win->tab = win->tab->next; endofname = 0; }

										//keep printing while there is something to print, ( check is inside switch/case )
										systemlog( 7,"3" );
										if( win->tab->opt == presentation && printfilename( win->filelist, x - win->tab->start, y + win->slide[win->mlevel] ) == '\0')
               								endofname = 1;

										//--= inside this actual printing is made =-------------------------------------------->
										switch( win->tab->opt )
      										{
          										case selected:
																	systemlog( 7,"S" );
            														if( x - win->tab->start >= 0 && win->filelist->selected && x - win->tab->start < win->tab->length )
																		{
																			wattron( win->win, COLOR_PAIR( filecolor ) );
              																mvwprintw( win->win, y, x, "*" );
																		}
																	else
																		{
																			wattron(win->win, COLOR_PAIR(1) );
																			mvwprintw( win->win, y, x, " " );

																		}
            											break;

          										case filename:
																	systemlog( 7,"F" );
																	if( x - win->tab->start >= 0 && x - win->tab->start < strlen( win->filelist->filename ) )
																		{	
																			wattron( win->win, COLOR_PAIR( filecolor ) );
																			mvwprintw( win->win, y, x, "%c", win->filelist->filename[x - win->tab->start] );
																		}
																	else
																		{
																			wattron(win->win, COLOR_PAIR(1) );
            																mvwprintw( win->win, y, x, " " );
																		}
            											break;

          										case presentation:
																	systemlog( 7,"P" );
            														if( x - win->tab->start >= 0 && !endofname )
																		{
              																wattron( win->win, COLOR_PAIR( filecolor ) );
              																mvwprintw( win->win, y, x, "%c", printfilename( win->filelist, x - win->tab->start, y + win->slide[win->mlevel]) );
																		}
            														else
																		{
																			wattron( win->win, COLOR_PAIR(1) );
              																mvwprintw( win->win, y, x, " " );
																		}
            											break;

          										case filetype:
																	systemlog( 7,"T" );
            														if( x - win->tab->start >= 0 && x - win->tab->start < printfiletypelenght( win->filelist, y + win->slide[win->mlevel] ) )
																		{
                              												wattron( win->win, COLOR_PAIR( filecolor ) );
              																mvwprintw( win->win, y, x, "%c", printCurrentfiletype( win->filelist, x - win->tab->start, y+win->slide[win->mlevel]) );
																		}
            														else
																		{
																			wattron( win->win, COLOR_PAIR(1) );
              																mvwprintw( win->win, y, x, " " );
																		}
            											break;

												case mode: 
																	systemlog( 7,"M" );
																	if( x - win->tab->start >= 0 && x - win->tab->start < win->tab->length )
																		{
																			wattron( win->win, COLOR_PAIR(1) );

																			if( S_ISDIR( win->filelist->status )  && x - win->tab->start == 0 )
																				mvwprintw( win->win, y, x, "d" );
																			else if( ( win->filelist->status & S_IRUSR) && x - win->tab->start == 1 )
																				mvwprintw( win->win, y, x, "r" );
																			else if( ( win->filelist->status & S_IWUSR) && x - win->tab->start == 2 )
                                                                                mvwprintw( win->win, y, x, "w" );
																			else if( ( win->filelist->status & S_IXUSR) && x - win->tab->start == 3 )
                                                                                mvwprintw( win->win, y, x, "x" );
																			else if( ( win->filelist->status & S_IRGRP) && x - win->tab->start == 4 )
                                                                                mvwprintw( win->win, y, x, "r" );
																			else if( ( win->filelist->status & S_IWGRP) && x - win->tab->start == 5 )
                                                                                mvwprintw( win->win, y, x, "w" );
																			else if( ( win->filelist->status & S_IXGRP) && x - win->tab->start == 6 )
                                                                                mvwprintw( win->win, y, x, "x" );
																			else if( ( win->filelist->status & S_IROTH) && x - win->tab->start == 7 )
                                                                                mvwprintw( win->win, y, x, "r" );
																			else if( ( win->filelist->status & S_IWOTH) && x - win->tab->start == 8 )
                                                                                mvwprintw( win->win, y, x, "w" );
																			else if( ( win->filelist->status & S_IXOTH) && x - win->tab->start == 9 )
                                                                                mvwprintw( win->win, y, x, "x" );
																			else
																				mvwprintw( win->win, y, x, "-" );
																		}
																	else
																		{
																			wattron( win->win, COLOR_PAIR(1) );
																			mvwprintw( win->win, y, x, " " );
																		}
																	break;

          										case size:
																	systemlog( 7,"Z" );
																	if( x - win->tab->start == 0 && x - win->tab->start < win->tab->length )
																		{
																			wattron( win->win, COLOR_PAIR(1) );
																			if( win->filelist->filesize < 1000 )
																				mvwprintw( win->win, y, x, "%ib", win->filelist->filesize );
																			else if( win->filelist->filesize < 1000000 )
																				mvwprintw( win->win, y, x, "%ikb", win->filelist->filesize/1000 );
																			else if( win->filelist->filesize < 1000000000 )
																				mvwprintw( win->win, y, x, "%iMb", win->filelist->filesize/1000000 );
																			else if( win->filelist->filesize < 1000000000000 )
																				mvwprintw( win->win, y, x, "%iGb", win->filelist->filesize/1000000000 );
																			else
																				mvwprintw( win->win, y, x, "%iTb", win->filelist->filesize/1000000000000 );
																		}
																	else if( x - win->tab->start >= 1 && x - win->tab->start < win->tab->length && x - win->tab->start < filesize_length( win->filelist->filesize ))
																		{
																			//do nothing
																		}
																	else
																		{
																			wattron( win->win, COLOR_PAIR(1) );
																			mvwprintw( win->win, y, x, " " );
																		}
            														break;

          										default:
																	wattron( win->win, COLOR_PAIR(1) ); 
            														mvwprintw( win->win, y, x, " " );
            														break;
      									}
									}
								else
									{
										wattron( win->win, COLOR_PAIR(1) );
										mvwprintw( win->win, y, x, " " );
									}
							}

								endofname = 0;
								win->tab = gettab( win->tab, 1 );
						}
			}

		wrefresh( win->win );
		systemlog( 2, "window printed" );
	}

void clearwindow( Windowtype *win )
	{
		int x, 
			y;

		systemlog( 2, "clearing window..." );

		for( y = 0; y < win->h; y++ ){
			for( x = 0; x < win->w; x++ ){

				if( win->hidden )
					mvwprintw( win->win, y, x, " " );

				else if( y > 0 && y < win->h-1 && x > 0 && x < win->w-1)
					mvwprintw( win->win, y, x, " " );
				
			}
		}

		systemlog( 2, "window cleard" );

	}

int toglehidewin( Windowtype *win )
	{
		systemlog( 3, "hiding window.." );
		if( win->hidden )
			win->hidden = 0;

		else
			win->hidden = 1;

		systemlog( 3, "window hidden" );
		return win->hidden;
	}

int insidewindow( Windowtype *win, int y, int x )
	{
		int inside = 0;
		
		systemlog( 3, "testing if inside window." );

			if( (x > win->x) && (x < win->x + win->w)
			 && (y > win->y) && (y < win->y + win->h) )
				{
					inside = 1;
				}

		return inside;
	}

int onshortcutrow( Windowtype *win, int y, int x )
	{
		int spoton = 1;

		systemlog( 3, "testing if on shourtcut." );

		win->shortcuts = getshortcut( win->shortcuts, 1 );	

			while( win->shortcuts->next != NULL && spoton )
				{
					if( win->shortcuts->x + win->x == x || win->shortcuts->y + win->y == y )
						{
							win->shortcuts = win->shortcuts->next;
						}
					else
						{
							spoton = 0;
						}
				}

			if(!( win->shortcuts->x + win->x == x || win->shortcuts->y + win->y ))
				spoton = 0;

		return spoton;
	}

shortcutType *getshortcut( shortcutType *shorty, int pos )
	{
		systemlog( 3, "seeking shourtcut." );

		while( shorty->prev != NULL )
			{
				shorty = shorty->prev;
			}

		while( shorty->next != NULL && shorty->y != pos )
			{
				shorty = shorty->next;
			}

		return shorty;
	}

void destroywindow( Windowtype *win ) 
	{
		systemlog( 2, "destroying window.." );

		win->hidden = 1;
		clearwindow( win );

		clearEnvironment_tabs( win->tab );

		clearEntrys( win->filelist );
		closedir( win->dir );

		delwin( win->win );
			free( win );

		systemlog( 2, "window destroyd" );
	}
