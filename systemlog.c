/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

//--=systemlog.c =----------
#include "systemlog.h"
#include "projecttypes.h"
#include "handleflags.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//#define _globals_

//prioritys:0: no logging 	
//			1: ERRORS
//			2: entering a large process or funktion
//			3: entering a smaler process or funktion, ex. get_this()
//			4: bigger blocks inside process or funktion, ex. "printing window decorations..."
//			5: intestens of ifs and stuff, ex "is inside endofname_if"
//			6: frekvent stuff like graffics, ex on every row
//			7: even more frekvent stuff, ex on every character or pixel
static int _first_time_logging_ = 1;
static int last_was_newline = 0;

void systemlog( int priority, char logmessage[] )
	{
		int t;
		char completmessage[256];
		char file_path_and_name[256];
		FILE *file;

		if( FILEPATH_LOG[0] != '\0' && ( priority <= show_logprioritys() || ( priority -90 > 0 && priority -90 <= show_logprioritys() ) ) )
			{
						strcpy(completmessage, " ");
						strcpy( file_path_and_name, FILEPATH_LOG );
						if( FILEPATH_LOG[strlen(FILEPATH_LOG)] != '/' )
						strcat( file_path_and_name, "/" );
						strcat( file_path_and_name, "jeti.log" );

						if( _first_time_logging_ )
							{
								if( access( file_path_and_name, F_OK ) != -1 )
									remove( file_path_and_name );
									//makes shure we always start with a fresh log file
								_first_time_logging_ = 0;
							}

						file = fopen( file_path_and_name, "a+");
					
						if( !last_was_newline && priority < 5 )
                                                	strcat( completmessage, "\n" );

						for( t = 0; (t < priority -1) && priority < 5; t++ )
							strcat( completmessage,"\t" );
					

						strcat( completmessage, logmessage );

						if( logmessage[strlen(logmessage)-1] == '\n' )
							last_was_newline = 1;
						else
							last_was_newline = 0;

						fprintf( file, completmessage );

						fclose( file );

			}
		else if( priority == 1 || priority == 91 )
			{
				printf( logmessage );
			}
	}
