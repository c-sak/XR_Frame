#ifndef __EZ_TRACK_H__
#define __EZ_TRACK_H__
/*===========================================================================*/
/* OpenGL TRACK Utility Functions
 *
 * This code is provided for non-profit and personal learning use
 * without any guarantee for results of the use.
 * Any commercial reuse of this material is prohibited.
 * Under the condition, this code can be redistributed as is.
 * Copyright (C) 2014 Toshikazu Ohshima, All Rights Reserved.
 */

#include "platform.h"

const int _n_tracks = 128;// for Vicon human bones (max ~73, with margin)

typedef struct{
    int id;
    float x, y, z;
    float roll, pitch, yaw;
	char name[32];// longest shogun human bone name
	int parent;
} ezTrackDataT;

typedef struct{
	ezTrackDataT data[ _n_tracks ];
} ezTrackArrayT;

#ifdef PLATFORM_WINDOWS
#include "igSharedMemoryT.h"
#endif

class ezTracker{
  public:
	ezTracker( bool use = true );
	~ezTracker();
	//- ezTracker_Vicon??override??????~??virtual???t?^:Crescent
	virtual void init();
	virtual bool open( char *key, bool w );
	virtual void read();
	virtual void write();
	virtual void close();
	void setPos( int i, float x, float y, float z );
	void setRot( int i, float roll, float pitch, float yaw );
	void setName( int i, const char *name );
	void setID( int i, int id );	
	ezTrackDataT *getTrackData( int i );
	ezTrackDataT *getTrackData( const char *name );
	ezTrackArrayT *getTrackArray();
	void setTrackData( int i, ezTrackDataT *trackdata );
  protected: //- ?A?N?Z?X???????X:Crescent
	ezTrackArrayT trackarray;
	int n_tracks;
#ifdef PLATFORM_WINDOWS
	iglib::igSharedMemoryT<ezTrackArrayT> *trackshm;
#else
	void *trackshm;
#endif
	bool use;
};

void ezTrack_getPos( ezTrackDataT *track, float *pos );
void ezTrack_getRot( ezTrackDataT *track, float *rot );

void ezTrack_xformBase( ezTrackDataT *base, ezTrackDataT *target, float *pos, float *rot );

/*---------------------------------------------------------------------------*/

/*===========================================================================*/
#endif //__EZ_TRACK_H__
