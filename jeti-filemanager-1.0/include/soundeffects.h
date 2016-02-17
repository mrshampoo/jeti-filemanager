/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

//-- soundeffects.h
#include "projecttypes.h"

#ifndef _SOUNDS_
	#define _SOUNDS_

	soundeffectType *getsoundaction( soundeffectType *sounds, int action );
	void playsound( soundeffectType *sounds, int action );

#endif
