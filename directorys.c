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

dirEntry *getlast( dirEntry *list )
	{
		while( list->next != NULL )
			list = list->next;

		return list;
	}

void clearEntrys( dirEntry **listptr )
	{
		dirEntry *prev;

		systemlog( 4, "clearEntrys.\n" );

		if( *listptr != NULL )
			{
				//get last
				while( (*listptr)->next != NULL )
					*listptr = (*listptr)->next;
        
				while( (*listptr)->prev != NULL )
					{
						prev = *listptr;
						*listptr = (*listptr)->prev;
						(*listptr)->next = NULL;
						free( prev );
						prev = NULL;
					}

				free( *listptr );
				*listptr = NULL;
			}
	}

int rget_filelist( DIR *dir, char wd[], dirEntry **listptr, int SHOWHIDDEN )
	{
		int success = 0;

		int x = 0;
		int len = 1;
		int dotsize = 1; /*obs: not a bool*/
		int link_loops = 0;
		int still_reading = 1;

		char path[SIZE_WORKDIREKTORY];
		char link_target_path[SIZE_WORKDIREKTORY];
		char rooted_link_path[SIZE_WORKDIREKTORY];
		dirEntry *obj = NULL;
		struct stat buf;
		struct stat linkbuf;

		systemlog( 3, "rget_filelist" );

		if( *listptr != NULL )
			{
				clearEntrys( listptr );
			}

		while( still_reading )
			{
				//create an new file object
				obj = malloc( sizeof( dirEntry ) );
				obj->prev = NULL;
				obj->next = NULL;

				if( (obj->file = readdir(dir) ) )
					still_reading = 1;
				else
					{
						still_reading = 0;
						free( obj );
						break;
					}
			
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
						systemlog( 4, "found a regular file");
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
						systemlog( 4, "found a symbolic link" );
						strcpy( rooted_link_path, path );
						do
							{
								if( ( len = readlink( rooted_link_path, link_target_path, sizeof(link_target_path) ) )!= -1 )
									{
										link_target_path[len] = '\0';
										if( link_loops == 0 )
											{
												//cant read link without the workdirectory of the link
												strcpy( rooted_link_path, wd );
											}
													
										strcat( rooted_link_path, link_target_path );
										lstat( rooted_link_path, &linkbuf );
	
										if( S_ISREG(linkbuf.st_mode) 
										|| S_ISFIFO(linkbuf.st_mode)
										|| S_ISCHR(linkbuf.st_mode)
										|| S_ISBLK(linkbuf.st_mode) )
											{
												strcpy( obj->filetype, "link");
												link_loops = 10;
											}
										else if( S_ISDIR(linkbuf.st_mode) )
											{
												strcpy( obj->filetype, "<DIR L>");
												link_loops = 10;
											}
										else if( S_ISLNK(linkbuf.st_mode) )
											{
												link_loops++;
											}
										else
											{
												systemlog( 1, "WARNING: coud not read link path properly" );
												strcpy( obj->filetype, "LinkER");
												link_loops = 10;
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
							} while( link_loops < 10 && link_loops > 0 );
					}
				else if( S_ISFIFO(buf.st_mode) )
					{
						systemlog( 4, "found a FIFO");
						strcpy( obj->presentation, obj->filename );
						strcpy( obj->filetype, "fifo\0");
					}
				else if( S_ISCHR(buf.st_mode) )
					{
						systemlog( 4, "found a char");
						strcpy( obj->presentation, obj->filename );
						strcpy( obj->filetype, "char\0");
					}
				else if( S_ISBLK(buf.st_mode) )
					{	
						systemlog( 4, "found a block" );
						strcpy( obj->presentation, obj->filename );
						strcpy( obj->filetype, "block\0");
					}

						else
							{
								systemlog( 1, "WARRNING: rget_filelist: unable to read file format");
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
				if( SHOWHIDDEN 
				|| ( obj->filename[0] == '.' && strlen(obj->filename) == 1 ) 
				|| ( obj->filename[0] == '.' && obj->filename[1] == '.' && strlen(obj->filename) == 2 ) 
				|| ( obj->filename[0] != '.' ) )
					{
						if( ( success = set_fileobj_tolist_sorted( listptr , obj )) == 0 )
							systemlog( 1, "ERROR: rget_filelist: set_fileobj_tolist_sorted: an unknown error detected!" );

						if( *listptr == NULL )
							systemlog( 1, "ERROR: rget_filelist: set_fileobj_tolist_sorted: list set but got back as NULL" );
					}
				else
					{
						free( obj );
					}
			}

		if( success )
			systemlog( 93,"\t[DONE]" );
		else
			systemlog( 93, "\t[FAILD]");

		return success;
	}

int set_fileobj_tolist_sorted( dirEntry **listptr, dirEntry *obj )
	{
		int success = 0;

		int x;
		int rooted = 0; //true when the object has found its home

		systemlog( 3, "set_fileobj_tolist_sorted" );

		if( obj == NULL )
			{
				systemlog( 1, "ERROR: set_fileobj_tolist_sorted: obj is NULL!" );
				success = -1;
			}
		if( obj->filename == NULL )
			{
				systemlog( 1, "ERROR: set_fileobj_tolist_sorted: obj->filename is NULL");
				success = -1;
			}
		if( obj->presentation == NULL )
			{
				systemlog( 1, "ERROR: set_fileobj_tolist_sorted: obj->presentation is NULL" );
				success = -1;
			}
		if( obj->filetype == NULL )
			{
				systemlog( 1, "ERROR: set_fileobj_tolist_sorted: obj->filetype is NULL");
				success = -1;
			}

		if( *listptr == NULL )
			{
				*listptr = malloc( sizeof(dirEntry) );
				systemlog( 4, "a New list was mallocated" );

				(*listptr)->prev = NULL;
				(*listptr)->next = NULL;
				(*listptr)->number = 0;
			}
		else
			{
				if( (*listptr)->number == 0 )
					{
						systemlog( 1, "ERROR: set_fileobj_tolist_sorted: list->number was 0, this should not happen. (none empty list with 0 objects!)");
						if( obj->number == 0 )
							systemlog( 1, "ERROR: set_fileobj_tolist_sorted: resiving obj->number with 0!" );
					}
			}

		while( !rooted )
			{
				//place all <DIR>'s at top and sort them separatly
				if( (*listptr)->filetype[0] == '<' && obj->filetype[0] != '<' )
					{ //non_dir_object is compared to a dir: drop righr at the spot (if we go futher we are at dirpart of list)
						rooted = 1;
						goto sort_action;
					}
				else if( (*listptr)->filetype[0] != '<' && obj->filetype[0] == '<' )
					{// a dir_obj is compared to a nondir: push obj higher to list_dir_part
						goto sort_action;
					}

				//Do the comparation
				for( x = 0; x < PRESENTATION_LENGTH; x++ )
					{
						if( (*listptr)->presentation[x] < obj->presentation[x] )
							{
								rooted = 1;
								goto sort_action;
							}
						else if( (*listptr)->presentation[x] > obj->presentation[x] )
							{
								goto sort_action;
							}
					}

				sort_action: /* do swappings or push list forward*/
				if( (*listptr)->number != 0 && rooted )
					{
							obj->prev = *listptr;
							if( (*listptr)->next != NULL )
								{
									obj->next = (*listptr)->next;
									obj->number = (*listptr)->number;
									(*listptr)->next->prev = obj;
								}
							else
								{
									obj->number = (*listptr)->number;
								}
							(*listptr)->next = obj;
							(*listptr) = (*listptr)->next;
					}
				else if( (*listptr)->number != 0 && (*listptr)->prev == NULL && !rooted )
					{/*is biggest*/
						(*listptr)->prev = obj;
						(*listptr)->prev->next = *listptr;
						(*listptr)->prev->prev = NULL;
						(*listptr) = (*listptr)->prev;
						rooted = 1;
					}
				else if( (*listptr)->number == 0 )
					{/*is first*/
						*listptr = obj;
						rooted = 1;
					}
				else
					{/*is bigger*/
						*listptr = (*listptr)->prev;
					}
			}

		//update listnumbers
		while( (*listptr)->next != NULL )
			{
				(*listptr)->number++;
				*listptr = (*listptr)->next;
			}
			if( (*listptr)->prev != NULL )
				(*listptr)->number++;

		if( success != -1 && ( success = rooted ) )
			systemlog( 93, "\t[DONE]" );
		else
			systemlog( 93, "\t[FAILED]" );

		return success;

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

								sprintf( erroroutput, " fetching: %d -> currnt filelist: %d\n", filenr, filelist->number );
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

		systemlog( 5, "isoffiletype" );

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

