/* (C)Copyright 2016 Harri Leino
this program is distributed under the terms of the GNU General Public License*/

/*--= directorys.c =------*/
#include "projecttypes.h"
#include "systemlog.h"
#include "directorys.h"

#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

dirEntry *getlast( dirEntry *filelist )
	{
		systemlog( 4, "enterd getlast.\n" );

		if( filelist->next != NULL )
			{
				filelist = getlast( filelist->next );
			}
		
		return filelist;
	}

dirEntry *getlast_and_add( dirEntry *filelist )
	{
		systemlog( 4, "enterd getlast_and_add.\n" );

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

		systemlog( 4, "clearEntrys.\n" );

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
			len = 1,
			dotsize = 1; /*obs: not a bool*/

		char path[SIZE_WORKDIREKTORY];
		char link_target_path[SIZE_WORKDIREKTORY];
		char rooted_link_path[SIZE_WORKDIREKTORY];
		dirEntry *obj;
		struct stat buf;
		struct stat linkbuf;

			systemlog( 3, "Get entrys!...\n" );
		
			//create a new list by mallocin the first dirrentry	
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

					//avoid useing d_name directly ( results in read errors )
					strcpy( obj->filename, obj->file->d_name );
					
					strcpy( path, wd );
					strcat( path, "/" );
					strcat( path, obj->filename );
					lstat( path , &buf );

					obj->status = buf.st_mode;
					obj->filesize = buf.st_size;

					obj->selected = 0;

					if( S_ISDIR(buf.st_mode) )
						{
							strcpy( obj->presentation, obj->file->d_name );
							strcpy( obj->filetype, "<DIR>\0");
						}

					else if( S_ISREG(buf.st_mode) )
						{/*regular file*/
							dotsize = 1;
							strcpy( obj->filetype, "         " );
							for( x = strlen(obj->filename); x >= 0; x-- )
								{
									if( obj->filename[x] == '.' && x != 0 && dotsize != 0 )
										{/*dot found*/
											dotsize = 0;
											obj->presentation[x] = '\0';
										}
									else if( dotsize <= TYPE_LENGTH && dotsize != 0 && x != 0 )
										{
											obj->filetype[ TYPE_LENGTH - dotsize ] = obj->filename[x];
											dotsize++;
										}	
									else if( dotsize == 0 )
										{/*put the rest (name) in the presentation*/
											obj->presentation[x] = obj->filename[x]; 
										}
									else
										{/*filetype to big or none existing*/
											strcpy( obj->presentation, obj->filename );
											strcpy( obj->filetype, " " );
											break;
										}
								}

							while( obj->filetype[0] == ' ' && obj->filetype[1] != '\0' )
								{/*push filetype text to the left when finnished*/
									strcpy( obj->filetype, obj->filetype +1 );
								}

						}

					else if( S_ISLNK(buf.st_mode) )
						{//it is a link!
							if( ( len = readlink( path, link_target_path, sizeof(link_target_path) ) )!= -1 )
								{
									link_target_path[len] = '\0';

									//cant read link without the workdirectory of the link
									strcpy( rooted_link_path, wd );
									strcat( rooted_link_path, link_target_path );

									lstat( rooted_link_path, &linkbuf );

									if( S_ISREG(linkbuf.st_mode) 
									|| S_ISFIFO(linkbuf.st_mode)
									|| S_ISCHR(linkbuf.st_mode)
									|| S_ISBLK(linkbuf.st_mode) )
										{
											strcpy( obj->filetype, "link");
										}
									else if( S_ISDIR(linkbuf.st_mode) )
										{
											strcpy( obj->filetype, "<DIR L>");
										}
									else
										{
											systemlog( 1, "WARNING: coud not read link path properly" );
											strcpy( obj->filetype, "LinkER");
										}

									strcpy( obj->presentation, obj->filename );
									if( SHOWHIDDEN )
										{
											strcat( obj->presentation, " -> " );
											strcat( obj->presentation, link_target_path );
										}
								}
							else
								{
									strcpy( obj->filetype, "ERROR L\0");
									strcpy( obj->presentation, obj->filename );
								}
						}

					else if( S_ISFIFO(buf.st_mode) )
						{
							strcpy( obj->presentation, obj->filename );
							strcpy( obj->filetype, "fifo\0");
						}

					else if( S_ISCHR(buf.st_mode) )
						{
							strcpy( obj->presentation, obj->filename );
							strcpy( obj->filetype, "char\0");
						}

					else if( S_ISBLK(buf.st_mode) )
						{	
							strcpy( obj->presentation, obj->filename );
							strcpy( obj->filetype, "block\0");
						}

					else
						{
							strcpy( obj->presentation, obj->filename );
							strcpy( obj->filetype, "ERROR\0");
						}

					obj->number = 1;

					//get id3 tags
					if( isoffiletype(obj, obj->number, "mp3") 
					 || isoffiletype(obj, obj->number, "MP3") 
					 || isoffiletype(obj, obj->number, "ogg")
					 || isoffiletype(obj, obj->number, "OGG") 
					 || isoffiletype(obj, obj->number, "flac"))
						obj = getid3tags( obj, path );

					//only add objects that should be shown
					if( SHOWHIDDEN )
						{	filelist = bubbeladd( filelist , obj );	}
					else if( ( obj->filename[0] == '.' && strlen(obj->filename) == 1 ) 
								|| ( obj->filename[0] == '.' && obj->filename[1] == '.' && strlen(obj->filename) == 2 ) 
								|| ( obj->filename[0] != '.' ) )
								{	filelist = bubbeladd( filelist , obj );	}
					else
						{	free( obj );	}

					//prepare for next file
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

		systemlog( 3, "Get entrys successfull\n" );
		return filelist;
	}

