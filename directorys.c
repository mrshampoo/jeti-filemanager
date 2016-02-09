/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= directorys.c =------*/
#include "projecttypes.h"
#include "systemlog.h"

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include "directorys.h"

dirEntry *getlast( dirEntry *filelist )
	{
		jetilog( 4, "enterd getlast.\n" );
		if( filelist->next != NULL )
			{
				filelist = getlast( filelist->next );
			}
		
		return filelist;
	}

dirEntry *getlast_and_add( dirEntry *filelist )
	{
		jetilog( 4, "enterd getlast_and_add.\n" );
		filelist->number++;
    	if( filelist->next != NULL )
      		{
        		filelist = getlast_and_add( filelist->next );
      		}

    	return filelist;

	}

void clearEntrys( dirEntry *filelist )
	{
		dirEntry *prev;

		jetilog( 4, "clearEntrys.\n" );

		if( filelist != NULL )
			{
				filelist = getlast( filelist );
        
				while( filelist->prev != NULL )
          {
            prev = filelist;
            filelist = filelist->prev;
						filelist->next = NULL;
            free( prev );
						prev = NULL;
          }

				free( filelist );
				filelist = NULL;
			}
	}

dirEntry *getEntrys( DIR *dir, char wd[], dirEntry *filelist, int SHOWHIDDEN )
	{
		int x = 0,
				dotsize = 1; /*obs: not a bool*/

		char path[500];
		dirEntry *obj;
		struct stat buf;

			jetilog( 3, "Get entrys!...\n" );
			
			clearEntrys( filelist );
			filelist = malloc( sizeof(dirEntry) );
				filelist->next = NULL;
				filelist->prev = NULL;
				filelist->number = 0;
			obj = malloc( sizeof(dirEntry) );
				obj->next = NULL;
				obj->prev = NULL;

			 while( (obj->file = readdir(dir)) )
				{/*file was found*/
					strcpy( path, wd );
					strcat( path, "/" );
					strcat( path, obj->file->d_name );
					lstat( path , &buf );

					obj->status = buf.st_mode;
					obj->filesize = buf.st_size;

					obj->selected = 0;

					if( S_ISDIR(buf.st_mode) )
						{
							strcpy( obj->presentation, obj->file->d_name );
							strcpy( obj->filetype, "<KAT>\0");
						}

					else if( S_ISREG(buf.st_mode) )
						{/*regular file*/
							dotsize = 1;
							strcpy( obj->filetype, "          ");
							for( x = strlen(obj->file->d_name); x >= 0; x-- )
								{
									if( obj->file->d_name[x] == '.' && x != 0 && dotsize != 0 )
										{/*dot found*/
											dotsize = 0;
											obj->presentation[x] = '\0';
										}
									else if( dotsize <= TYPE_LENGTH && dotsize != 0 && x != 0 )
										{
											obj->filetype[ TYPE_LENGTH - dotsize ] = obj->file->d_name[x];
											dotsize++;
										}	
									else if( dotsize == 0 )
										{
											obj->presentation[x] = obj->file->d_name[x]; 
										}
									else
										{
											strcpy( obj->presentation, obj->file->d_name );
											strcpy( obj->filetype, " ");
											break;
										}
								}

							while( obj->filetype[0] == ' ' )
								{
									strcpy( obj->filetype, obj->filetype +1 );
								}

						}

					else if( S_ISLNK(buf.st_mode) )
						{
							strcpy( obj->presentation, obj->file->d_name );
            	strcpy( obj->filetype, "link\0");
						}

					else if( S_ISFIFO(buf.st_mode) )
						{
							strcpy( obj->presentation, obj->file->d_name );
            	strcpy( obj->filetype, "fifo\0");
						}

					else if( S_ISCHR(buf.st_mode) )
						{
							strcpy( obj->presentation, obj->file->d_name );
            	strcpy( obj->filetype, "char\0");
						}

					else if( S_ISBLK(buf.st_mode) )
						{	
							strcpy( obj->presentation, obj->file->d_name );
            	strcpy( obj->filetype, "block\0");
						}

					else
						{
							strcpy( obj->presentation, obj->file->d_name );
							strcpy( obj->filetype, "ERROR\0");
						}

					obj->number = 1;

					if( isoffiletype(obj, obj->number, "mp3") 
					 || isoffiletype(obj, obj->number, "MP3") 
					 || isoffiletype(obj, obj->number, "ogg")
					 ||	isoffiletype(obj, obj->number, "OGG") 
					 || isoffiletype(obj, obj->number, "flac"))
						obj = getid3tags( obj, path );

					if( SHOWHIDDEN )
						{	filelist = bubbeladd( filelist , obj );	}
					else if( ( obj->file->d_name[0] == '.' && strlen(obj->file->d_name) == 1 ) 
								|| ( obj->file->d_name[0] == '.' && obj->file->d_name[1] == '.' && strlen(obj->file->d_name) == 2 ) 
								|| ( obj->file->d_name[0] != '.' ) )
								{	filelist = bubbeladd( filelist , obj );	}
					else
						{	/*free( obj );*/	}

					obj = malloc( sizeof(dirEntry) );
					obj->prev = NULL;
					obj->next = NULL;
				}

			if( filelist->number != 0 )
				{/*no more files where found*/
					free( obj );
					obj = NULL;
				}
			else
				{/*nothing was found*/
					free( filelist );
					free( obj );
					filelist = NULL;
					obj = NULL;
				}

		jetilog( 3, "Get entrys successfull\n" );
		return filelist;
	}

