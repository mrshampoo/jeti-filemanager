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

static int colors_inited = 0;

void init_colors( Windowtype *win )
	{
		systemlog( 2, "init_colors" );

		if( !colors_inited )
			{
				start_color();
				assume_default_colors( -1, -1 );
			}

		init_pair( 1, win->color[0], win->color[9] );
		init_pair( 2, win->color[1], win->color[9] ); //executebles
		init_pair( 3, win->color[2], win->color[9] ); //katalogs
		init_pair( 4, win->color[3], win->color[9] ); //links 
		init_pair( 5, win->color[4], win->color[9] ); //fifo
		init_pair( 6, win->color[5], win->color[9] ); //char
		init_pair( 7, win->color[6], win->color[9] ); //block 
		init_pair( 8, win->color[7], win->color[9] ); //error 
		init_pair( 9, win->color[8], win->color[9] ); //selected
		init_pair( 10, win->color[10], win->color[9] ); //highlighted
		init_pair( 11, win->color[11], win->color[9] ); //borders
		init_pair( 12, win->color[12], win->color[9] ); //border_text

		systemlog( 92, "\t[DONE]" );
	}

Windowtype *newwindow( int h, int w, int y, int x )
	{
		Windowtype *win;
		int i = 0;
		
		systemlog( 2, "newwindow" );

		win = malloc(sizeof(Windowtype));

			win->win = newwin( h, w, y, x );
				win->h = h;
				win->w = w;
				win->x = x;
				win->y = y;
				win->tab = NULL;
				win->shortcuts = NULL;
				win->filelist = NULL;
				win->orientation_shortcuts = -1;
				win->orientation_workdir = -1;
				win->orientation_filelist = -1;
				win->type_workdir = -1;
				win->show_hidden_files = -1;
				win->wd[0] = '\0';

			win->dir = NULL;

			win->mlevel = 0;	
			win->marker[win->mlevel] = 0;
			win->visible_marker = 0;
			win->slide[win->mlevel] = 0;
			win->hidden = 0;
			win->noexe = 0;

			for( i= 0; i <= NUMBEROFCOLORS; i++ )
				{
					win->color[i] = -2;
				}

			win->type_workdir = -1;
			win->orientation_workdir = -1;
			win->orientation_filelist = -1;
			win->orientation_shortcuts = -1;

		systemlog( 92, "\t[DONE]");
		return win;
	}

Windowtype *redefinewindow( Windowtype *win, int h, int w, int y, int x )
	{
		systemlog( 3, "redefining window" );

		win->hidden = 1;
		clearwindow( win );
		delwin( win->win );
		win->win = newwin( h, w, y, x);
		win->h = h;
		win->w = w;
		win->x = x;
		win->y = y;
		win->hidden = 0;

		put_start_totablist( win->tab, w -2 );

		systemlog( 93, "\t[DONE]" );
		return win;
	}

int loadnewdir( Windowtype *win, char wd[] )
	{
		int success = 0;

		if( !access(wd, R_OK) )
			{
				systemlog( 3, "loadnewdir" );

				//close dir if alredy open
				if( win->dir )
					closedir( win->dir );

				//copy workdir
				if( strncmp( win->wd_history, wd, strlen( wd ) ) )
					{
						strcpy( win->wd_history, wd );
						success |= 2;
					}
				strcpy( win->wd, wd );

				//get files
				win->dir = opendir( wd );
					rget_filelist( win->dir, win->wd, &win->filelist, win->show_hidden_files );

				//if succlessterminal also change the real dir
				if( !strncmp( TERMINALNAME, "st", 2 ) )
					chdir( wd );

				success |= 1;
			}
		else
			{
				success = -1;
				systemlog( 1, "ERROR: loadnewdir: no read access:" );
				systemlog( 91, wd );
			}

		if( success > 0 )
			{
				systemlog( 93, "\t[DONE]");
				systemlog( 94, "("); systemlog( 94, win->wd );systemlog( 94, ")");
			}
		else
			{
				systemlog( 93, "\t[FAILD]" );
				systemlog( 93, "("); systemlog( 93, win->wd ); systemlog( 93, ")");
			}

		return success;
	}

