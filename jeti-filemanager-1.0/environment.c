/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= environment.h =--------*/
#include "projecttypes.h"
#include "window.h"
#include "environment.h"
#include "systemlog.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static char CONFIGNAME[250];

int get_configname()
	{
		int c = 0;
		int exist = 1;
		char buff[250];
		FILE *fp;

		jetilog( 2, "try to locate configfiles\n" );

		fp = popen( "echo $XDG_CONFIG_HOME/jeti.rc", "r" );
		while( ( buff[c] = fgetc( fp )) != EOF ){ c++; }
			buff[c-1] = '\0';

		if( !access( buff, R_OK ) )
			{
				jetilog( 5, "access to " ); jetilog( 5, buff );
            	strcpy( CONFIGNAME, buff );
			}
        else
			{
				c = 0;
				fp = popen( "echo $HOME/.jeti.rc;", "r" );
				while( ( buff[c] = fgetc( fp )) != EOF ){ c++; } 
					buff[c-1] = '\0';

				if( !access( buff, R_OK ) )
					{
						jetilog( 5, "access to " ); jetilog( 5, buff ); jetilog( 5, " " );
            			strcpy( CONFIGNAME, buff );
						printf( CONFIGNAME );
					}
				else
            		{
						c = 0;
                		fp = popen( "echo $XDG_DATA_DIRS/jeti/jeti.rc;", "r" );
                		while( ( buff[c] = fgetc( fp )) != EOF ){ c++; }
                    		buff[c-1] = '\0';

                		if( !access( buff, R_OK ) )
                    		{
                        		jetilog( 5, "access to " ); jetilog( 5, buff ); jetilog( 5, " " );
                        		strcpy( CONFIGNAME, buff );
                        		printf( CONFIGNAME );
                    		}
                		else
                    		{ 
                				jetilog( 1, "ERROR: there are no config files!!\n" );
                				exist = 0;
							}
            		}
				
			}	

		pclose( fp );

        return exist;
	}

int set_configname( char path[] )
	{
		int exist = 1;

		if( !access( path, R_OK ) )
			{
				strcpy( CONFIGNAME, path );
			}
		else
			{
				jetilog( 1, "WARNING: cant open config, looking for fallback config\n" );
				exist = 0;
			}

		return exist;
	}

Windowtype *init_win_colors( Windowtype *win )
	{
		int c;

		char buff[250];
		FILE *fp;

		jetilog( 2, "trying to initiate colors...\n" );

		//fallback values
		win->color[0] = -1;
		for( c = 1; c < 9; c++ )
      		win->color[c] = win->color[0];

		if( !access( CONFIGNAME, R_OK ) )
			{
				fp = fopen( CONFIGNAME, "rb");
			c = 0;
			while( (buff[c] = fgetc(fp)) != EOF )
			{
				if( buff[c] == '\n' )
					{
						while( buff[c] == ' ' ) /*remove blankspace in the beginning*/
							{ strcpy( buff, buff+1 ); }
	
						if( !strncmp( buff, "window.", 7 ) )
							{
								if( !strncmp( buff+7, "color.", 6 ) )
									{
										if( !strncmp( buff+13, "default: ", 9 ) )   
                      						{ win->color[0] = (int) strtol( buff+22, NULL, 10 ); }

										else if( !strncmp( buff+13, "executeble: ", 12 ) )
											{ win->color[1] = (int) strtol( buff+25, NULL, 10 ); }

										else if( !strncmp( buff+13, "katalog: ", 9 ) )
											{ win->color[2] = (int) strtol( buff+22, NULL, 10 ); }

										else if( !strncmp( buff+13, "link: ", 6 ) )   
                      						{ win->color[3] = (int) strtol( buff+19, NULL, 10 ); }

										else if( !strncmp( buff+13, "fifo: ", 6 ) )   
                    						{ win->color[4] = (int) strtol( buff+19, NULL, 10 ); }

										else if( !strncmp( buff+13, "char: ", 6 ) )   
                      						{ win->color[5] = (int) strtol( buff+19, NULL, 10 ); }

										else if( !strncmp( buff+13, "block: ", 7 ) )   
                      						{ win->color[6] = (int) strtol( buff+20, NULL, 10 ); }

										else if( !strncmp( buff+13, "error: ", 7 ) )   
                      						{ win->color[7] = (int) strtol( buff+20, NULL, 10 ); }

										else if( !strncmp( buff+13, "selected: ", 10 ) )   
                      						{ win->color[8] = (int) strtol( buff+23, NULL, 10 ); }
									}

							}	
						c = 0;
					}
				else
					c++;
			}
				fclose( fp );
			}
		else
			{
				//no config
			}

		jetilog( 2, "colors initiated\n");
		return win;
	}