dirEntry *bubbeladd( dirEntry *filelist, dirEntry *obj )
	{
		int x;
		int setroot = 0;

		jetilog( 3, "bubbleadd entry\n" );
		while( !setroot )
			{
				if( filelist->filetype[0] == '<' && obj->filetype[0] != '<' )
					{
						setroot = 1;
            goto sort_action;
					}
				else if( filelist->filetype[0] != '<' && obj->filetype[0] == '<' )
					{
						goto sort_action;
					}

				for( x = 0; x < PRESENTATION_LENGTH; x++ )
					{
						if( filelist->presentation[x] < obj->presentation[x] )
              {
                setroot = 1;
                goto sort_action;
              }
            else if( filelist->presentation[x] > obj->presentation[x] )
              {
                goto sort_action;
              }
					}

				sort_action:
				/* do swappings*/
				if( filelist->number != 0 && setroot )
					{
							obj->prev = filelist;
							if( filelist->next != NULL )
								{
									obj->next = filelist->next;
									obj->number = filelist->next->number;
									filelist->next->prev = obj;
								}
							else
								{
									obj->number = filelist->number +1;
								}
							filelist->next = obj;
							filelist = filelist->next;
					}
				else if( filelist->number != 0 && filelist->prev == NULL && !setroot )
					{/*is biggest*/
						obj->next = filelist;
						filelist->prev = obj;
						filelist = filelist->prev;
						setroot = 1;
					}
				else if( filelist->number == 0 )
					{/*is first*/
						filelist = obj;
						setroot = 1;
					}
				else
					{/*is bigger*/
						filelist = filelist->prev;
					}
			}

		if( filelist->next != NULL )
			filelist = getlast_and_add( filelist->next );

		jetilog( 3, "bubbleadd entry successful" );
		return filelist;

	}

dirEntry *gotoEntry( dirEntry *filelist, int filenr )
	{
		jetilog( 4, "enterd gotoentry\n" );
		while( filelist->number != filenr )
		{
				if( filenr > filelist->number && filelist->next != 0 )
					{
						filelist = filelist->next;
					}
				else if( filenr < filelist->number  && filelist->prev != 0 )
					{
						filelist = filelist->prev;
					}
				else
					{
						break;
						/*snopp nÃtt gick fel*/
					}
		}
		return filelist;
	}

dirEntry *getid3tags( dirEntry *obj, char path[] )
	{
		int readsucces = 1;

		char id3_buf[128];
			char title[31];
			char artist[31];
			char album[31];
			char year[5];
			char comment[31];

		FILE *fp;

		jetilog( 3, "getid3tags...\n" );

		fp = fopen( path, "rb" );
 			if( fseek( fp, -128, SEEK_END ) ||
				fread( id3_buf, sizeof(char), sizeof(id3_buf), fp ) != 1 )
				{
					readsucces = 0; 
					jetilog( 1, "ERORR: fseek or fread unsuccess full: id3tag\n" ); 
				}

     	if( readsucces && ( id3_buf[0] == 'T' && id3_buf[1] == 'A' && id3_buf[2] == 'G' ) )
     		{
				strncpy( title, id3_buf+3, 30 );
					strcat( title, "\0" );
				strncpy( artist, id3_buf+33, 30 );
					strcat( artist, "\0" );
                strncpy( album, id3_buf+63, 30 );
					strcat( album, "\0" );
                strncpy( year, id3_buf+67, 4 );
					strcat( year, "\0" );
                strncpy( comment, id3_buf+97, 30 );
					strcat( comment, "\0" );

					if( strlen( artist ) || strlen( title ) || strlen( year ) || strlen( album ) || strlen( comment ) )
						{
							jetilog( 5, "gott title/artist/album/year/comment\n" );
							strcpy( obj->presentation, "" );
						}

					if( strlen( artist ) )
						{
							strcat( obj->presentation, artist );
						}

					if( strlen( title ) )
						{
							if( strlen( artist ) )
								strcat( obj->presentation, " - " );
							strcat( obj->presentation, title );
						}

					if( strlen( year ) )
						{
							strcat( obj->presentation, " - " );
							strcat( obj->presentation, year );
						}

					if( strlen( album ) )
						{
							strcat( obj->presentation, " " );
							strcat( obj->presentation, album );
						}

					if( strlen( comment ) )
            			{
							strcat( obj->presentation, " " );
              				strcat( obj->presentation, comment );
            			}

     		}

    fclose( fp );

		jetilog( 3, "getid3tags successfull\n" );

		return obj;
	}

int printtotalnr( dirEntry *filelist )
	{
		jetilog( 4, "printtotalnr\n" );
		filelist = getlast( filelist );
		return filelist->number;
	}

int printCurrentnr( dirEntry *filelist )
	{
		jetilog( 4, "printCurrentnr\n" );
		return filelist->number;
	}

int printfiletypelenght( dirEntry *filelist, int filenr )
	{
		jetilog( 4, "printfiletypelenght\n" );
		if( filenr != filelist->number )
      { filelist = gotoEntry( filelist, filenr ); }

		return strlen( filelist->filetype );
	}

char printCurrentfiletype( dirEntry *filelist, int c, int filenr )
	{
		if( filenr != filelist->number )
      { filelist = gotoEntry( filelist, filenr ); }

		return filelist->filetype[c];
	}

char printfilename( dirEntry *filelist, int c, int filenr )
	{
		if( filenr != filelist->number )
			{	filelist = gotoEntry( filelist, filenr );	}

			return filelist->presentation[c];
	}

int isoffiletype( dirEntry *filelist, int filenr, char type[] )
	{
		int yes = 1,
				x = 0;

		jetilog( 4, "isoffiletype\n" );

		if( filenr != filelist->number )
      { filelist = gotoEntry( filelist, filenr ); }

		for( x = strlen(type); x >= 0; x-- )
		{
			if( type[x] != filelist->filetype[x] )
      { yes = 0; }
		}

		return yes;
	}
