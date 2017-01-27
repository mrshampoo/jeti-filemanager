/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/
#include "projecttypes.h"
#include "command-window.h"
#include "systemlog.h"
#include <stdlib.h>
#include <string.h>

void addslash( char st[], char cmd[] )
	{
		int i = 0; //current char
		char c[2]; //char to string midleman
		char tmpcmd[COMMANDLENGTH];

		systemlog( 4, "addslash" );

		c[1] = '\0';
		tmpcmd[0] = '\0';

			for( i = 0; i < strlen(cmd); i++ )
				{
					if( cmd[i] == ' '
					||  cmd[i] == '&'
					||  cmd[i] == '('
					||  cmd[i] == ')'
					||  cmd[i] == '['
					||  cmd[i] == ']'
					||  cmd[i] == '\''
					||  cmd[i] == '"'
					||  cmd[i] == '<'
					||  cmd[i] == '>'
					||  cmd[i] == '\n'
					||  cmd[i] == '\t' )
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

int find_and_add_dir( char st[], char cmd[], char adir[], char pdir[] )
	{
		int activdirs = 0;
		int i = 0; //current char
		char c[2]; //char to string midleman
		char tmpcmd[512];

		systemlog( 4, "find_and_add_dir" );

		c[1] = '\0';
		tmpcmd[0] = '\0';

		for( i = 0; i < strlen( cmd ); i++ )
			{
				if( cmd[i] == '$' && cmd[i+1] == 'd' )
					{
						if( cmd[i+2] == '2' )
							{
								activdirs |= 2;
								strcat( tmpcmd, pdir );
								i += 4;
							}
						else if( cmd[i+2] == '1' || cmd[i+2] == ' ' )
							{
								activdirs |= 1;
								strcat( tmpcmd, adir );
								i += 4;
							}
						else
							{
								systemlog( 1, "ERROR: found $d but not separated from other shit" );
							}
						
					}
				else
					{
						c[0] = cmd[i];
						strcat( tmpcmd, c );
					}

			}
		c[0] = '\0';
		strcat(tmpcmd, c);

		strcpy( st, tmpcmd );
		systemlog( 93, st );
		return activdirs;
	}

int find_and_add_fp( char st[], char cmd[], char fp[] )
	{
		int activ = 0;
		int i = 0; //current char

		char c[2]; //char to string midleman
		char tmpcmd[COMMANDLENGTH];

		systemlog( 4, "find_and_add_fp" );

		c[0] = '/'; 
		c[1] = '\0';
		tmpcmd[0] = '\0';

		for( i = 0; i != strlen( cmd ); i++ )
			{
				if( cmd[i] == '$' && cmd[i+1] == 'f' )
					{ 
						activ |= 1;
						strcat( tmpcmd, fp );
						i += 2;
					}

				c[0] = cmd[i];
				strcat( tmpcmd, c );
			}

		strcpy( st, tmpcmd );
			
		return activ;
	}