tabtype *gettab( tabtype *tab, int n )
	{
		jetilog( 3, "enterd gettab..\n" );

		while( tab->prev != NULL && tab->n != n )
			tab = tab->prev;

		while( tab->next != NULL && tab->n != n )
			tab = tab->next;

		if( tab->next == NULL && tab->n != n )
			jetilog( 1, "ERROR: gettab: could not find tab\n" );

		return tab;
	}

tabtype *sorttabs( tabtype *tab, int wide )
	{
		int bubbled = 1;
		tabtype *temp;

		char strlog[10];

		jetilog( 3, "enterd sorttabs..\n" );
		
		jetilog( 4, "rewinds tabs.\n" );	
		while( tab->prev != NULL )
			tab = tab->prev;

		while( bubbled || tab->next != NULL )
			{
				//has ben rewined?
				if( tab->prev == NULL )
					{
						jetilog( 4, "starts over.\n" );
						bubbled = 0;
					}

				//error testing
				if( tab->next != NULL && tab->start == tab->next->start )
					jetilog( 1, "ERROR: tabs have same starting position\n" );

				//found some thing in the wrong end
				while( tab->next != NULL && tab->start > tab->next->start )
					{
						jetilog( 4, "sorting a tab." );
							sprintf( strlog, "(%i)", tab->n );
                            jetilog( 4, strlog );

						temp = tab->next;
						jetilog( 5, "temp = tab->next;" );

						tab->next = temp->next;
						temp->prev = tab->prev;
						jetilog( 5, "temp->prev = tab->prev;" );
					
						temp->prev->next = temp;
						if( tab->next != NULL )
							tab->next->prev = tab;
						jetilog( 5, "tab->next->prev = tab;" );

						tab->prev = temp;
						temp->next = tab;

						//temp = NULL;
						bubbled++;
						jetilog( 5, "sorting succes!.\n" );
					}

				//moves along in the tabquee
				if( tab->next == NULL && bubbled )
					{
						//rewind
						jetilog( 4, "rewind inside bubblesort.\n" );
						while( tab->prev != NULL )
							tab = tab->prev;
					}
				else if( tab->next != NULL )
                    {
                        jetilog( 4, "moves forward." );
                            sprintf( strlog, "(%i)\n", tab->n );
                            jetilog( 4, strlog );

                        tab = tab->next;
                    }
			}

		return tab;
	}