dirEntry *bubbeladd( dirEntry *filelist, dirEntry *obj )
	{
		int x;
		int rooted = 0; //true when the object has found its home

		systemlog( 4, "bubbleadd entry\n" );
		while( !rooted )
			{
				//place all <DIR>'s at top and sort them separatly
				if( filelist->filetype[0] == '<' && obj->filetype[0] != '<' )
					{ //non_dir_object is compared to a dir: drop righr at the spot (if we go futher we are at dirpart of list)
						rooted = 1;
						goto sort_action;
					}
				else if( filelist->filetype[0] != '<' && obj->filetype[0] == '<' )
					{// a dir_obj is compared to a nondir: push obj higher to list_dir_part
						goto sort_action;
					}

				//Do the comparation
				for( x = 0; x < PRESENTATION_LENGTH; x++ )
					{
						if( filelist->presentation[x] < obj->presentation[x] )
							{
								rooted = 1;
								goto sort_action;
							}
						else if( filelist->presentation[x] > obj->presentation[x] )
							{
								goto sort_action;
							}
					}

				sort_action: /* do swappings or push list forward*/
				if( filelist->number != 0 && rooted )
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
				else if( filelist->number != 0 && filelist->prev == NULL && !rooted )
					{/*is biggest*/
						obj->next = filelist;
						filelist->prev = obj;
						filelist = filelist->prev;
						rooted = 1;
					}
				else if( filelist->number == 0 )
					{/*is first*/
						filelist = obj;
						rooted = 1;
					}
				else
					{/*is bigger*/
						filelist = filelist->prev;
					}
			}

		//update listnumbers
		if( filelist->next != NULL )
			filelist = getlast_and_add( filelist->next );

		systemlog( 4, "bubbleadd entry successful" );
		return filelist;

	}

dirEntry *gotoEntry( dirEntry *filelist, int filenr )
	{
		int outofrange = 0;
		char erroroutput[16];

		systemlog( 4, "enterd gotoentry" );
		while( filelist->number != filenr )
		{
				if( !outofrange && filenr > filelist->number && filelist->next != NULL )
					{
						filelist = filelist->next;
					}
				else if( !outofrange && filenr < filelist->number  && filelist->prev != NULL )
					{
						filelist = filelist->prev;
					}
				else if( filenr == filelist->number )
					{
						//file was found
						break;
					}
				else if( outofrange && filenr > filelist->number && filelist->prev != NULL )
					{
						//backward searching
						filelist = filelist->prev;
					}
				else if( outofrange && filenr < filelist->number  && filelist->next != NULL )
					{
						//backward searching
						filelist = filelist->next;
					}
				else
					{
						if( outofrange )
							{
								systemlog( 1, "ERROR: gotoEntry: out of range:");

								snprintf(erroroutput, 16, " %d - %d\n", filenr, filelist->number );
								systemlog( 91, erroroutput );
								break;
							}

						outofrange = 1;
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

		systemlog( 3, "getid3tags...\n" );

		fp = fopen( path, "rb" );
			if( fseek( fp, -128, SEEK_END ) ||
				fread( id3_buf, sizeof(char), sizeof(id3_buf), fp ) != 1 )
				{
					readsucces = 0; 
					systemlog( 1, "ERORR: fseek or fread unsuccess full: id3tag\n" ); 
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
							systemlog( 5, "gott title/artist/album/year/comment\n" );
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

		systemlog( 3, "getid3tags successfull\n" );

		return obj;
	}

int printtotalnr( dirEntry *filelist )
	{
		systemlog( 4, "printtotalnr\n" );
		filelist = getlast( filelist );
		return filelist->number;
	}

int printCurrentnr( dirEntry *filelist )
	{
		systemlog( 4, "printCurrentnr\n" );
		return filelist->number;
	}

int printfiletypelenght( dirEntry *filelist, int filenr )
	{
		systemlog( 4, "printfiletypelenght\n" );
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

		systemlog( 4, "isoffiletype\n" );

		//get right entry
		if( filenr != filelist->number )
			{ filelist = gotoEntry( filelist, filenr ); }

		//check that not null ( makes for comparison imposible )
		if( ( type[0] == '\0' ) || ( filelist->filetype[0] == '\0' )  )
			yes = 0;

		//compare
		for( x = strlen(type); x >= 0; x-- )
			{
				if( type[x] != filelist->filetype[x] )
					{ yes = 0; }
			}

		return yes;
	}

