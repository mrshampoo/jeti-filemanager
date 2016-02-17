/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

//-- soundeffects.c
#include "projecttypes.h"
#include "soundeffects.h"
#include "handleflags.h"
#include <string.h>
#include <stdlib.h>

soundeffectType *getsoundaction( soundeffectType *sounds, int action )
    {
        while( sounds->prev != NULL )
            {
                sounds = sounds->prev;
            }

        while( sounds->next != NULL && sounds->action != action )
            {
                sounds = sounds->next;
            }

        return sounds;
    }

void playsound( soundeffectType *sounds, int action )
	{
		char cmd[COMMANDLENGTH];

		if( sounds != NULL && is_not_muted() )
			{
				if( sounds->action != action )
					sounds = getsoundaction( sounds, action );

				if( sounds->action != -1 )
					{
						strcpy( cmd, "aplay " );
						strcat( cmd, sounds->sound );
						strcat( cmd, " > /dev/null 2>&1 &" );
						system( cmd );
					}
			}	
	}