tabtype *init_wintabs_environment( tabtype *tab, int wide )
    {
        int left[10];
        int l = -1;

		int rol = 0; //right or left

        int right[10];
        int r = -1;

        int c = 0;
        int d = 0;
        char buff[250];
        FILE *fp;

		tab = NULL;

		jetilog( 2, "trying to initiate tabfeelds...\n" );

		if( !access( CONFIGNAME, R_OK ) )
            {
        		fp = fopen( CONFIGNAME, "rb" );
			jetilog( 4, "rc file opend\n" );

            while( (buff[c] = fgetc(fp) ) != EOF )
                {
                    if( buff[c] == '\n' )
                        {
							jetilog( 6, "EOL, ");

                            while( buff[0] == ' ' )
								{
                                	strcpy( buff, buff+1 );
									jetilog( 4, "removed blank, ");
								}

                            if( !strncmp( buff, "window.tab.", 11 ) )
                                {d+=11;

									jetilog( 4, "window.tab. found\n");

									//create a new tab
									rol = 0;
									if( tab == NULL )
										{
											tab = malloc( sizeof(tabtype) );
        									tab->n = 0;
        									tab->opt = nothing;
											tab->start = 0;
											tab->length = 0;
        									tab->next = NULL;
        									tab->prev = NULL;
										}
									else
										{
											tab->next = malloc( sizeof(tabtype) );
											tab->next->next = NULL;
											tab->next->prev = tab;
											tab = tab->next;
											tab->n = tab->prev->n +1;
											tab->opt = nothing;
											tab->start = 0;
											tab->length = 0;
										}
									jetilog( 4, "new tab malloced\n");

                                    if( !strncmp( buff+d, "left: ", 6 ) )
                                        {  d+=6; rol = -1; l++; left[l] = tab->n; }
									else if( !strncmp( buff+d, "right: ", 7 ) )
                                        {  d+=7; rol = 1; r++; right[r] = tab->n; }
									else
										{ /*ERROR no tab-direction*/ 
											jetilog( 1, "ERROR: no tab direction\n" );  
										}

                                            //get length
                                            if( !strncmp( buff+d, "selected", 8 ) )
                                                {  d+=8;
													tab->opt = selected; 
													tab->length = 1;
													jetilog( 4, "selected length found, ");
												}
                                            else if( !strncmp( buff+d, "filename", 8) )
                                                {  d+=8;
                                                    tab->opt = filename;
                                                    tab->length = PRESENTATION_LENGTH;
													jetilog( 4, "filename length found, ");
                                                }

                                            else if( !strncmp( buff+d, "presentation", 12) )
                                                {  d+=12;
                                                    tab->opt = presentation;
                                                    tab->length = PRESENTATION_LENGTH;
													jetilog( 4, "presentation length found, ");
                                                }
                                            else if( !strncmp( buff+d, "filetype", 8 ) )
                                                {  d+=8; 
													tab->opt = filetype; 
													tab->length = TYPE_LENGTH;
													jetilog( 4, "filetype length found, ");
												}
                                            else if( !strncmp( buff+d, "size", 4 ) )
                                                {  d+=4; 
													tab->opt = size; 
													tab->length = SIZE_SIZE; 
													jetilog( 4, "size length found, ");
												}
											else if( !strncmp( buff+d, "mode", 4 ) )
                                                {  d+=4;
                                                    tab->opt = mode;
                                                    tab->length = 10;
                                                    jetilog( 4, "mode length found, ");
                                                }
 											else if( !strncmp( buff+d, "nothing", 7 ) )
                                                {  d+=7; 
													tab->opt = nothing; 
													tab->length = 1; 
													jetilog( 4, "nothing length found, ");
												}
                                            else 
												{ /*ERROR tab length*/ 
													tab->opt = nothing; tab->length = 1;
													jetilog( 1, "ERROR: bad tab length, " ); 
												}

                                            //find start left
											if( rol < 0 )
												{
													jetilog( 4, "is left centerd\n" );
                                            		if( l > 0 )
                                                		tab->start = gettab( tab, left[l-1] )->start + gettab( tab, left[l-1] )->length +1;
                                            		else
                                                		tab->start = 1;
												}

                                            //find start right
											else if( rol > 0 )
												{
													jetilog( 4, "is right centerd\n" );
                                            		if( r > 0 )
                                                		tab->start = gettab( tab, right[r-1] )->start - tab->length -1;
                                            		else
                                                		tab->start = wide - tab->length -2 ;
												}
										
                                }
                            d = 0;
							c = 0;
                        }
                    else
                        {
                            c++;
                        }
                }

        		fclose( fp );
				jetilog( 4, "rc file closed\n" );
			}
		else
			{
				//no config
			}

        if( l == -1 && r == -1 )
            {//ERROR no tabs read, faling back to defaults
				jetilog( 1, "WARNING: no tabs read, faling back to defaults\n" );

                tab = malloc( sizeof(tabtype) );
				tab->n = 0;
                tab->start = 1;
                tab->length = 1;
                tab->opt = selected;

				tab->next = malloc( sizeof(tabtype) );
				tab->next->n = 1;
                tab->next->start = tab->start + tab->length +1;
                tab->next->length = PRESENTATION_LENGTH;
                tab->next->opt = filename;
				tab->next->prev = tab;
            }

		tab = sorttabs( tab, wide );

		jetilog( 2, "tabfeelds initiated\n");
        return tab;
    }

