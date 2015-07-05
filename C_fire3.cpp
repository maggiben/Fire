#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include "c:\source\battle\video.h"
#include "c:\source\battle\key_int.h"
#include "c:\source\battle\tctimer.h"

#define byte        unsigned char
#define word        unsigned short int
#define lword       unsigned long int

/* 'fire' code view area information */
#define HSTART      10           //horizontal offset
#define VSTART      10           //vertical offset

/* screen specifics */
#define SCREENX     320         //total horizontal pixels of screen
#define SCREENY     200         //total vertical pixels of screen
#define SCREEN_SIZE 64000       //screen size in bytes
#define VIDEO_LOC   0xA000      //video memory segment address

/* bitmap buffer specifics */
#define MEMSIZE     64000       //size of buffer in bytes
#define HSIZE       300         //horizontal size in bytes
#define VSIZE       180         //vertical size in bytes
#define LAST        180         //last vertical line number

#define TABLESIZE		65535

byte pal_buf[ 768 ]; 			//palette buffer
byte far table[ TABLESIZE ]; 	//random number table
byte far buffer[ MEMSIZE ];  	//fire buffer for iteration of 'fire' routine

//variables for timer functions
unsigned long int start_time, stop_time;
double *time_ptr;
double time_fudge;
double avg_frame_time = 0;
unsigned long int frame_counter = 0;

int startup( void );


void main( void )
{
	lword vloop, hloop, count;

	word rand_count;

	word size = HSIZE;
	int last_offset;

	word video_offset;
	word table_seg;
	word buffer_seg;
	word buffer_off;

	//horizontal position to start drawing last line at
	last_offset = ( HSIZE - size ) / 2;

	//calculate offsets and segments of the far data blocks allocated
	buffer_seg   = FP_SEG( buffer );
	buffer_off   = FP_OFF( buffer );
	table_seg    = FP_SEG( table );
	video_offset = FP_OFF( SCREENX * VSTART + HSTART );

	//get ready to go
	if( startup() == 1 ) exit( 1 );

	while( key_code != ESCAPE ) {

		if( action == KEY_PRESS ) {

			action = CLEARED;

			switch( key_code ) {

				//save a screen shot
				case LEFTSHIFT:
					save_pcx( "fire.pcx", pal_buf );
					break;

				//decrease the size of the last line
				case NUMPADMINUS:
					size--;
					if( size < 1 ) size = 1;

					//blank the last line
					for( hloop = ( HSIZE*LAST );
					hloop < ( HSIZE*(LAST + 1) ); hloop++ )
					buffer[ hloop ] = 0;
					break;

				//increase the size of the random line
				case NUMPADPLUS:
					size++;
					if( size > HSIZE ) size = HSIZE;

					//blank the last line
					for( hloop = ( HSIZE*LAST );
					hloop < ( HSIZE*(LAST + 1) ); hloop++ )
					buffer[ hloop ] = 0;
					break;

				//dump random numbers into the buffer
				case SPACEBAR:
					for( count = 0; count < MEMSIZE; count++ )
						buffer[count] = random(256);
					break;
			}
			//calculate the offset to start drawing the random last line
			last_offset = ( ( HSIZE - size ) >> 1 );
		}

		//get the start time for this video frame
		start_time = readtimer();

		//here I attempt an assembly version of the 'fire' routine
		asm {
				push 	ds
				push 	es
				push	ax

				mov 	ax, 0xA000         //es:di -> video memory destination
				mov 	es, ax
				mov 	ax, [video_offset]
				mov 	di, ax

				mov 	ax, [buffer_seg]   //ds:si -> source buffer memory
				mov 	ds, ax
				mov 	ax, [buffer_off]
				mov 	si, ax

				mov	ax, 0
				mov	bx, 0
				mov 	dx, VSIZE       //vertical pixel counter
		}

		vloop:
		asm   mov 	cx, HSIZE / 2 		//horizontal pixel counter

		hloop: asm {
				mov 	al, [ si + HSIZE / 2 ]
				mov 	bl, [ si + HSIZE / 2 - 1 ]
				add	ax, bx
				mov 	bl, [ si + HSIZE / 2 + 1 ]
				add	ax, bx
				mov 	bl, [ si + HSIZE / 2 * 2 ]
				add	ax, bx
				shr 	ax, 2

				jz		hloop2
				dec	ax
		}

		hloop2: asm {
				mov	[si], al    //make a copy for the next iteration...
				stosb             //..also dump the byte into the video mem.
				inc 	si          //..next video mem byte...
				stosb             //..dump the byte into video mem again.
				dec	cx
				jnz	hloop

				add 	di, ( SCREENX - HSIZE )  //goto start offset of next line

				dec 	dx
				jnz 	vloop


				//random last line into fire buffer
				//just step through an array of precalc'ed random numbers
				//stuff these bytes on to the LAST line of the fire window
				//buffer.

				mov ax,  [buffer_seg]		//last buffer line -> es:di
				mov es,	ax

				mov ax,	[buffer_off]
				add ax,	HSIZE*LAST / 2
				add ax,	[last_offset]
				mov di,	ax

				mov ax,	[table_seg]       //current random pos. -> ds:si
				mov ds,	ax
				mov ax,	[rand_count]      //last random number used
				mov si,	ax

				mov cx,	[size]            //dump them into buffer
				shr cx,  1
				rep movsb

				mov rand_count, si         //save the offset into the random
													//number array for the next run
				pop	ax
				pop	es
				pop	ds
		}

		//get the stop time for this frame
		stop_time = readtimer();

		//advance frame counter and compute the running average frame time
		frame_counter++;
		elapsedtime( start_time, stop_time, time_ptr );
		if( avg_frame_time != 0 )
			avg_frame_time = ( avg_frame_time + *time_ptr - time_fudge ) / 2;
		else avg_frame_time = *time_ptr - time_fudge;

	}

	//install the old keyboard handler
	install_oldkey();

	//return the timer back to normal
	restoretimer();

	//back to text mode
	mode(3);

	//report time statistics
	printf( "average frame time:\t%f ms\n", avg_frame_time );
	printf( "average f/s:\t\t%f frames/sec\n", 1/(avg_frame_time/1000) );
	printf( "total frame count:\t%ld frames\n", frame_counter );

}