int filesize( int fsize )
	{
		int size = 0;
		int kilo = 0;
		char atribute;
		char string[4];

		systemlog( 4, "filesize");

			size = fsize;

			while( size >= 1000 )
				{
					size /= 1000;
					kilo++;
				}
			switch( kilo )
				{
					case 1: atribute = 'k'; break;
					case 2: atribute = 'M'; break;
					case 3: atribute = 'G'; break;
					case 4: atribute = 'T'; break;
					case 5: atribute = 'Y'; break;
					default: atribute = ' '; break;
				}

			if( 499 < ((fsize - size*kilo) / ( kilo -1 )) )
				size++;

			sprintf( string, "%i%c", size, atribute );

		return size;
	}

int print_win_workdirectory( Windowtype *win, int orientation, int highlight, int procent_spaceing )
	{
		//orientation =>
		// xx00 = random
		// xx01 = right
		// xx10 = left
		// xx11 = center
		// 00xx = top
		// 01xx = right
		// 10xx = left
		// 11xx = bottom

		int success = 0;

		char workdir[SIZE_WORKDIREKTORY];
		int x = 0;
		int y = 0;
		int start = 0;
		int c = 0;
		int spaceing = 0;

		//calculate procentage
		if( (orientation & 8) ^ (orientation & 4) )
			{
				spaceing = procent_spaceing * win->h / 100;
			}
		else
			{
				spaceing = procent_spaceing * win->w / 100;
			}

		//representation form
		if( win->type_workdir & 1 )
			{
				strcpy( workdir, win->wd );
			}
		else
			{
				//get leaf dir
				if ( ( c = strlen( win->wd )-1 ) > 0 )
					{
						if( win->wd[c] == '/' )
							c--;

						while( win->wd[c] != '/' )
							c--;

						strcpy( workdir, win->wd + c +1 );

						if( workdir[ c = strlen(workdir) -1 ] == '/' )
							workdir[c] = '\0';
					}
				else
					{
						workdir[0] = '/'; workdir[1] = '\0';
					}
				c = 0;
			}

		if( highlight )
			wattron( win->win, COLOR_PAIR( 10 ) );
		else
			wattron( win->win, COLOR_PAIR( 12 ) );

		if( (orientation & 3) == 3 )
			{//center centration
				if( (orientation & 12) == 12 || orientation < 4 )
					start = win->w/2 - strlen( workdir )/2;
				else
					start = win->h/2 - strlen( workdir )/2;
			}
		else if( orientation & 2 )
			{//left centration
				start = 3 + spaceing;
			}
		else if( orientation & 1 )
			{//right centration
				if( (orientation & 12) == 12 || orientation < 4 )
					start = win->w -3 -strlen( workdir ) -spaceing;
				else
					start = win->h -3 -strlen( workdir ) -spaceing;
			}
		else
			{
				systemlog( 1, "ERROR: print_win_workdirectory: none text centration!" );
				success = -1;
			}

		while( c <= strlen( workdir ) )
			{
				if( ( orientation & 12 ) == 12 )
					{//botem border
						y = win->h -1;
						x = start + c;
					}
				else if( orientation & 8 )
					{//left border
						x = 0;
						y = start + c;
					}
				else if( orientation & 4 )
					{//right border
						x = win->w -1;
						y = start + c;
					}
				else
					{//top border
						y = 0;
						x = start + c;
					}

				if( (orientation & 12) == 12 || orientation < 4 )
					{
						if( x >= 1 && x < win->w -1  )
							{
								mvwprintw( win->win, y, x, "%c", workdir[c] );
								success = 1;
							}
					}
				else
					{
						if( y >= 1 && y < win->h -1 )
							{
								mvwprintw( win->win, y, x, "%c", workdir[c] );
								success = 1;
							}
					}
				c++;
			}

		win->shortcuts = getshortcut( win->shortcuts, 1 );

		//print brackets and set shortcut- size and cordinats
		if( (orientation & 12 ) == 12 || orientation < 4 )
			{
				wattron( win->win, COLOR_PAIR( 11 ) );
				if( strlen( workdir ) < win->w -6 )
					{
						mvwprintw( win->win, y, start -2, "[ " );
						mvwprintw( win->win, y, x, " ]" );
					}
				else
					{
						mvwprintw( win->win, y, 1, "[ " );
						mvwprintw( win->win, y, win->w -3, " ]" );
					}
				win->shortcuts->x = start;
				win->shortcuts->y = y;
			}
		else
			{
				win->shortcuts->x = x;
				win->shortcuts->y = start;
			}
		wattron( win->win, COLOR_PAIR( 1 ) );

		win->shortcuts->l = strlen( workdir );

		return success;
	}

