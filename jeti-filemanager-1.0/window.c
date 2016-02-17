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
		
		jetilog( 2, "creating new window...\n" );

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
			win->marker[win->mlevel] = -1;
			win->slide[win->mlevel] = 0;
			win->hiden = 0;
			win->noexe = 0;

		jetilog( 2, "window created\n");
		return win;
	}

Windowtype *redefinewindow( Windowtype *win, int h, int w, int y, int x )
	{
		jetilog( 3, "redefining window..\n" );

		win->hiden = 1;
		clearwindow( win );
		delwin( win->win );
		clearEnvironment_tabs( win->tab );
    win->win = newwin( h, w, y, x);
		win->h = h;
		win->w = w;
		win->x = x;
		win->y = y;
		win->hiden = 0;
    win = init_win_colors( win );
		win->tab = init_wintabs_environment( win->tab, win->w );

		jetilog( 3, "window redefined\n" );
		return win;
	}

int loadnewdir( Windowtype *win, char wd[] )
	{
		int success = 1;
		int i = 0;

		if( !access(wd, R_OK) )
			{
				jetilog( 3, "loading new dir..\n" );

				closedir( win->dir );

				while( i != strlen(wd) )
      				{
        				win->wd[i] = wd[i];
        				i++;
      				}
      			win->wd[i] = '\0';
					jetilog( 5, "enterd new pathname successfuly.\n" );

				win->dir = opendir( wd );
					jetilog( 5, "dir was able to open successfuly.\n" );
				win->filelist = getEntrys( win->dir, wd, win->filelist, win->showhidden );

				jetilog( 3, "new dir loaded\n" );
			}
		else
			{
				success = 0;
				jetilog( 1, "ERROR: no read access\n" );
			}
		return success;
	}