Windowtype *init_workdir( Windowtype *win )
	{
		int b = 1;
		int c = 0;
		int d = 0;

		char cwd[1024];
		char buff[250];
		FILE *fp;

		jetilog( 2, "initing workdir\n");

		//fallback values:
		win->showhidden = 0;
		strcpy( win->wd, "/" );

		 if( !access( CONFIGNAME, R_OK ) )
            {
				fp = fopen( CONFIGNAME, "rb");

			while( (buff[c] = fgetc(fp)) != EOF )
      			{
        			if( buff[c] == '\n' )
          				{
            				while( buff[0] == ' ' ) /*remove blankspace in the beginning*/
              					{ strcpy( buff, buff+1 ); }

                        	if( !strncmp( buff, "window.startdirectory: ", 23 ) )
              					{ d+=23;
									if( buff[d] == '\"' )
										{ 
											while( buff[d+b] != '\"' )
												{
													//copy content
													win->wd[b-1] = buff[d+b];
													b++;
												}
										}
									else if( !strncmp( buff+d, "cwd", 3 ) )
										{
											if( !getcwd(cwd, sizeof(cwd)) )
												jetilog( 1, "ERROR: getcwd()");
											strcpy( win->wd, cwd );
										}
									else if( buff[d] != '\n' )
										d++;
								}

							if( !strncmp( buff, "window.showhiden: ", 18 ) )
                                { d+=18;
									win->showhidden = (int) strtol( buff+d, NULL, 10 );
                                }

							d = 0;
							c = 0;
                    		b = 1;
						}
					else
						c++;
				}

				fclose( fp );
			}
		else
			{
				//no config
			}

		jetilog( 2, "workdir initiated\n");
		return win;
	}

filetypeAction *init_fileAction()
	{

		filetypeAction *action;
		char command[COMMANDLENGTH];


		int b = 1, /*läser*/
				c = 0, /*fullrad*/
				d = 1; /*läst*/

		char buff[250];
    	FILE *fp;

		jetilog( 2, "trying to initiate fileActions...\n" );

		action = malloc( sizeof(filetypeAction) );
			strcpy( action->Action, " " );
			action->next = malloc( sizeof(filetypeAction) );;
			action->next->next = NULL;
			action->next->prev = action;
			action->prev = NULL;

		if( !access( CONFIGNAME, R_OK ) )
            {
				fp = fopen( CONFIGNAME, "rb");

      while( (buff[c] = fgetc(fp)) != EOF )
      {
        if( buff[c] == '\n' )
          {
            while( buff[0] == ' ' ) /*remove blankspace in the beginning*/
              { strcpy( buff, buff+1 ); }
					
						if( !strncmp( buff, "file.type: ", 11 ) )
              { d+=10;
 
								while( action->next != NULL )
									{
										action = action->next;
									}

								while( buff[d] != '\"' )
									d++;
															
								memset(command,0,sizeof(command)); /*empty cmommand*/								

								while( buff[d+b] != '\"' )
									{ command[b-1] = buff[d+b]; b++;  }

								strcpy( action->Action, command );

								d += b + 1; 
								b = 1;

								while( buff[d+b-1] != '\n' )
									{
										if( buff[d+b] != '\n' )
											{
												action->filetype[b-1] = buff[d+b];
											}

										b++;											

										if( buff[d+b] == ',' || buff[d+b] == '\n' )
											{
												action->next = malloc( sizeof(filetypeAction) );
                				action->next->next = NULL;
                				action->next->prev = action;
               	 				action = action->next;
												strcpy( action->Action, command );
												d += b;     
                				b = 1;
											}
									}
              }

						c = 0;
						d = 1;
					}
				else
					c++;
			}

				fclose( fp );
			}
		else
			{
				//no config
			}

		jetilog( 2, "fileAction initiated\n");
		return action;
	}