int print_win_shortcut( Windowtype *win, int num, int orientation, int highlight, int procent_spaceing )
	{
		//orientation =>
		// xx00 = random
		// xx01 = left
		// xx10 = right
		// xx11 = center
		// 00xx = top
		// 01xx = right
		// 10xx = left
		// 11xx = bottom
		
		int success = 0;

		int x = 0;
		int y = 0;
		int start = 0;
		int c = 0;
		int d = 0;
		int spaceing = 0;
		char status[30];

		if( win->shortcuts->n != num )
			win->shortcuts = getshortcut( win->shortcuts, num );

		if( (orientation & 8) ^ (orientation & 4) )
			{
				spaceing = procent_spaceing * win->h / 100;
			}
		else
			{
				spaceing = procent_spaceing * win->w / 100;
			}


		if( win->shortcuts != NULL && win->shortcuts->label[0] != '-' ) //dont do a shit if its the none-shortcut shortcut
			{
				d = c;

				if( highlight || win->shortcuts->highlighted )
					wattron( win->win, COLOR_PAIR( 10 ) );
				else
					wattron( win->win, COLOR_PAIR( 12 ) );

				while( c <= strlen( win->shortcuts->label ) )
					{
						if( (orientation & 3) == 3 )
							{//center centration
								systemlog( 1, "WARNING: print_win_shortcut: centration center is still under construction!" );
								success = -1;
							/*
								if( orientation & 12 || orientation < 4 )
									start = win->w/2 - strlen( win->shortcuts->label ) - num*SHORTCUTLABEL_LENGTH/2;
								else
									start = win->h/2 - strlen( win->shortcuts->label ) - num*SHORTCUTLABEL_LENGTH/2;
							*/
							}
						else if( orientation & 2 )
							{//left centration
								start = 1 + (num-1)*SHORTCUTLABEL_LENGTH + spaceing;
							}
						else if( orientation & 1 )
							{//right centration
								if( ( orientation & 12 ) == 12 || orientation < 4 )
									start = win->w -1 -win->shortcuts->l -(num-1)*SHORTCUTLABEL_LENGTH  -spaceing;
								else
									start = win->h -1 -win->shortcuts->l -(num-1)*SHORTCUTLABEL_LENGTH  -spaceing;
							}
						else
							{
								systemlog( 1, "ERROR: print_win_workdirectory: none text centration!" );
								success = -1;
							}

						if( ( orientation & 12 ) == 12 )
							{//botem border
								y = win->h -1;
								x = start + c;
							}
						else if( orientation & 8 )
							{//left border
								x = 0;
								y = start + c;
							}
						else if( orientation & 4 )
							{//right border
								x = win->w -1;
								y = start + c;
							}
						else
							{//top border
								y = 0;
								x = start + c;
							}

						if( x >= 0 && x < win->w && y >= 0 && y < win->h )
							{
								mvwprintw( win->win, y, x, "%c", win->shortcuts->label[c] );
								success = 1;
							}
						c++;
					}

				//define position if the past position ain't the same as the present
				if( win->shortcuts->x != start +d && ( ( orientation & 12 ) == 12 || orientation < 4 ) )
					{
						win->shortcuts->x = start +d;
						win->shortcuts->y = y;
						systemlog( 4, "horizontal" );
					}
				else if( win->shortcuts->y != start +d && ((orientation & 12) == 4 || (orientation & 12) == 8) )
					{
						win->shortcuts->x = x;
						win->shortcuts->y = start +d;
						systemlog( 4, "vertikal");
					}
				sprintf( status, "%d shortcut number:%d, x:%d:, Y:%d, l:%d", orientation, win->shortcuts->n, win->shortcuts->x, win->shortcuts->y, win->shortcuts->l );
				systemlog( 4, status );

				if( highlight )
					wattroff( win->win, COLOR_PAIR( 2 ) );
			}

		return success;
	}

