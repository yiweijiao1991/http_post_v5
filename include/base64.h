#ifndef __BASE__64__
#define __BASE__64__

char * base64_encode( const unsigned char * bindata, char * base64, int binlength );
int base64_decode( const char * base64, unsigned char * bindata );
#endif