shortcutType *init_shortcuts()
	{
		shortcutType *shortCuts;
		//shortCuts = NULL;

		char buff[250];
    	FILE *fp;
		int b = 1; //läser
		int c = 0; //läst rad
		int d = 0; //lästa tecken
		int y = 1; //placement

		jetilog( 2, "trying to initiate shortcuts...\n" );

		shortCuts = malloc( sizeof(shortcutType) );
		shortCuts->prev = NULL;
		shortCuts->next = NULL;
		shortCuts->x = 0;
		shortCuts->y = 1;
		shortCuts->label = '-';
		shortCuts->dir[0]= '_';

		if( !access( CONFIGNAME, R_OK ) )
            {
				fp = fopen( CONFIGNAME, "rb");
			while( (buff[c] = fgetc(fp)) != EOF )
     		{
        	if( buff[c] == '\n' )
          	{
				 //remove blankspace in the beginning
				while( buff[0] == ' ' )
              		{ strcpy( buff, buff+1 ); }

				if( !strncmp( buff, "window.shortcut: ", 17 ) )
				   { d+=17;
						while( buff[d] != '\'' )
                  			d++;

            			shortCuts->label = buff[d+1];
						d++;

            			while( buff[d] != '\"' )
                			d++;

            			while( buff[d+b] != '\"' )
										{	
											shortCuts->dir[b-1] = buff[d+b];
											b++;
										}

						//prepare for new shortcut
                            shortCuts->next = malloc( sizeof(shortcutType) );
                            shortCuts->next->prev = shortCuts;
                            shortCuts->next->next = NULL;
                            shortCuts = shortCuts->next;
                            shortCuts->x = 0;
							y++;
                            shortCuts->y = y;
                            shortCuts->label = '-';

					}	

							//rewind readers
							d = 0;
							b = 1;
							c = 0;

				}

					else c++;
				}

				fclose( fp );
			}
		else
			{
				//no config
			}

		if( shortCuts->prev != NULL )
			{
				shortCuts = shortCuts->prev;
				free( shortCuts->next );
				shortCuts->next = NULL; 
			}

		jetilog( 2, "shortcuts initiated\n");
		return shortCuts;
	}
