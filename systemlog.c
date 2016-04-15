/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

//--=systemlog.c =----------
#include "systemlog.h"
#include "handleflags.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//prioritys:0: no logging 	
//			1: ERRORS
//			2: entering a large process or funktion
//			3: entering a smaler process or funktion, ex. get_this()
//			4: bigger blocks inside process or funktion, ex. "printing window decorations..."
//			5: intestens of ifs and stuff, ex "is inside endofname_if"
//			6: frekvent stuff like graffics, ex on every row
//			7: even more frekvent stuff, ex on every character or pixel
static int _first_time_logging_ = 1;

void systemlog( int priority, char logmessage[] )
	{
		int t;
		char completmessage[250];
		FILE *file;

		if( priority <= show_logprioritys() )
			{
				strcpy(completmessage, " ");

				if( _first_time_logging_ )
					{
						if( access( "jeti.log", F_OK ) != -1 )
							remove( "jeti.log" );
							//makes shure we always start with a fresh log file
						_first_time_logging_ = 0;
					}

				file = fopen("jeti.log", "a+");
					for( t = 0; (t < priority -1) && priority < 6; t++ )
						strcat( completmessage,"\t" );

					strcat( completmessage, logmessage );
				
					if( logmessage[strlen(logmessage)-1] != '\n' && priority < 6 )
						strcat( completmessage, "\n" );

					fprintf( file, completmessage );
				fclose( file );
			}
	}