int print_win_filetabs_line( Windowtype *win, int num, int orientation, int highlight, int procent_spaceing )
	{
		//orientation =>
		// 00 = down -> up
		// 01 =	left -> right
		// 10 = right -> left
		// 11 = up -> down

		int success = 0;
		int x = 1;
		int y = 0;
		int c = 0;
		int start = 0;
		int endofstring = 1;
		char content[ PRESENTATION_LENGTH ];
		int file_color = 1;
		int not_printed = 1;
		int spaceing = procent_spaceing * ( win->w -2 ) / 100;

		//calculate spaceing
		if( (orientation & 1)  ^ (orientation & 2) )
			spaceing = procent_spaceing * ( win->w -2 ) / 100;
		else
			spaceing = procent_spaceing * ( win->h -2 ) / 100;

		//rewind stuff
		while( win->tab->prev != NULL )
			win->tab = win->tab->prev;

		if( win->filelist->number != num )
			win->filelist = gotoEntry( win->filelist, num );

		//let there be colors
		if( win->marker[win->mlevel] == num && win->visible_marker )
			{
				wattron( win->win, A_REVERSE );
				file_color = 1;
			}
		else if( highlight )
			file_color = 10;
		else if( win->filelist->selected )
			file_color = 9;
		else if( isoffiletype( win->filelist, num, "ERROR") || isoffiletype( win->filelist, num, "linkER" ) )
			file_color = 8;
		else if( isoffiletype( win->filelist, num, "block") )
			file_color = 7;
		else if( isoffiletype( win->filelist, num, "char") )
			file_color = 6;
		else if( isoffiletype( win->filelist, num, "fifo") )
			file_color = 5;
		else if( isoffiletype( win->filelist, num, "link") || isoffiletype( win->filelist, num, "<DIR L>") )
			file_color = 4;
		else if( isoffiletype( win->filelist, num, "<DIR>") )
			file_color = 3;
		else if( S_IEXEC & win->filelist->status )
			file_color = 2;
		else
			file_color = 1;

		//determen placement
		if( ( orientation & 3 ) == 3 )
			{
				if( MOVEMENT >= 0 )
					y = win->filelist->number -win->slide[ win->mlevel ] -spaceing;
				else
					{
						y = win->filelist->number -win->slide[ win->mlevel ] +spaceing;
						c = 0;
					}
			}
		else if( orientation & 2 )
			{
				y = win->filelist->number -win->slide[ win->mlevel ];
				if( MOVEMENT >= 0 )
					c = 0 +spaceing;
				else
					c = 0 -spaceing;
			}
		else if( orientation & 1)
			{
				y = win->filelist->number -win->slide[ win->mlevel ];
				if( MOVEMENT >= 0 )
					c = 0 -spaceing;
				else
					c = 0 +spaceing;
			}
		else
			{
				if( procent_spaceing >= 0 )
					y = win->filelist->number -win->slide[ win->mlevel ] +spaceing;
				else
					y = win->filelist->number -win->slide[ win->mlevel ] -spaceing;
				c = 0;
			}

		//print stuff
		while( x < win->w -1  && y > 0 && y < win->h -1 )
			{
				//-- prepare tab_content for printing
				if( not_printed || ( win->tab->next != NULL && win->tab->next->start == c ) )
					{
						while( win->tab->next != NULL && win->tab->start < c && win->tab->next->start <= c )
							win->tab = win->tab->next;

						endofstring = 0;
						wattron( win->win, COLOR_PAIR( 1 ) );
						switch( win->tab->opt )
							{
								case nothing : 
									wattron( win->win, COLOR_PAIR( 1 ) );
									break;

								case selected :
									if( win->filelist->selected )
										{
											strcpy( content, "*" );
										}
									else
										strcpy( content, " " );

									wattron( win->win, COLOR_PAIR( file_color ) );
									break;

								case filename : 
									strcpy( content, win->filelist->filename );
									wattron( win->win, COLOR_PAIR( file_color ) );
									break;

								case presentation : 
									strcpy( content, win->filelist->presentation );
									wattron( win->win, COLOR_PAIR( file_color ) );
									break;

								case filetype :
									strcpy( content, win->filelist->filetype );
									wattron( win->win, COLOR_PAIR( file_color ) );
									break;

								case mode :
									if( S_ISDIR( win->filelist->status ) ) { strcpy( content, "d" ); } else { strcpy( content, "-" ); }
									if( win->filelist->status & S_IRUSR ) { strcat( content, "r" ); } else { strcat( content, "-" ); }
									if( win->filelist->status & S_IWUSR ) { strcat( content, "w" ); } else { strcat( content, "-" ); }
									if( win->filelist->status & S_IXUSR ) { strcat( content, "x" ); } else { strcat( content, "-" ); }
									if( win->filelist->status & S_IRGRP ) { strcat( content, "r" ); } else { strcat( content, "-" ); }
									if( win->filelist->status & S_IWGRP ) { strcat( content, "w" ); } else { strcat( content, "-" ); }
									if( win->filelist->status & S_IXGRP ) { strcat( content, "x" ); } else { strcat( content, "-" ); }
									if( win->filelist->status & S_IROTH ) { strcat( content, "r" ); } else { strcat( content, "-" ); }
									if( win->filelist->status & S_IWOTH ) { strcat( content, "w" ); } else { strcat( content, "-" ); }
									if( win->filelist->status & S_IXOTH ) { strcat( content, "x" ); } else { strcat( content, "-" ); }

									if( highlight )
										wattron( win->win, COLOR_PAIR( file_color ) );
									break;

								case size :
									sprintf( content, "%db", win->filelist->filesize  );

									if( highlight )
										wattron( win->win, COLOR_PAIR( file_color ) );
									break;

								default : break;
							}

						if( win->tab->centration < 0 ) 
							{//left
								start = win->tab->start;
							}
						else if( win->tab->centration > 0 && win->tab->next != NULL )
							{//right
								if( win->tab->start > (start = c +( win->tab->next->start -win->tab->start ) -strlen( content ) ) )
									{
										start = win->tab->start;
									}
							}
						else if( win->tab->centration > 0 )
							{ //right-centration for endtab
								if( win->tab->start > (start = c +( win->w -2 -win->tab->start ) -strlen( content ) ) )
									{
										start = win->tab->start;
									}
							}
						else if( win->tab->centration == 0 && win->tab->next != NULL )
							{//center
								if( win->tab->start > ( start = c +( win->tab->next->start -win->tab->start )/2 -strlen( content )/2 ) )
									{
										start = win->tab->start;
									}
							}
						else
							{//center-centration for endtab
								if( win->tab->start > ( start = c +( win->w -2 -win->tab->start )/2 -strlen( content )/2 ) )
									{
										start = win->tab->start;
									}
							}

					}

				//-- printing happens here ------------->
				if( x > 0 )
					{
						if( c < start || c - start > strlen( content ) || endofstring || win->tab->opt == nothing )
							{
								mvwprintw( win->win, y, x, " " );
							}
						else
							{
								if( content[c-start] == '\0' )
									{
										endofstring = 1;
										mvwprintw( win->win, y, x, " " );
									}
								else
									mvwprintw( win->win, y, x, "%c", content[c-start] );
							}
					}

				//count upp
				x++;
				c++;

				if( not_printed )
					not_printed = 0;
			}

		wattroff( win->win, A_REVERSE );
		wattron( win->win, COLOR_PAIR(1) );

		return success;
	}

