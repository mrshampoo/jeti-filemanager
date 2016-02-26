/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handleflags.h"
#include "environment.h"

static int logprioritys_shown = 0;
static int mute = 0;

void showhelpmenu()
	{
		printf( "this is a filemanager cald Jeti\n" );
		printf( "usage: jeti <OPTION>\n" );
		printf( "  -h or --help\t\t\t shows this helpmenu\n" );
		printf( "  -m or --mute\t\t\t turn of sound\n" );
		printf( "  -c or --config <path>\t\t alternativ config\n" );
		printf( "  -v or --version\t\t print version\n" );
		printf( "  -l or --log <int>\t\t creates a jeti.log file\n" );
		printf( "<where <int> is, is the priority index\n" );
		printf("\tprioritys:\t0: no logging (default)\n");
		printf( "\t\t\t1: ERRORS\n" );
		printf( "\t\t\t2: entering a large process or funktion\n" );
		printf( "\t\t\t3: entering a smaler process or funktion, ex. get_this()\n" );
		printf( "\t\t\t4: bigger blocks inside process or funktion, ex. \"printing window decorations...\"\n" );
		printf( "\t\t\t5: intestens of ifs and stuff, ex \"is inside endofname_if\"\n" );
		printf( "\t\t\t6: frekvent stuff like graffics, ex on every row\n" );
		printf( "\t\t\t7: even more frekvent stuff, ex on every character or pixel\n" );
		printf( "\n" );
		exit(0);
	}
void print_version()
	{
		printf( "1.1.1\n" );
		exit(0);
	}

int handle_flags( int argc, char *argv[] )
    {
        int o = 0;
        int alternativ_config = 0;

        while( argv[o+1] != NULL )
            {
				o++;

                if( !strncmp( argv[o], "-h", 2 ) || !strncmp( argv[o], "--help", 6 ))
                    {
                        showhelpmenu();
                    }
                else if( !strncmp( argv[o], "-l", 2 ) )
                    {
                        logprioritys_shown = (int) strtol( argv[o+1], NULL, 10 );
                    }
                else if( !strncmp( argv[o], "-m", 2 ) || !strncmp( argv[o], "--mute", 6 ) )
                    {
                        mute = 1;
                    }
				else if( !strncmp( argv[o], "-c", 2 ) || !strncmp( argv[o], "--config", 8 ) )
                    {
						alternativ_config = set_configname( argv[o+1] );
					}
				else if( !strncmp( argv[o], "-v", 2 ) || !strncmp( argv[o], "--version", 9 ) )
                    {
						print_version();
					}
                else 
                    {
						//printf( "strange input!\n" );
                    }
            }
		if( !alternativ_config )
			alternativ_config = get_configname();

        return alternativ_config;
    }

int is_not_muted()
	{
		return !mute;
	}

int show_logprioritys()
	{
		return logprioritys_shown;
	}
int toggle_mute()
	{
		if( mute )
			mute = 0;
		else
			mute = 1;

		return mute;
	}
