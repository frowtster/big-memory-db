#pragma once

int NewLineSize(char *buf);
void RandomString( char *buf, int length );

#ifndef htonll
#define htonll(x) \
	((((x) & 0xff00000000000000LL) >> 56) | \
	(((x) & 0x00ff000000000000LL) >> 40) | \
	(((x) & 0x0000ff0000000000LL) >> 24) | \
	(((x) & 0x000000ff00000000LL) >> 8) | \
	(((x) & 0x00000000ff000000LL) << 8) | \
	(((x) & 0x0000000000ff0000LL) << 24) | \
	(((x) & 0x000000000000ff00LL) << 40) | \
	(((x) & 0x00000000000000ffLL) << 56))
#endif
#ifndef ntohll
#define ntohll(x) \
	((((x) & 0x00000000000000FF) << 56) | \
	(((x) & 0x000000000000FF00) << 40) | \
	(((x) & 0x0000000000FF0000) << 24) | \
	(((x) & 0x00000000FF000000) << 8)  | \
	(((x) & 0x000000FF00000000) >> 8)  | \
	(((x) & 0x0000FF0000000000) >> 24) | \
	(((x) & 0x00FF000000000000) >> 40) | \
	(((x) & 0xFF00000000000000) >> 56))
#endif