void printwindow( Windowtype *win, int movement, int activ )
	{
		int decoration_movement = 0;

		systemlog( 2, "printwindow" );

		if( movement < 0 )
			movement = -movement;

		if( !win->hidden )
			{
				
				//reset stuff
				wattroff( win->win, A_REVERSE );
				clear();
				if( STARTUP )
					{
						if( ANIMATING && movement )
							decoration_movement = movement;
						else
							{
								STARTUP = 0;
								MOVEMENT = 100;
							}
					}

				//window decorations
				wattron( win->win, COLOR_PAIR( 11 ) );
				box( win->win, 0, 0 );
				wattron( win->win, COLOR_PAIR(1) );
				print_win_workdirectory( win, win->orientation_workdir, activ, decoration_movement );

				//decorate with shortcuts
				systemlog( 4,"decorate with shortcuts" );
				win->shortcuts = getshortcut( win->shortcuts, 2 );
				while( win->shortcuts->next != NULL )
					{
						print_win_shortcut( win, win->shortcuts->n, win->orientation_shortcuts, 0, decoration_movement );
						win->shortcuts = win->shortcuts->next;
					}
					print_win_shortcut( win, win->shortcuts->n, win->orientation_shortcuts, 0, decoration_movement );

				if( STARTUP == 0 )
				{
				//print files, ALL of them. (only the once in visible range vill be seen)
				while( win->filelist->prev != NULL && win->filelist->number > win->slide[win->mlevel] )
						win->filelist = win->filelist->prev;
				while( win->filelist->next != NULL && win->filelist->number - win->slide[win->mlevel] < win->h -2 )
					{
						print_win_filetabs_line( win, win->filelist->number, win->orientation_filelist, 0, movement );
						win->filelist = win->filelist->next;
					}
					print_win_filetabs_line( win, win->filelist->number, win->orientation_filelist, 0, movement );
				}
			}

		wrefresh( win->win );
		systemlog( 92, "\t[DONE]" );
	}

