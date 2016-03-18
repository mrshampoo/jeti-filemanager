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

int get_configname()
	{
		int c = 0;
		int exist = 1;
		char buff[250];
		FILE *fp;

		systemlog( 2, "try to locate configfiles" );

		fp = popen( "echo $XDG_CONFIG_HOME/jeti.rc", "r" );
		while( ( buff[c] = fgetc( fp )) != EOF )
			{ c++; } buff[c-1] = '\0';

		if( !access( buff, R_OK ) )
			{
				systemlog( 5, "access to " ); systemlog( 5, buff );
				strcpy( CONFIGNAME, buff );
			}
		else
			{
				c = 0;
				fp = popen( "echo $HOME/.jeti.rc;", "r" );
				while( ( buff[c] = fgetc( fp )) != EOF )
					{ c++; } buff[c-1] = '\0';

				if( !access( buff, R_OK ) )
					{
						systemlog( 5, "access to " ); systemlog( 5, buff ); systemlog( 5, " " );
						strcpy( CONFIGNAME, buff );
						printf( CONFIGNAME );
					}
				else
					{
						c = 0;
						fp = popen( "echo $XDG_DATA_DIRS/jeti/jeti.rc;", "r" );
						while( ( buff[c] = fgetc( fp )) != EOF )
							{ c++; } buff[c-1] = '\0';

						if( !access( buff, R_OK ) )
							{
								systemlog( 5, "access to " ); systemlog( 5, buff ); systemlog( 5, " " );
								strcpy( CONFIGNAME, buff );
								printf( CONFIGNAME );
							}
						else
							{ 
								systemlog( 1, "ERROR: there are no config files!!" );
								exist = 1;
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
				systemlog( 1, "WARNING: cant open config, looking for fallback config" );
				exist = 0;
			}

		return exist;
	}

int set_terminalname( char terminal[] )
	{
		int exist = 1;
		char termpath[256];

		strcpy( termpath, "/bin/" );
		strcat( termpath, terminal );
		
		if( !access( termpath, X_OK ) )
			strcpy( TERMINALNAME, terminal );
		else
			{
				systemlog( 1, terminal );
				systemlog( 1, "ERROR: there is no sush terminal. fallback set to xterm.\n" );
				strcpy( TERMINALNAME, "xterm" );
				exist = 0;
			}

		return exist;
	}

int get_terminalname()
	{
		int found = 0;
		int l = 0; //line reading
		int i = 0; //instruction reading
		int v = 0;

		char tmpterminalname[256];
		char buff[256];
		FILE *fp;

		tmpterminalname[0] = '\0';

		if( !access( CONFIGNAME, R_OK ) )
			{
				fp = fopen( CONFIGNAME, "rb");

				while( (buff[l] = fgetc(fp)) != EOF )
					{
						if( buff[l] == '\n' )
							{
								//remove blankspace in the beginning
								while( buff[0] == ' ' ) 
									{ strcpy( buff, buff+1 ); }

								if( !strncmp( buff, "terminal.name: ", 15 ) )
									{i+=15;

										//remove blankspace in the beginning
										while( buff[i] == ' ' )
											i++;

										while( buff[i+v] != '\n' )
											{
												tmpterminalname[v] = buff[i+v];
												v++;	
											}
											tmpterminalname[v] = '\0';

										if( tmpterminalname[0] != '\0' )
											{
												found = set_terminalname( tmpterminalname );
											}
										else
											{
												systemlog(1, "WARNING: get_terminalname: coud not find terminal name in config -> fallback terminal set to xterm");
												set_terminalname( "xterm" );
											}
									}

								l = 0;
							}
						else
							l++;
					}
			}
		else
			{
				systemlog(1, "ERROR: get_terminalname: no config -> fallback terminal set to xterm");
				set_terminalname( "xterm" );
			}	

		return found;
	}

Windowtype *init_win_colors( Windowtype *win )
	{
		int l = 0; //line reading
		int c = 0; //color fallback loop

		char buff[250];
		FILE *fp;

		systemlog( 2, "trying to initiate colors...\n" );

		//fallback values
		win->color[0] = -1;
		for( c = 1; c < 9; c++ )
      		win->color[c] = win->color[0];

		if( !access( CONFIGNAME, R_OK ) )
			{
				fp = fopen( CONFIGNAME, "rb");

			while( (buff[l] = fgetc(fp)) != EOF )
				{
					if( buff[l] == '\n' )
						{
							//remove blankspace in the beginning
							while( buff[0] == ' ' )
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

											else if( !strncmp( buff+13, "background: ", 12 ) )
												{ win->color[9] = (int) strtol( buff+25, NULL, 10 ); }
										}

								}
							//rewind readers	
							l = 0;
						}
					else
						l++;
				}
				fclose( fp );
			}
		else
			{
				//no config
			}

		systemlog( 2, "colors initiated\n");
		return win;
	}

