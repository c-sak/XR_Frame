#include "ezMIDI.h"

HMIDIOUT ezMIDI::_hMidi;
bool ezMIDI::polyphonic; // polyphonic mode

int scale_standard[7] = { 0,2,4,5,7,9,11 };
int scale_bluenote[7] = { 0,3,5,6,7,10, 12 };
int scale_percussion[8] = { 35,38,41,45,47,44,49,51 };//////////////ドラムセット
int scale_rockcode[7] = { 0,3,5,7,10,11,15 };