int filesize_length( int fsize )
	{
		int size = 1;

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

void printwindow( Windowtype *win )
	{
		int x = 0,
				y = 0,
				filecolor = 0,
				listsize,
				endofname = 0;

		jetilog( 2, "trying to prit window...\n" );

		start_color();
		assume_default_colors( -1, -1 );
		init_pair(1, win->color[0], -1 );
		init_pair(2, win->color[1], -1 ); /*executebles*/
 		init_pair(3, win->color[2], -1 ); /*katalogs*/
		init_pair(4, win->color[3], -1 ); /*links*/
		init_pair(5, win->color[4], -1 ); /*fifo*/
    	init_pair(6, win->color[5], -1 ); /*char*/
    	init_pair(7, win->color[6], -1 ); /*block*/
    	init_pair(8, win->color[7], -1 ); /*error*/
		init_pair(9, win->color[8], -1 ); /*selected*/

		if( !win->hiden )
			{

				/*get number of printeble objects*/
				jetilog( 4,"get number of printeble objects\n" );
				if( win->h-1 > (getlast( win->filelist))->number + 1 )
					{
						listsize = (getlast( win->filelist ))->number + 1;
					}
				else
					{
						listsize = win->h-1;
					}

				/* how far down in the filelist are we?*/
				jetilog( 4,"how far down in the filelist are we?\n" );
				if( ((getlast(win->filelist))->number - win->slide[win->mlevel] < win->h -2) && win->slide[win->mlevel] != 0 )
					{
						if( (getlast(win->filelist))->number +2 - win->h < 0 )
							win->slide[win->mlevel] = 0;
						else
							win->slide[win->mlevel] = (getlast(win->filelist))->number +2 -win->h;
					}

				/* window decorations */
				jetilog( 4,"window decorations\n" );
				wattron( win->win, COLOR_PAIR(1) );
				wattroff( win->win, A_REVERSE );
				clear();
				box( win->win, 0, 0 );
				for( x = 0; win->wd[x] != '\0' && x+2 < win->w -2; x++ )
					{
						mvwprintw( win->win, 0,x+2,"%c", win->wd[x] );
					}

				//rewind shourtcuts
				jetilog( 4,"rewind shourtcuts\n" );
				while( win->shortcuts->prev != NULL )
					win->shortcuts = win->shortcuts->prev;
				//Decorate with shortcuts
				jetilog( 4,"Decorate with shortcuts\n" );
				for( y = 1; ( y < win->h-1) && ( win->shortcuts->dir[0] != '_' ); y++ )
					{
						if( win->shortcuts->next != NULL )
							{
								jetilog( 5,"printing shourtcuts," );
								mvwprintw( win->win, y, 0, "%c", win->shortcuts->label );
								win->shortcuts = win->shortcuts->next;
							}
						else if( win->shortcuts->next == NULL )
							{
								jetilog( 5,"printing last shourtcut.\n" );
								mvwprintw( win->win, y, 0, "%c", win->shortcuts->label );
								break;
							}
					}
			//--= window substans  =----------------------------------------------------->
    		for( y = 1; y < win->h-1; y++ )
					{
						jetilog( 6,"--= inside window substans =--\n" );

						win->filelist = gotoEntry( win->filelist, y+win->slide[win->mlevel] );

						//collorate row tru filetype
						jetilog( 6,"collorate row tru filetype\n" );
						if( win->filelist->selected )
							filecolor = 9;
						else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "ERROR") )
            				filecolor = 8;
            			else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "block") )
              				filecolor = 7;
            			else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "char") )
              				filecolor = 6;
            			else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "fifo") )
              				filecolor = 5;
            			else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "link") )
              				filecolor = 4;
            			else if( isoffiletype( win->filelist, y+win->slide[win->mlevel], "<KAT>") )
              				filecolor = 3;
            			else if( S_IEXEC & gotoEntry(win->filelist, y+win->slide[win->mlevel])->status )
              				filecolor = 2;
            			else
              				filecolor = 1;

						//make row where marker is, white.
						jetilog( 6,"make row where marker is, white\n" );
						if( y+win->slide[win->mlevel] == win->marker[win->mlevel] && win->marker[win->mlevel] != -1 )
            				{
               					filecolor = 1;
                				wattron( win->win, A_REVERSE );
              				}
            			else wattroff( win->win, A_REVERSE );

						//rewind tabs
						while( win->tab->prev != NULL )
							win->tab = win->tab->prev;

    					for( x = 1; x <= (win->w -2) ; x++ )
							{
								//as long as there are files to print
								jetilog( 7,"1" );
								if( y < listsize )
           							{
										//keep printibg while not in next tabfeeld, then shange to next tabfeeld
										jetilog( 7,"2" );
										if( win->tab->next != NULL && x >= win->tab->next->start )
                      						{ win->tab = win->tab->next; endofname = 0; }

										//keep printing while there is something to print, ( check is inside switch/case )
										jetilog( 7,"3" );
										if( win->tab->opt == presentation && printfilename( win->filelist, x - win->tab->start, y + win->slide[win->mlevel] ) == '\0')
               								endofname = 1;

										//--= inside this actual printing is made =-------------------------------------------->
										switch( win->tab->opt )
      										{
          										case selected:
																	jetilog( 7,"S" );
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
																	jetilog( 7,"F" );
																	if( x - win->tab->start >= 0 && x - win->tab->start < strlen( win->filelist->file->d_name ) )
																		{
																			mvwprintw( win->win, y, x, "%c", win->filelist->file->d_name[x - win->tab->start] );
																		}
																	else
																		{
            																mvwprintw( win->win, y, x, " " );
																		}
            											break;

          										case presentation:
																	jetilog( 7,"P" );
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
																	jetilog( 7,"T" );
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
																	jetilog( 7,"M" );
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
																	jetilog( 7,"Z" );
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
            														mvwprintw( win->win, y, x, " " );
            														break;
      									}
									}
								else
									{
										mvwprintw( win->win, y, x, " " );
									}
							}

									endofname = 0;
									win->tab = gettab( win->tab, 1 );
						}
			}

		wrefresh( win->win );
		jetilog( 2, "window printed\n" );
	}

void clearwindow( Windowtype *win )
	{
		int x, 
			y;

		jetilog( 2, "clearing window...\n" );

		for( y = 0; y < win->h; y++ ){
			for( x = 0; x < win->w; x++ ){

				if( win->hiden )
					mvwprintw( win->win, y, x, " " );

				else if( y > 0 && y < win->h-1 && x > 0 && x < win->w-1)
					mvwprintw( win->win, y, x, " " );
				
			}
		}

		jetilog( 2, "window cleard\n" );

	}

int toglehidewin( Windowtype *win )
	{
		jetilog( 3, "hiding window..\n" );
		if( win->hiden )
			win->hiden = 0;

		else
			win->hiden = 1;

		jetilog( 3, "window hidden\n" );
		return win->hiden;
	}

int insidewindow( Windowtype *win, int y, int x )
	{
		int inside = 0;
		
		jetilog( 3, "testing if inside window.\n" );

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

		jetilog( 3, "testing if on shourtcut.\n" );

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
		jetilog( 3, "seeking shourtcut.\n" );

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
		jetilog( 3, "destroying window..\n" );

		win->hiden = 1;
		clearwindow( win );

		clearEnvironment_tabs( win->tab );

		clearEntrys( win->filelist );
		closedir( win->dir );

		delwin( win->win );
			free( win );

		jetilog( 2, "window destroyd\n" );
	}