tabtype *gettab( tabtype *tab, int n )
	{
		systemlog( 3, "enterd gettab..\n" );

		while( tab->prev != NULL && tab->n != n )
			tab = tab->prev;

		while( tab->next != NULL && tab->n != n )
			tab = tab->next;

		if( tab->next == NULL && tab->n != n )
			systemlog( 1, "ERROR: gettab: could not find tab\n" );

		return tab;
	}

tabtype *sorttabs( tabtype *tab, int wide )
	{
		int bubbled = 1;
		tabtype *temp;

		char strlog[10];

		systemlog( 3, "enterd sorttabs..\n" );
		
		systemlog( 4, "rewinds tabs.\n" );	
		while( tab->prev != NULL )
			tab = tab->prev;

		while( bubbled || tab->next != NULL )
			{
				//has ben rewined?
				if( tab->prev == NULL )
					{
						systemlog( 4, "starts over.\n" );
						bubbled = 0;
					}

				//error testing
				if( tab->next != NULL && tab->start == tab->next->start )
					systemlog( 1, "ERROR: tabs have same starting position\n" );

				//found some thing in the wrong end
				while( tab->next != NULL && tab->start > tab->next->start )
					{
						systemlog( 4, "sorting a tab." );
							sprintf( strlog, "(%i)", tab->n );
                            systemlog( 4, strlog );

						temp = tab->next;
						systemlog( 5, "temp = tab->next;" );

						tab->next = temp->next;
						temp->prev = tab->prev;
						systemlog( 5, "temp->prev = tab->prev;" );
					
						temp->prev->next = temp;
						if( tab->next != NULL )
							tab->next->prev = tab;
						systemlog( 5, "tab->next->prev = tab;" );

						tab->prev = temp;
						temp->next = tab;

						//temp = NULL;
						bubbled++;
						systemlog( 5, "sorting succes!.\n" );
					}

				//moves along in the tabquee
				if( tab->next == NULL && bubbled )
					{
						//rewind
						systemlog( 4, "rewind inside bubblesort.\n" );
						while( tab->prev != NULL )
							tab = tab->prev;
					}
				else if( tab->next != NULL )
                    {
                        systemlog( 4, "moves forward." );
                            sprintf( strlog, "(%i)\n", tab->n );
                            systemlog( 4, strlog );

                        tab = tab->next;
                    }
			}

		return tab;
	}

