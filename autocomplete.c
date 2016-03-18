/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/
#include "projecttypes.h"
#include "command-window.h"
#include <stdlib.h>
#include <string.h>

void addslash( char st[], char cmd[] )
	{
		int i = 0; //current char
		char c[2]; //char to string midleman
		char tmpcmd[COMMANDLENGTH];

		c[1] = '\0';
		tmpcmd[0] = '\0';

			for( i = 0; i < strlen(cmd); i++ )
				{
					if( cmd[i] == ' '
					||  cmd[i] == '\n'
					||  cmd[i] == '\t'
					||  cmd[i] == '\0'
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

int find_and_add_dir( char st[], char cmd[], char dir[] )
	{
		int at = 0;
		int i = 0; //current char
		char c[2]; //char to string midleman
		char tmpcmd[COMMANDLENGTH];

		c[1] = '\0';
		tmpcmd[0] = '\0';

		for( i = 0; i < strlen( cmd ); i++ )
			{
				if( cmd[i] == '$' && cmd[i+1] == 'd' )
					{
						at = i;
						strcat( tmpcmd, dir );
						i += 4;
						
					}
				else
					{
						c[0] = cmd[i];
						strcat( tmpcmd, c );
					}

			}

		strcpy( st, tmpcmd );
		return at;
	}

int find_and_add_fp( char st[], char cmd[], char fp[] )
	{
		int at = 0;
		int i = 0; //current char

		char c[2]; //char to string midleman
		char tmpcmd[COMMANDLENGTH];

		c[0] = '/'; 
		c[1] = '\0';
		tmpcmd[0] = '\0';

		for( i = 0; i != strlen( cmd ); i++ )
			{
				if( cmd[i] == '$' && cmd[i+1] == 'f' )
					{ 
						at = i;
						strcat( tmpcmd, fp );
						i += 2;
					}

				c[0] = cmd[i];
				strcat( tmpcmd, c );
			}

		strcpy( st, tmpcmd );
			
		return at;
	}
