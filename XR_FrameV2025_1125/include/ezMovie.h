#pragma once
/*########################################################################
 * ezMovie.h
 *######################################################################*/

#include "image.h"

class ezMovie {
private:
public:

	int _n_frames; // number of frames
	int _curr; // current frame
	bool _play;
	bool _pause;
	int _fps; // frames/sec
	int _time; // time of start
	int _elapsed; // elapsed time
	bool _repeat; // repeat switch

	ezImage *frame;

	char *_path;
	ezMovie( const char *path, int n, int fps ) {
		_path = (char *)malloc(strlen(path));
		strcpy(_path, path);
		_n_frames = n;
		_fps = fps;
		_curr = 1; // ◆ 0
		_play = false;
		_pause = false;
		_time = 0;
		_elapsed = 0;
		_repeat = false;

		char num[8];
		char fullpath[256];

		strcpy(fullpath, _path);
		sprintf(num, "%04d", _curr);
		strcat(fullpath, num);
		strcat(fullpath, ".jpg");
		printf("%s\n", fullpath);
		frame = new ezImage(fullpath);
	};

	~ezMovie() {
	};
	void play( int t ) {
		if( _pause ){
			_pause = false;
			_time = t - _elapsed;
		}
		else{
			stop();
			_play = true;
			_time = t;
		}
		read();
	}
	void stop( void ) {
		_play = false;
		_pause = false;
		_curr = 1; // ◆ 0
		_time = 0;
		_elapsed = 0; 
		read();
	}
	void repeat ( bool sw ){ _repeat = sw; }
	void pause( void ) {
		_pause = true; 
	}
	bool seek(int no) {
		if (no < _n_frames) {
			_curr = no;
			return true;
		}
		return false;
	}
	void rewind( void ) { seek(0); }

	bool update( int t = 0 ) {
		if( !_play ) return false;
		if( t == 0 ){ //コマ送りモード
			if ( _curr < _n_frames ) { // ◆ -1
				_curr++;
				read();
				return true;
			}
			else { //最後のフレームで停止
				stop();
				return false;
			}
		}
		else{
			_elapsed = t - _time;
			int i = 0.001 * _elapsed * _fps;
			if( _curr < i ){
				if( i < _n_frames + 1 ) _curr = i; // ◆ +0
				else{
					if( _repeat ){
						stop();
						play( t );
					}
					else{
						_curr = _n_frames; // ◆ -1
						_play = false;
					}
				}
				read();
				return true;
			}
			else{
				return false;
			}
		}
	}
	bool isPlaying( void ){ return _play; }
	void read(void) {
		{
			char num[8];
			char fullpath[256];

			strcpy(fullpath, _path);
			sprintf(num, "%04d", _curr);
			strcat(fullpath, num);
			strcat(fullpath, ".jpg");
			//printf("%s\n", fullpath);
			frame->loadJpg(fullpath);
			frame->update();
		}
	}
};