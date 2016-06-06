/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= projecttypes.h =-------*/
#include <dirent.h>
#include <ncurses.h>
#ifndef _projecttypes_
 #define _projecttypes_

	//#define CONFIGNAME "jeti.rc"
	#define COMMANDLENGTH 256
	#define CMDHISTORY 10
	#define SIZE_HISTORY 50
	#define SIZE_WORKDIREKTORY 256
	#define NUMBEROFCOLORS 10
	#define PRESENTATION_LENGTH 256
	#define TYPE_LENGTH 10
	#define DIRLENGTH 256
	#define SIZE_SIZE 5
	#define PASSWD_LENGTH 64

	#ifndef _globals_
		char TERMINALNAME[256];
		char CONFIGNAME[256];
		char FILEPATH_LOG[248];
		int DW_REACTION;
	#else
		extern char TERMINALNAME[256];
		extern char CONFIGNAME[256];
		extern char FILEPATH_LOG[248];
		extern int DW_REACTION;
	#endif

	typedef enum printoption 
		{ 
		  nothing, 
		  selected,
 		  filename,
		  presentation,
		  filetype,
		  mode, 
		  size 
		}printoption;

  	typedef struct tabtype
    	{
			int n; 	//order number for when the tab was read, for debugging purpose only
      		int start;
      		int length;
      		printoption opt;
			struct tabtype *next;
			struct tabtype *prev;
    	}tabtype;

	typedef struct filetypeAction
        {
            char filetype[TYPE_LENGTH];
            char Action[COMMANDLENGTH];
            struct filetypeAction* next;
            struct filetypeAction* prev;
        }filetypeAction;

	typedef struct shortcutType
        {
            int x,y;
            char dir[512];
            char label;
            struct shortcutType* next;
            struct shortcutType* prev;
        }shortcutType;

    typedef struct soundeffectType
        {
            int action; 	//action number, that corresponds to a specific action
            char sound[DIRLENGTH];
            struct soundeffectType* next;
            struct soundeffectType* prev;
        }soundeffectType;

	typedef struct dirEntry
    	{
      		struct dirent* file;
      		int number;
            int selected;
            int filesize;
            int status;
            char presentation[PRESENTATION_LENGTH];
            char filetype[TYPE_LENGTH];
      		struct dirEntry* next;
      		struct dirEntry* prev;
    	}dirEntry;

	typedef struct Windowtype
        {
            int h,w,x,y;
            int hidden; 				//is the window hidden or not?
            int showhidden;				//show hidden files and link paths
			int noexe;					//do not execute executables, use fileaction on them instead
            int mlevel;					//marker level, keeps track of wish directory the marker height was stored
            int marker[SIZE_HISTORY];
            int slide[SIZE_HISTORY];
            int color[NUMBEROFCOLORS];
            char wd[SIZE_WORKDIREKTORY];
            DIR *dir;
            tabtype *tab;
            dirEntry *filelist;
            WINDOW *win;
			shortcutType *shortcuts;
        }Windowtype;

	typedef struct cmdWindowtype
        {
            WINDOW *win;
            int h,w,x,y;
            int cruser;
            int hidden;
            char wd[SIZE_WORKDIREKTORY];
            char ddir[SIZE_WORKDIREKTORY];	//destination dir, keeps track of the passiv windows workdirectory
            char currentcmd[CMDHISTORY+1][COMMANDLENGTH];
        }cmdWindowtype;

	typedef struct dialogwindowtype
		{
			WINDOW *win;
			WINDOW *yes;
			WINDOW *no;
			int h,w,x,y;
			int hidden;
		}dialogwindowtype;

#endif