soundeffectType *init_soundeffects()
	{
		soundeffectType *sounds;
		//sounds = NULL;

		char buff[250];
    	FILE *fp;
		int b = 1;
		int c = 0;
		int d = 0;

		jetilog( 2, "trying to initiate soundeffects...\n" );

		sounds = malloc( sizeof( soundeffectType ) );
		sounds->prev = NULL;
		sounds->next = NULL;
		sounds->action = -1;

		if( !access( CONFIGNAME, R_OK ) )
            {
				fp = fopen( CONFIGNAME, "rb");
			while( (buff[c] = fgetc(fp)) != EOF )
      			{
        			if( buff[c] == '\n' )
         	 			{
							while( buff[0] == ' ' ) //remove blankspace in the beginning
              					{ strcpy( buff, buff+1 ); }

							if( !strncmp( buff, "sound.", 6 ) )
              					{ d+=6;
									if( !strncmp( buff+d, "enter.", 6 ) )
              							{ d+=6;
											if( !strncmp( buff+d, "doubledot: ", 10 ) )
              									{ d+=10; sounds->action = 1; }
											else if( !strncmp( buff+d, "singledot: ", 11 ) )
                      							{ d+=11; sounds->action = 2;}
											else if( !strncmp( buff+d, "kat: ", 5 ) )
												{ d+=5; sounds->action = 3; }
											else if( !strncmp( buff+d, "executeble: ", 12 ) )
                      							{ d+=12; sounds->action = 4; }
											else if( !strncmp( buff+d, "scrollupp: ", 11 ) )
                                                { d+=11; sounds->action = 5; }
											else if( !strncmp( buff+d, "scrolldown: ", 12 ) )
                                                { d+=12; sounds->action = 6; }
											else if( !strncmp( buff+d, "stepupp: ", 9 ) )
                                                { d+=9; sounds->action = 7; }
											else if( !strncmp( buff+d, "stepdown: ", 10 ) )
                                                { d+=10; sounds->action = 8; }
											else if( !strncmp( buff+d, "selectfile: ", 12 ) )
                                                { d+=12; sounds->action = 9; }
											else if( !strncmp( buff+d, "copyfiles: ", 11 ) )
                                                { d+=11; sounds->action = 10; }
											else if( !strncmp( buff+d, "movefiles: ", 11 ) )
                                                { d+=11; sounds->action = 11; }
											else if( !strncmp( buff+d, "removefiles: ", 13 ) )
                                                { d+=13; sounds->action = 12; }
											else if( !strncmp( buff+d, "shortcut: ", 10 ) )
                                                { d+=10; sounds->action = 13; }
												
											while( buff[d] != '\"' )
												d++;
										
											while( buff[d+b] != '\"' )
												{
													sounds->sound[b-1] = buff[d+b];
													b++;
												}
										}
              					}

							sounds->next = malloc( sizeof(soundeffectType) );
							sounds->next->next = NULL;
							sounds->next->prev = sounds;
							sounds = sounds->next;
							sounds->action = -1;

							b = 1;
                            d = 0;
                            c = 0;
						}
        			else 
						{
							c++;
						}
      			}
    			fclose( fp );
			}
		else
			{
				//no config
			}

		//sounds = sounds->prev;
		//free( sounds->next );
		//sounds->next = NULL;

		jetilog( 2, "soundeffects initiated\n");
		return sounds;
	}

void clearEnvironment_tabs( tabtype *tabs )
	{
		jetilog( 3, "clearing tabs..\n");

		while( tabs->next != NULL )
			tabs = tabs->next;
		while( tabs->prev != NULL )
			{
				tabs = tabs->prev;
				free( tabs->next );
				tabs->next = NULL;
			}

		free( tabs );

		jetilog( 2, "tabs cleard\n");
	}

void clearEnvironment_actions( filetypeAction *actions )
	{
		jetilog( 3, "clearing actions..\n");

		while( actions->next != NULL )
				actions = actions->next;

		while( actions->prev != NULL )
			{
				actions = actions->prev;
				free( actions->next );
				actions->next = NULL;
			}

		free( actions );

		jetilog( 2, "actions cleard\n");
	}

void clearEnvironment_shortcuts( shortcutType *shortCuts )
	{
		jetilog( 3, "clearing shortcuts..\n");

		while( shortCuts->next != NULL )
			shortCuts = shortCuts->next;

		while( shortCuts->prev != NULL )
			{
				shortCuts = shortCuts->prev;
				free( shortCuts->next );
				shortCuts->next = NULL;
			}

		free( shortCuts );

		jetilog( 2, "shortcuts cleard\n");
	}

void clearEnvironmett_sounds( soundeffectType *sounds )
	{
		jetilog( 3, "clearing sounds..\n");

		while( sounds->next != NULL )
			sounds = sounds->next;

		while( sounds->prev != NULL )
			{
				sounds = sounds->prev;
				free( sounds->next );
				sounds->next = NULL;
			}
		
		free( sounds );

		jetilog( 2, "sounds cleard\n");
	}

