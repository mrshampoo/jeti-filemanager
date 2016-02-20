/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= autocomplete.h =--------------*/
#include "projecttypes.h"

#ifndef _autocomplete_
	#define _auocomplete_

	void addslash( char st[], char cmd[] );
	int find_and_add_dir( char st[], char cmd[], char dir[] );
	/*fp = file path*/
	int find_and_add_fp( char st[], char cmd[], char fp[] );

#endif
