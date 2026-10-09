// Tracker Utility Functions

/*
 * This code is provided for non-profit and personal learning use
 * without any guarantee for results of the use.
 * Any comercial reuse of this material is prohibited.
 * Under the condition, this code can be redistributed as is.
 * Copyright (C) 2014 Toshikazu Ohshima, All Rights Reserved.
 */
#include "platform.h"

#include <stdlib.h>
#include <string.h>

//#include "igSharedMemoryT.h"
#include "ezTrack.h"

using namespace std; //おまじない

// 共有メモリからのデータを入れる変数
//static ezTrackArrayT trackarray;
// 共有メモリ
//static iglib::igSharedMemoryT<ezTrackArrayT> *trackshm;

/*===========================================================================*/
ezTracker::ezTracker( bool use )
{
	this->use = use;
	n_tracks = _n_tracks;
}

ezTracker::~ezTracker()
{
}

void ezTracker::init()
{
}
#ifdef PLATFORM_WINDOWS
bool ezTracker::open( char *key, bool w )
{
	if( use ){
    	trackshm = new iglib::igSharedMemoryT<ezTrackArrayT>( key );
	    return trackshm->open( w );
	}
	else{
		return true; // useがfalseのときには、ダミーでtrueを返す
	}
}
void ezTracker::read()
{
	if( use ) trackshm->read( trackarray );
}
void ezTracker::write()
{
	if( use ) trackshm->write( trackarray );
}
void ezTracker::close()
{
	if( use ) trackshm->close();
}
#else
bool ezTracker::open( char *key, bool w ){ return false; }
void ezTracker::read(){}
void ezTracker::write(){}
void ezTracker::close(){}
#endif
//-----------------------------------------------------------------------------
void ezTracker::setPos( int i, float x, float y, float z )
{
	trackarray.data[i].x = x;
	trackarray.data[i].y = y;
	trackarray.data[i].z = z;
}
void ezTracker::setRot( int i, float roll, float pitch, float yaw )
{
	trackarray.data[i].roll = roll;
	trackarray.data[i].pitch = pitch;
	trackarray.data[i].yaw = yaw;
}
void ezTracker::setName( int i, const char *name )
{
	strcpy( trackarray.data[i].name, name );
}
void  ezTracker::setID( int i, int id )
{
	trackarray.data[i].id = id;
}
ezTrackDataT *ezTracker::getTrackData( int i )
{
	return &(trackarray.data[i]);
}
ezTrackDataT *ezTracker::getTrackData( const char *name )
{
	int i;
	for( i=0; i<_n_tracks; i++ ){
		if( trackarray.data[i].id != -1 ){
			if( strcmp( trackarray.data[i].name, name ) == 0 )
				return &(trackarray.data[i]);
		}
	}
	return NULL;
}
void ezTracker::setTrackData( int i, ezTrackDataT *trackdata )
{
	setPos( i, trackdata->x, trackdata->y, trackdata->z );
	setRot( i, trackdata->roll, trackdata->pitch, trackdata->yaw );
	setName( i, trackdata->name );
	setID( i, trackdata->id );
}
ezTrackArrayT *ezTracker::getTrackArray()
{
	return &trackarray;
}
//-----------------------------------------------------------------------------
void ezTrack_getPos( ezTrackDataT *track, float *pos )
{
	pos[0] = track->x;
	pos[1] = track->y;
	pos[2] = track->z;
}
void ezTrack_getRot( ezTrackDataT *track, float *rot )
{
	rot[0] = track->roll;
	rot[1] = track->pitch;
	rot[2] = track->yaw;
}
//=============================================================================