void clearwindow( Windowtype *win )
	{
		int x, 
			y;

		systemlog( 2, "clearwindow" );

		for( y = 0; y < win->h; y++ ){
			for( x = 0; x < win->w; x++ ){

				if( win->hidden )
					mvwprintw( win->win, y, x, " " );

				else if( y > 0 && y < win->h-1 && x > 0 && x < win->w-1)
					mvwprintw( win->win, y, x, " " );
				
			}
		}

		systemlog( 92, "\t[DONE]" );

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

			if( (x > win->x) && (x < win->x + win->w -1)
			 && (y > win->y) && (y < win->y + win->h -1) )
				{
					inside = 1;
				}

		return inside;
	}

int onshortcut( Windowtype *win, int y, int x )
	{
		int shortcut_num = 0;
		char status[ 30 ];

		systemlog( 3, "testing if onshortcut" );

		win->shortcuts = getshortcut( win->shortcuts, 1 );	

			while( 1 )
				{
					if( ( win->orientation_shortcuts < 4 || ( win->orientation_shortcuts & 12 ) == 12 ) && y == win->shortcuts->y + win->y && x >= win->shortcuts->x + win->x && x <= win->shortcuts->x + win->shortcuts->l + win->x )
						{
							shortcut_num = win->shortcuts->n;
							break;
						}
					else if( (( win->orientation_shortcuts & 4 ) || ( win->orientation_shortcuts & 8 ) ) && ( ( win->orientation_shortcuts & 12 ) != 12 ) && y >= win->shortcuts->y + win->y && y <= win->shortcuts->y + win->shortcuts->l + win->y && x == win->shortcuts->x + win->x )
						{
							shortcut_num = win->shortcuts->n;
							break;
						}
					else if( win->shortcuts->next != NULL )
						{
							win->shortcuts = win->shortcuts->next;
						}
					else
						{
							break;
						}
				}

		if( shortcut_num )
			{
				//sprintf( status, "\t[ success ] (shortcuts_num:%d, clickY:%d, clickX:%d)", shortcut_num, y, x );
				sprintf( status, "\t[ success ]");
				systemlog( 93, status );
			}
		else
			{
				systemlog( 94,"\t[FAILD]" );
			}
		return shortcut_num;
	}

shortcutType *getshortcut( shortcutType *shorty, int pos )
	{
		systemlog( 3, "seeking shourtcut." );

		while( shorty->prev != NULL )
			{
				shorty = shorty->prev;
			}

		if( shorty->n > pos )
			systemlog( 1, "ERROR: getshortcut: the shortcut you are looking for is smaler than the lists minimum" );

		while( shorty->next != NULL && shorty->n != pos )
			{
				shorty = shorty->next;
			}

		if( shorty->next == NULL && shorty->n != pos )
			systemlog( 1, "ERROR: getshortcut: the shortcut number could not be found");

		return shorty;
	}

void destroywindow( Windowtype *win ) 
	{
		systemlog( 2, "destroying window.." );

		win->hidden = 1;
		clearwindow( win );

		clearEnvironment_tabs( &win->tab );

		clearEntrys( &win->filelist );
		closedir( win->dir );

		delwin( win->win );
			free( win );

		systemlog( 2, "window destroyd" );
	}
