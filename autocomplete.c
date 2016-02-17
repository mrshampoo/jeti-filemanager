/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/
#include "projecttypes.h"
#include "command-window.h"
#include <stdlib.h>
#include <string.h>

void addslash( char st[], char cmd[] )
	{
		int i = 0;
		char c[2];
		char tmpcmd[COMMANDLENGTH];

		c[1] = '\0';
    tmpcmd[0] = '\0';

			for( i = 0; i < strlen(cmd); i++ )
				{
        	if( cmd[i] == ' '
          ||  cmd[i] == '('
          ||  cmd[i] == ')'
          ||  cmd[i] == '['
          ||  cmd[i] == ']'
          ||  cmd[i] == '<'
          ||  cmd[i] == '>' )
          	{
            	strcat( tmpcmd, "\\" );
            }

					c[0] = cmd[i];
          strcat( tmpcmd, c );
				}

			if( &st[0] == &cmd[0]  )
				{	strcpy( st, tmpcmd );	}
			else
				{	strcat( st, tmpcmd );	}
	}

int find_and_add_filename( char st[], char fn[] )
	{
		int at = 0;
		int a = 0;
		char tmpcmd[COMMANDLENGTH];
		char c[2];
			c[1] = '\0';
			tmpcmd[0] = '\0';

			for( a = 0; a != strlen( st ); a++ )
				{
					if( st[a] == '$' && st[a+1] == 'f' && st[a+2] == 'n'  )
						{ at = a;
							strcat( tmpcmd, fn );
							a += 3; 
						}

					c[0] = st[a];
					strcat( tmpcmd, c );
				}
			
				strcpy( st, tmpcmd );

		return at;
	}

int find_and_add_fp( char st[], char cmd[], char fp[], char destination[] )
	{
		int a = 0;

    char tmpcmd[COMMANDLENGTH];
    char c[2];
      c[0] = '/'; 
			c[1] = '\0';
      tmpcmd[0] = '\0';

      for( a = 0; a != strlen( cmd ); a++ )
        {
          if( cmd[a] == '$' && cmd[a+1] == 'f' )
            { 
              strcat( tmpcmd, fp );
              a += 2;
            }

					if( cmd[a] == '$' && cmd[a+1] == 'd' )
            { 
              strcat( tmpcmd, destination );
							if( tmpcmd[strlen(tmpcmd) != '/'] ) 
								strcat( tmpcmd, c );
              a += 2;
            }

          c[0] = cmd[a];
          strcat( tmpcmd, c );
        }

        strcpy( st, tmpcmd );
			
		return a/2;
	}