tabtype *init_wintabs_environment( tabtype *tab, int wide )
    {
        int left[10];
        int ln = -1;
		int rol = 0; //right or left
        int right[10];
        int rn = -1;

        int l = 0; //line reading
        int i = 0; //instruction reading
        char buff[250];
        FILE *fp;

		tab = NULL;

		systemlog( 2, "trying to initiate tabfeelds...\n" );

		if( !access( CONFIGNAME, R_OK ) )
            {
        		fp = fopen( CONFIGNAME, "rb" );
			systemlog( 4, "rc file opend\n" );

            while( (buff[l] = fgetc(fp) ) != EOF )
                {
                    if( buff[l] == '\n' )
                        {
							systemlog( 6, "EOL, ");

							//remove blanks in the beginning
                            while( buff[0] == ' ' )
								{
                                	strcpy( buff, buff+1 );
									systemlog( 4, "removed blank, ");
								}

							//read instructions
                            if( !strncmp( buff, "window.tab.", 11 ) )
                                {i+=11;

									systemlog( 4, "window.tab. found\n");

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
									systemlog( 4, "new tab malloced\n");

                                    if( !strncmp( buff+i, "left: ", 6 ) )
                                        {  i+=6; rol = -1; ln++; left[ln] = tab->n; }
									else if( !strncmp( buff+i, "right: ", 7 ) )
                                        {  i+=7; rol = 1; rn++; right[rn] = tab->n; }
									else
										{ /*ERROR no tab-direction*/ 
											systemlog( 1, "ERROR: no tab direction\n" );  
										}

                                            //get length
                                            if( !strncmp( buff+i, "selected", 8 ) )
                                                {  i+=8;
													tab->opt = selected; 
													tab->length = 1;
													systemlog( 4, "selected length found, ");
												}
                                            else if( !strncmp( buff+i, "filename", 8) )
                                                {  i+=8;
                                                    tab->opt = filename;
                                                    tab->length = PRESENTATION_LENGTH;
													systemlog( 4, "filename length found, ");
                                                }

                                            else if( !strncmp( buff+i, "presentation", 12) )
                                                {  i+=12;
                                                    tab->opt = presentation;
                                                    tab->length = PRESENTATION_LENGTH;
													systemlog( 4, "presentation length found, ");
                                                }
                                            else if( !strncmp( buff+i, "filetype", 8 ) )
                                                {  i+=8; 
													tab->opt = filetype; 
													tab->length = TYPE_LENGTH;
													systemlog( 4, "filetype length found, ");
												}
                                            else if( !strncmp( buff+i, "size", 4 ) )
                                                {  i+=4; 
													tab->opt = size; 
													tab->length = SIZE_SIZE; 
													systemlog( 4, "size length found, ");
												}
											else if( !strncmp( buff+i, "mode", 4 ) )
                                                {  i+=4;
                                                    tab->opt = mode;
                                                    tab->length = 10;
                                                    systemlog( 4, "mode length found, ");
                                                }
 											else if( !strncmp( buff+i, "nothing", 7 ) )
                                                {  i+=7; 
													tab->opt = nothing; 
													tab->length = 1; 
													systemlog( 4, "nothing length found, ");
												}
                                            else 
												{ /*ERROR tab length*/ 
													tab->opt = nothing; tab->length = 1;
													systemlog( 1, "ERROR: bad tab length, " ); 
												}

                                            //find start left
											if( rol < 0 )
												{
													systemlog( 4, "is left centerd\n" );
                                            		if( ln > 0 )
                                                		tab->start = gettab( tab, left[ln-1] )->start + gettab( tab, left[ln-1] )->length +1;
                                            		else
                                                		tab->start = 1;
												}

                                            //find start right
											else if( rol > 0 )
												{
													systemlog( 4, "is right centerd\n" );
                                            		if( rn > 0 )
                                                		tab->start = gettab( tab, right[rn-1] )->start - tab->length -1;
                                            		else
                                                		tab->start = wide - tab->length -2 ;
												}
										
                                }

							//rewind readers
                            i = 0;
							l = 0;
                        }
                    else
                        {
                            l++;
                        }
                }

        		fclose( fp );
				systemlog( 4, "rc file closed\n" );
			}
		else
			{
				//no config
			}

        if( ln == -1 && rn == -1 )
            {//ERROR no tabs read, faling back to defaults
				systemlog( 1, "WARNING: no tabs read, faling back to defaults\n" );

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

		systemlog( 2, "tabfeelds initiated\n");
        return tab;
    }

Windowtype *init_workdir( Windowtype *win )
	{
		int l = 0; //line reading
		int i = 0; //instruction reading
		int v = 1; //instruction values reading

		char cwd[1024];
		char buff[250];
		FILE *fp;

		systemlog( 2, "initing workdir\n");

		//fallback values:
		win->showhidden = 0;
		strcpy( win->wd, "/" );

		 if( !access( CONFIGNAME, R_OK ) )
            {
				fp = fopen( CONFIGNAME, "rb");

			while( (buff[l] = fgetc(fp)) != EOF )
      			{
        			if( buff[l] == '\n' )
          				{
							/*remove blankspace in the beginning*/
            				while( buff[0] == ' ' )
              					{ strcpy( buff, buff+1 ); }

							//read instructions
                        	if( !strncmp( buff, "window.startdirectory: ", 23 ) )
              					{ i+=23;
									if( buff[i] == '\"' )
										{ 
											//read values
											while( buff[i+v] != '\"' )
												{
													//copy content
													win->wd[v-1] = buff[i+v];
													v++;
												}
										}
									else if( !strncmp( buff+i, "cwd", 3 ) )
										{
											//get workdirectory as start dir
											if( !getcwd(cwd, sizeof(cwd)) )
												systemlog( 1, "ERROR: getcwd()");
											strcpy( win->wd, cwd );
										}
									else if( buff[i] != '\n' )
										i++;
								}
							else if( !strncmp( buff, "window.showhiden: ", 18 ) )
                                { i+=18;
									//read values
									win->showhidden = (int) strtol( buff+i, NULL, 10 );
                                }

							//rewind readers
							i = 0;
							l = 0;
                    		v = 1;
						}
					else
						l++;
				}

				fclose( fp );
			}
		else
			{
				//no config
			}

		systemlog( 2, "workdir initiated\n");
		return win;
	}

filetypeAction *init_fileAction()
	{

		filetypeAction *action;
		char command[COMMANDLENGTH];

		int l = 0; //line reading
		int i = 1; //instruction reading
		int v = 1; //instruction values reading

		char buff[250];
    	FILE *fp;

		systemlog( 2, "trying to initiate fileActions...\n" );

		action = malloc( sizeof(filetypeAction) );
			strcpy( action->Action, " " );
			action->next = malloc( sizeof(filetypeAction) );;
			action->next->next = NULL;
			action->next->prev = action;
			action->prev = NULL;

		if( !access( CONFIGNAME, R_OK ) )
            {
				fp = fopen( CONFIGNAME, "rb");

      			while( (buff[l] = fgetc(fp)) != EOF )
      				{
        				if( buff[l] == '\n' )
          					{
								/*remove blankspace in the beginning*/
            					while( buff[0] == ' ' )
              						{ strcpy( buff, buff+1 ); }
					
								//read instructions
								if( !strncmp( buff, "file.type: ", 11 ) )
									{ i+=10;

										//pack actions last in line 
										while( action->next != NULL )
											{
												action = action->next;
											}

										while( buff[i] != '\"' )
											i++;
															
										memset(command,0,sizeof(command)); /*empty command*/								

										//read values: action
										while( buff[i+v] != '\"' )
											{ command[v-1] = buff[i+v]; v++;  }

										strcpy( action->Action, command );

										i += v + 1; 
										v = 1;

										//read values: filetypes
										while( buff[i+v-1] != '\n' )
											{
												if( buff[i+v] != '\n' )
													{
														action->filetype[v-1] = buff[i+v];
													}

												v++;											

												if( buff[i+v] == ',' || buff[i+v] == '\n' )
													{
														action->next = malloc( sizeof(filetypeAction) );
														action->next->next = NULL;
														action->next->prev = action;
														action = action->next;
														strcpy( action->Action, command );
														i += v;     
														v = 1;
													}
											}
									}

								//rewind readers
								l = 0;
								i = 1;
							}
						else
							l++;
					}

				fclose( fp );
			}
		else
			{
				//no config
			}

		systemlog( 2, "fileAction initiated\n");
		return action;
	}

shortcutType *init_shortcuts()
	{
		shortcutType *shortCuts;

		char buff[250];
    	FILE *fp;

		int l = 0; //line reading
		int i = 0; //instruction reading
		int v = 1; //instruction values reading

		int y = 1; //placement

		systemlog( 2, "trying to initiate shortcuts...\n" );

		//prepare a empty shortcut to create a list
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
				while( (buff[l] = fgetc(fp)) != EOF )
					{
						if( buff[l] == '\n' )
							{
				 				//remove blankspace in the beginning
								while( buff[0] == ' ' )
									{ strcpy( buff, buff+1 ); }

								//read instruction
								if( !strncmp( buff, "window.shortcut: ", 17 ) )
									{ i+=17;
										while( buff[i] != '\'' )
										i++;

										//read label
										shortCuts->label = buff[i+1];
										i++;

										while( buff[i] != '\"' )
											i++;

										//read value
										while( buff[i+v] != '\"' )
											{	
												shortCuts->dir[v-1] = buff[i+v];
												v++;
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
								i = 0;
								v = 1;
								l = 0;

							}
							else 
								l++;
					}
				fclose( fp );
			}
		else
			{
				//no config
			}

		//remove unused mallocation
		shortCuts = shortCuts->prev;
		free( shortCuts->next );
		shortCuts->next = NULL; 

		systemlog( 2, "shortcuts initiated\n");
		return shortCuts;
	}
soundeffectType *init_soundeffects()
	{
		soundeffectType *sounds;
		//sounds = NULL;

		char buff[250];
    	FILE *fp;

		int l = 0; //line reading
		int i = 0; //instruction reading
		int v = 1; //instruction values reading

		systemlog( 2, "trying to initiate soundeffects...\n" );

		sounds = malloc( sizeof( soundeffectType ) );
		sounds->prev = NULL;
		sounds->next = NULL;
		sounds->action = -1;

		if( !access( CONFIGNAME, R_OK ) )
            {
				fp = fopen( CONFIGNAME, "rb");
			while( (buff[l] = fgetc(fp)) != EOF )
      			{
        			if( buff[l] == '\n' )
         	 			{
							//remove blankspace in the beginning
							while( buff[0] == ' ' )
              					{ strcpy( buff, buff+1 ); }

							//read instructions
							if( !strncmp( buff, "sound.", 6 ) )
              					{ i+=6;
									if( !strncmp( buff+i, "enter.", 6 ) )
              							{ i+=6;
											if( !strncmp( buff+i, "doubledot: ", 10 ) )
              									{ i+=10; sounds->action = 1; }
											else if( !strncmp( buff+i, "singledot: ", 11 ) )
                      							{ i+=11; sounds->action = 2;}
											else if( !strncmp( buff+i, "kat: ", 5 ) )
												{ i+=5; sounds->action = 3; }
											else if( !strncmp( buff+i, "executeble: ", 12 ) )
                      							{ i+=12; sounds->action = 4; }
											else if( !strncmp( buff+i, "scrollupp: ", 11 ) )
                                                { i+=11; sounds->action = 5; }
											else if( !strncmp( buff+i, "scrolldown: ", 12 ) )
                                                { i+=12; sounds->action = 6; }
											else if( !strncmp( buff+i, "stepupp: ", 9 ) )
                                                { i+=9; sounds->action = 7; }
											else if( !strncmp( buff+i, "stepdown: ", 10 ) )
                                                { i+=10; sounds->action = 8; }
											else if( !strncmp( buff+i, "selectfile: ", 12 ) )
                                                { i+=12; sounds->action = 9; }
											else if( !strncmp( buff+i, "copyfiles: ", 11 ) )
                                                { i+=11; sounds->action = 10; }
											else if( !strncmp( buff+i, "movefiles: ", 11 ) )
                                                { i+=11; sounds->action = 11; }
											else if( !strncmp( buff+i, "removefiles: ", 13 ) )
                                                { i+=13; sounds->action = 12; }
											else if( !strncmp( buff+i, "shortcut: ", 10 ) )
                                                { i+=10; sounds->action = 13; }
												
											while( buff[i] != '\"' )
												i++;
										
											//get values
											while( buff[i+v] != '\"' )
												{
													sounds->sound[v-1] = buff[i+v];
													v++;
												}
										}
              					}

							//prepare for next sound
							sounds->next = malloc( sizeof(soundeffectType) );
							sounds->next->next = NULL;
							sounds->next->prev = sounds;
							sounds = sounds->next;
							sounds->action = -1;

							//rewind readers
							v = 1;
                            i = 0;
                            l = 0;
						}
        			else 
						{
							l++;
						}
      			}
    			fclose( fp );
			}
		else
			{
				//no config
				systemlog( 1, "WARNING: soundeffects: config not read");
			}

		//remove unused mallocation
		sounds = sounds->prev;
		free( sounds->next );
		sounds->next = NULL;

		systemlog( 2, "soundeffects initiated\n");
		return sounds;
	}

void clearEnvironment_tabs( tabtype *tabs )
	{
		systemlog( 3, "clearing tabs..\n");

		while( tabs->next != NULL )
			tabs = tabs->next;
		while( tabs->prev != NULL )
			{
				tabs = tabs->prev;
				free( tabs->next );
				tabs->next = NULL;
			}

		free( tabs );

		systemlog( 2, "tabs cleard\n");
	}

void clearEnvironment_actions( filetypeAction *actions )
	{
		systemlog( 3, "clearing actions..\n");

		while( actions->next != NULL )
				actions = actions->next;

		while( actions->prev != NULL )
			{
				actions = actions->prev;
				free( actions->next );
				actions->next = NULL;
			}

		free( actions );

		systemlog( 2, "actions cleard\n");
	}

void clearEnvironment_shortcuts( shortcutType *shortCuts )
	{
		systemlog( 3, "clearing shortcuts..\n");

		while( shortCuts->next != NULL )
			shortCuts = shortCuts->next;

		while( shortCuts->prev != NULL )
			{
				shortCuts = shortCuts->prev;
				free( shortCuts->next );
				shortCuts->next = NULL;
			}

		free( shortCuts );

		systemlog( 2, "shortcuts cleard\n");
	}

void clearEnvironmett_sounds( soundeffectType *sounds )
	{
		systemlog( 3, "clearing sounds..\n");

		while( sounds->next != NULL )
			sounds = sounds->next;

		while( sounds->prev != NULL )
			{
				sounds = sounds->prev;
				free( sounds->next );
				sounds->next = NULL;
			}
		
		free( sounds );

		systemlog( 2, "sounds cleard\n");
	}

