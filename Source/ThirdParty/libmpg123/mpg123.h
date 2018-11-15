/*
	mpg123_msvc: MPEG Audio Decoder library wrapper header for EpicGames UBT

	copyright 2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	initially written by Patrick Dehne and Thomas Orgis.
*/
#ifndef MPG123_UBT
#define MPG123_UBT

#include <stdlib.h>
#include <sys/types.h>

#define MPG123_NO_CONFIGURE
#include "mpg123.h.in" /* Yes, .h.in; we include the configure template! */
#endif