int startup( void )
{
	long int count;

	time_ptr = (double *) malloc( sizeof( double ));
	initializetimer();
	start_time = readtimer();
	stop_time = readtimer();
	elapsedtime( start_time, stop_time, time_ptr );
	time_fudge = *time_ptr;
	for( count = 0; count < 1000; count++ ) {
		start_time = readtimer();
		stop_time = readtimer();
		elapsedtime( start_time, stop_time, time_ptr );
		time_fudge = ( time_fudge + *time_ptr ) / 2;
	}

	mode( 13 );

	color_scale( 0, 0, 0, 100, 0, 0, 0, 72, pal_buf );         //smoke
	color_scale( 100, 0, 0, 150, 50, 0, 73, 105, pal_buf );     //reds
	color_scale( 150, 50, 0, 255, 100, 0, 106, 150, pal_buf );    //yellows
	color_scale( 255, 100, 0, 255, 255, 255, 151, 255, pal_buf );  //whites

//	color_scale( 10, 0, 0, 255, 40, 40, 0, 72, pal_buf );         //smoke
//	color_scale( 255, 50, 50, 255, 190, 0, 73, 105, pal_buf );     //reds
//	color_scale( 255, 190, 0, 255, 255, 0, 106, 150, pal_buf );    //yellows
//	color_scale( 255, 255, 0, 255, 255, 255, 151, 255, pal_buf );  //whites


	//set up the palette using a color scale function
//	color_scale( 20, 10, 10, 50, 30, 30, 0, 19, pal_buf );      //smoke
//	color_scale( 60, 30, 30, 190, 50, 50, 20, 49, pal_buf );
//	color_scale( 190, 50, 50, 255, 190, 0, 50, 105, pal_buf );   //reds
//	color_scale( 255, 190, 0, 255, 255, 0, 106, 150, pal_buf );  //yellows
//	color_scale( 255, 255, 0, 255, 255, 255, 151, 255, pal_buf );//whites

	//load palette hardware registers with palette buffer
	set_pal( pal_buf );

	//initialize fire buffer
	for( count = 0; count < MEMSIZE; count++ )
		buffer[ count ] = 255;

	/* create the random table */
	randomize();
	for( count = 0; count < TABLESIZE; count++ ) {
		table[count] = random(2) * 255;
	}

	install_newkey();    //install custom keyboard interrupt handler

	return( 0 );
}