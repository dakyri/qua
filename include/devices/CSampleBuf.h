#ifndef _C_SAMPLE_BUF
#define _C_SAMPLE_BUF

#include <math.h>

// set of pure C functions for manipulating sample buffers.
// kind of antique... a 90s relic, one of the original untouched files
// simplify sample buffer for vst style buffer (seperate into a float*) for each chan
// ????????????? maybe inline asms if we have to get nasty about it

/*
 * copy "nFrames" from the "nChan" channel signal "src" to the
 * "nChan" channel signal "dst" 
 */
inline void
sample_buf_copy(float **dst, float **src, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		float *q = src[j];
		for (long i=0; i<nFrames; i++) {
			*p++ = *q++;
		}
	}
}

/*
 * linear pitch shift copy at most "nFrames" from the "nChan" channel signal
 * "src" to the "nChan" channel signal "dst" starting at the double "position"
 * with a double pitch shifted "index" increment. presumably, there
 * are at most "nFrames" physical frames in src and physical space in
 * dst. the number of physical frames used and copied is returned.
 * an "nChan" array of float for the "last" sample is read and updated to the final
 * read src sample 
 * if "one" < 0, the direction is reversed.
 * a negative initial position should happily do nothing
 */
inline long
sample_buf_copy_linearpitch(float **dst, float **src, short nChan, long nFrames,
							double position, double one, float *last)
{
	long	nwrit=0;
	long	curf=0;
	bool	rev=(one<0.0);
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		float *q = src[j];
		nwrit = 0;
		while (nwrit<nFrames) {
			float	prevsamp;
			if (rev) {
				curf = (long)ceil(position);
				if (curf<0) {
					break;
				}
				if (curf==nFrames-1) {
					prevsamp = last[j];
				} else {
					prevsamp = q[curf+1];
				}
				*p++ = (float)(prevsamp+(curf-position)*(q[curf]-prevsamp));
			} else {
				curf = (long)floor(position);
				if (curf>=nFrames) {
					break;
				}
				if (curf <= 0) {
					prevsamp = last[j];
				} else {
					prevsamp = q[curf-1];
				}
				*p++ = (float)(prevsamp+(position-curf)*(q[curf]-prevsamp));
			}
			nwrit++;
			position += one;
		}
	}
	for (short j=0; j<nChan; j++) {
		last[j] = src[j][curf];
	}
	return nwrit;
}

/*
 * linear pitch shift copy at most "nFrames" from the mono signal
 * "src" to the "nChan" channel signal "dst" starting at the double "position"
 * with a double pitch shifted "index" increment. presumably, there
 * are at most "nFrames" physical frames in src and physical space in
 * dst. the number of physical frames used and copied is returned.
 * an "nChan" array of float for the "last" sample is read and updated to the final
 * read src sample 
 * if "one" < 0, the direction is reversed.
 * a negative initial position should happily do nothing
 */
inline long
sample_buf_copy_linearpitch_mono(
				float **dst, float *src, short nChan, long from, long nFrames,
				double position, double one, float *last)
{
	long	nwrit=0;
	long	curf=0;
	bool	rev=(one<0.0);
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		float *q = src;
		nwrit = 0;
		while (nwrit<nFrames) {
			float	prevsamp;
			if (rev) {
				curf = (long)ceil(position);
				if (curf<0) {
					break;
				}
				if (curf==nFrames-1) {
					prevsamp = last[j];
				} else {
					prevsamp = q[curf+1];
				}
				*p++ = (float)(prevsamp+(curf-position)*(q[curf]-prevsamp));
			} else {
				curf = (long)floor(position);
				if (curf>=nFrames) {
					break;
				}
				if (curf <= 0) {
					prevsamp = last[j];
				} else {
					prevsamp = q[curf-1];
				}
				*p++ = (float)(prevsamp+(position-curf)*(q[curf]-prevsamp));
			}
			nwrit++;
			position += one;
		}
	}
	last[0] = src[curf];
	return nwrit;
}

/*
 * copy "nFrames" from the mono signal "src" to "nChan" channels of the
 * multi channel "dest" starting at position "from" in "dst"
 */
inline void
sample_buf_copy_mono(float **dst, float *src, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		float *q = src;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q++;
		}
	}
}

/*
 * copy "nFrames" from the mono signal "src" to "nChan" channels of the
 * multi channel "dest" starting at position "from" in "dst"
 */
inline void
sample_buf_copy_mono(float **dst, float *src, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		float *q = src;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q++;
		}
	}
}
/*
 * copy "nFrames" backwards from the mono signal "src" to "nChan" channels of the
 * multi channel "dest" starting at position "from" in "dst"
 */
inline void
sample_buf_copy_reversed_mono(float **dst, float *src, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		float *q = src;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q--;
		}
	}
}

/*
 * copy "nFrames" from the multi channel signal "src" to "nChan" channels of the
 * mono "dest", averaging if "nChan" > 1
 */
inline void
sample_buf_copy_to_mono(float *dst, float **src, short nChan, long nFrames)
{
	float *p = dst;
	if (nChan == 1) {
		float *q = src[0];
		for (long i=0; i<nFrames; i++) {
			*p++ = *q++;
		}
	} else if (nChan == 2) { // probly the most common case
		float *q = src[0];
		float *r = src[1];
		for (long i=0; i<nFrames; i++) {
			*p++ = ((*q++)+(*r++))/2;
		}
	} else {
		for (long i=0; i<nFrames; i++) {
			float tot = 0;
			for (short j=0; j<nChan; j++) {
				tot += src[j][i];
			}
			*p++ = tot/nChan;
		}
	}
}

/*
 * copy "nFrames" from the "srcChan" multi channel sig "src" to the "dstChan" multi
 * channel sig "dst".
 */
inline void
sample_buf_copy_multi(float **dst, short dstChan, float **src, short srcChan, long nFrames)
{
	short j=0, k=0;
	for (;;) {
		if (j >= dstChan) {
			j = 0;
			break;
		}
		if (k >= srcChan) {
			return;
		}
		float *p = dst[j++];
		float *q = src[k++];
		for (long i=0; i<nFrames; i++) {
			*p++ = *q++;
		}
	}
	for (;;) {
		if (j >= dstChan) {
			j = 0;
		}
		if (k >= srcChan) {
			return;
		}
		float *p = dst[j++];
		float *q = src[k++];
		for (long i=0; i<nFrames; i++) {
			*p++ += *q++;
		}
	}
}


/*
 * accumulate "nFrames" from the "srcChan" multi channel sig "src" to the "dstChan" multi
 * channel sig "dst".
 */
inline void
sample_buf_add_multi(float **dst, short dstChan, float **src, short srcChan, long nFrames)
{
	short j=0, k=0;
	for (;;) {
		if (j >= dstChan) {
			j = 0;
		}
		if (k >= srcChan) {
			return;
		}
		float *p = dst[j++];
		float *q = src[k++];
		for (long i=0; i<nFrames; i++) {
			*p++ += *q++;
		}
	}
}


/////////////////////////////////////////////////////////////////////////
// for interleaved stereo copies and adds
/////////////////////////////////////////////////////////////////////////
/*
 * copy "nFrames" from the stereo interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" starting at position "from" in the
 * destination. the averaged stereo input is sent to every channel of the output
 */
inline void
sample_buf_copy_centring_interleaved_stereo(
	float **dst, float *src, short nChan, long from, long nFrames)
{
	float *q = src;
	float *p = dst[0]+from;
	for (long i=0; i<nFrames; i++) {
		*p++ = (*q+*(q+1))/2;
		q+=2;
	}
	for (short j=1; j<nChan; j++) {
		float *q = dst[0]+from;
		float *p = dst[j]+from;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q++;
		}
	}
}

/*
 * add "nFrames" from the stereo interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" starting at position "from" in the
 * destination. the averaged stereo input is sent to every channel of the output
 */
inline void
sample_buf_add_centring_interleaved_stereo(
	float **dst, float *src, short nChan, long from, long nFrames)
{
	float *q = src;
	float *p = dst[0]+from;
	for (long i=0; i<nFrames; i++) {
		*p++ += (*q+*(q+1))/2;
		q+=2;
	}
	for (short j=1; j<nChan; j++) {
		float *q = dst[0]+from;
		float *p = dst[j]+from;
		for (long i=0; i<nFrames; i++) {
			*p++ += *q++;
		}
	}
}

/*
 * linear pitch shift copy at most "nFrames" from the stereo interleaved signal
 * "src" to the "nChan" channel signal "dst" starting at the double "position"
 * with a double pitch shifted "index" increment.
 * the averaged stereo input is sent to every channel of the output
 * presumably, there are at most "nFrames" physical frames in src and physical space in
 * dst. the number of physical frames used and copied is returned.
 * an "nChan" array of float for the "last" sample is read and updated to the final
 * read src sample 
 * if "one" < 0, the direction is reversed.
 * a negative initial position should happily do nothing
 */
inline long
sample_buf_copy_linearpitch_centring_interleaved_stereo(
				float **dst, float *src, short nChan, long from, long nFrames,
				double position, double one, float *last)
{
	long	nwrit=0;
	long	curf=0;
	long	curs = 0;
	bool	rev=(one<0.0);
	float *q = src;
	float *p = dst[0]+from;

	while (nwrit<nFrames) {
		float	prevsamp;
		if (rev) {
			curf = (long)ceil(position);
			if (curf<0) {
				break;
			}
			curs = curf * 2;
			if (curf>=nFrames-1) {
				prevsamp = last[0];
			} else {
				prevsamp = ((q[curs+2]+q[curs+3])/2);
			}
			*p++ = (float)(prevsamp+(curf-position)*(((q[curs]+q[curs+1])/2)-prevsamp));
		} else {
			curf = (long)floor(position);
			if (curf>=nFrames) {
				break;
			}
			curs = curf * 2;
			if (curf <= 0) {
				prevsamp = last[0];
			} else {
				prevsamp = ((q[curs+2]+q[curs+3])/2);
			}
			*p++ = (float)(prevsamp+(position-curf)*(((q[curs]+q[curs+1])/2)-prevsamp));
		}
		nwrit++;
		position += one;
	}
	for (short j=1; j<nChan; j++) {
		float *q = dst[0]+from;
		float *p = dst[j]+from;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q++;
		}
	}
	last[0] = (src[curs]+src[curs+1])/2;
	return nwrit;
}


/*
 * linear pitch shift add at most "nFrames" from the stereo interleaved signal
 * "src" to the "nChan" channel signal "dst" starting at the double "position"
 * with a double pitch shifted "index" increment.
 * the averaged stereo input is sent to every channel of the output
 * presumably, there are at most "nFrames" physical frames in src and physical space in
 * dst. the number of physical frames used and copied is returned.
 * an "nChan" array of float for the "last" sample is read and updated to the final
 * read src sample 
 * if "one" < 0, the direction is reversed.
 * a negative initial position should happily do nothing
 */
inline long
sample_buf_add_linearpitch_centring_interleaved_stereo(
				float **dst, float *src, short nChan, long from, long nFrames,
				double position, double one, float *last)
{
	long	nwrit=0;
	long	curf=0;
	long	curs = 0;
	bool	rev=(one<0.0);
	float *q = src;
	float *p = dst[0]+from;

	while (nwrit<nFrames) {
		float	prevsamp;
		if (rev) {
			curf = (long)ceil(position);
			if (curf<0) {
				break;
			}
			curs = curf * 2;
			if (curf>=nFrames-1) {
				prevsamp = last[0];
			} else {
				prevsamp = ((q[curs+2]+q[curs+3])/2);
			}
			*p++ += (float)(prevsamp+(curf-position)*(((q[curs]+q[curs+1])/2)-prevsamp));
		} else {
			curf = (long)floor(position);
			if (curf>=nFrames) {
				break;
			}
			curs = curf * 2;
			if (curf <= 0) {
				prevsamp = last[0];
			} else {
				prevsamp = ((q[curs+2]+q[curs+3])/2);
			}
			*p++ += (float)(prevsamp+(position-curf)*(((q[curs]+q[curs+1])/2)-prevsamp));
		}
		nwrit++;
		position += one;
	}
	for (short j=1; j<nChan; j++) {
		float *q = dst[0]+from;
		float *p = dst[j]+from;
		for (long i=0; i<nFrames; i++) {
			*p++ += *q++;
		}
	}
	last[0] = (src[curs]+src[curs+1])/2;
	return nwrit;
}


/*
 * copy "nFrames" from the stereo interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" starting at position "from" in the
 * destination. the averaged stereo input is sent to every channel of the output
 */
inline void
sample_buf_copy_reversed_centring_interleaved_stereo(
	float **dst, float *src, short nChan, long from, long nFrames)
{
	float *q = src;
	float *p = dst[0]+from;
	for (long i=0; i<nFrames; i++) {
		*p++ = (*q+*(q+1))/2;
		q-=2;
	}
	for (short j=1; j<nChan; j++) {
		float *q = dst[0]+from;
		float *p = dst[j]+from;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q++;
		}
	}
}

/*
 * add "nFrames" from the stereo interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" starting at position "from" in the
 * destination. the averaged stereo input is sent to every channel of the output
 */
inline void
sample_buf_add_reversed_centring_interleaved_stereo(
	float **dst, float *src, short nChan, long from, long nFrames)
{
	float *q = src;
	float *p = dst[0]+from;
	for (long i=0; i<nFrames; i++) {
		*p++ = (*q+*(q+1))/2;
		q-=2;
	}
	for (short j=1; j<nChan; j++) {
		float *q = dst[0]+from;
		float *p = dst[j]+from;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q++;
		}
	}
}

///////////////////////////////////////////////////////////
// INTERLEAVED !!!!!!
///////////////////////////////////////////////////////////
/*
 * copy "nFrames" from the "nChan" channel interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" 
 */
inline void
sample_buf_copy_interleaved(
		float **dst, float *src, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		float *q = src + j;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q;
			q+=nChan;
		}
	}
}


/*
 * linear pitch shift copy at most "nFrames" from the "nChan" channel interleaved signal
 * "src" to the "nChan" channel signal "dst" starting at the double "position"
 * with a double pitch shifted "index" increment. presumably, there
 * are at most "nFrames" physical frames in src and physical space in
 * dst. the number of physical frames used and copied is returned.
 * an "nChan" array of float for the "last" sample is read and updated to the final
 * read src sample 
 * if "one" < 0, the direction is reversed.
 * a negative initial position should happily do nothing
 */
inline long
sample_buf_copy_linearpitch_interleaved(
			float **dst, float *src, short nChan, long nFrames,
			double position, double one, float *last)
{
	long	nwrit=0;
	long	curf=0;
	long	curs=0;
	bool	rev=(one<0.0);
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		float *q = src+j;
		nwrit = 0;
		while (nwrit<nFrames) {
			float	prevsamp;
			if (rev) {
				curf = (long)ceil(position);
				if (curf<0) {
					break;
				}
				curs = nChan*curf;
				if (curf==nFrames-1) {
					prevsamp = last[j];
				} else {
					prevsamp = q[curs+1];
				}
				*p++ = (float)(prevsamp+(curf-position)*(q[curf]-prevsamp));
			} else {
				curf = (long)floor(position);
				if (curf>=nFrames) {
					break;
				}
				curs = nChan*curf;
				if (curf <= 0) {
					prevsamp = last[j];
				} else {
					prevsamp = q[curs-1];
				}
				*p++ = (float)(prevsamp+(position-curf)*(q[curf]-prevsamp));
			}
			nwrit++;
			position += one;
		}
	}
	for (short j=0; j<nChan; j++) {
		last[j] = src[curs+j];
	}
	return nwrit;
}


/*
 * copy "nFrames" from the "nChan" channel interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" starting at position "from" in the
 * destination
 */
inline void
sample_buf_copy_interleaved(
		float **dst, float *src, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		float *q = src + j;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q;
			q+=nChan;
		}
	}
}

/*
 * linear pitch shift copy at most "nFrames" from the "nChan" channel interleaved signal
 * "src" to the "nChan" channel signal "dst" starting at position
 * "from" in "dst" and the double "position" in "src"
 * with a double pitch shifted "index" increment. presumably, there
 * are at most "nFrames" physical frames in src and physical space in
 * dst. the number of physical frames used and copied is returned.
 * an "nChan" array of float for the "last" sample is read and updated to the final
 * read src sample 
 * read src sample 
 * if "one" < 0, the direction is reversed.
 * a negative initial position should happily do nothing
 */
inline long
sample_buf_copy_linearpitch_interleaved(
		float **dst, float *src, short nChan, long from, long nFrames,
		double position, double one, float *last)
{
	long	nwrit=0;
	long	curf=0;
	long	curs=0;
	bool	rev=(one<0.0);
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		float *q = src+j;
		nwrit = 0;
		while (nwrit<nFrames) {
			float	prevsamp;
			if (rev) {
				curf = (long)ceil(position);
				if (curf<0) {
					break;
				}
				curs = nChan*curf;
				if (curf==nFrames-1) {
					prevsamp = last[j];
				} else {
					prevsamp = q[curs+1];
				}
				*p++ = (float)(prevsamp+(curf-position)*(q[curf]-prevsamp));
			} else {
				curf = (long)floor(position);
				if (curf>=nFrames) {
					break;
				}
				curs = nChan*curf;
				if (curf <= 0) {
					prevsamp = last[j];
				} else {
					prevsamp = q[curs-1];
				}
				*p++ = (float)(prevsamp+(position-curf)*(q[curf]-prevsamp));
			}
			nwrit++;
			position += one;
		}
	}
	for (short j=0; j<nChan; j++) {
		last[j] = src[curs+j];
	}
	return nwrit;
}


/*
 * copy "nFrames" from the "nChan" channel interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" starting at position "from" in the
 * destination
 */
inline void
sample_buf_copy_reversed_interleaved(
		float **dst, float *src, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		float *q = src + j;
		for (long i=0; i<nFrames; i++) {
			*p++ = *q;
			q-=nChan;
		}
	}
}


/*
 * add "nFrames" from the "nChan" channel interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" 
 */
inline void
sample_buf_add_interleaved(
		float **dst, float *src, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		float *q = src + j;
		for (long i=0; i<nFrames; i++) {
			*p++ += *q;
			q+=nChan;
		}
	}
}

/*
 * add "nFrames" from the "nChan" channel interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" starting at position "from" in the
 * destination
 */
inline void
sample_buf_add_interleaved(
		float **dst, float *src, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		float *q = src + j;
		for (long i=0; i<nFrames; i++) {
			*p++ += *q;
			q+=nChan;
		}
	}
}

/*
 * add backwards "nFrames" from the "nChan" channel interleaved signal "src" to the
 * "nChan" channel seperated signal "dst" starting at position "from" in the
 * destination
 */
inline void
sample_buf_add_reversed_interleaved(
		float **dst, float *src, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		float *q = src + j;
		for (long i=0; i<nFrames; i++) {
			*p++ += *q;
			q+=nChan;
		}
	}
}



/*
 * copy "nFrames" to the "nChan" channel interleaved signal "dst" from the
 * "nChan" channel seperated signal "src" 
 */
inline void
sample_buf_copy_to_interleaved(float *dst, float **src, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst+j;
		float *q = src[j];
		for (long i=0; i<nFrames; i++) {
			*p = *q++;
			p+=nChan;
		}
	}
}

/*
 * copy "nFrames" to the "nChan" channel interleaved signal "dst" from the
 * "nChan" channel seperated signal "src" starting at position "from" in the
 * source
 */
inline void
sample_buf_copy_to_interleaved(float *dst, float **src, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst+j;
		float *q = src[j]+from;
		for (long i=0; i<nFrames; i++) {
			*p = *q++;
			p+=nChan;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// BITS THAT ADD
///////////////////////////////////////////////////////////////////////
/*
 * add "nFrames" to the "nChan" channel interleaved signal "dst" from the
 * "nChan" channel seperated signal "src" 
 */
inline void
sample_buf_add_to_interleaved(float *dst, float **src, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst+j;
		float *q = src[j];
		for (long i=0; i<nFrames; i++) {
			*p += *q++;
			p+=nChan;
		}
	}
}

/*
 * add "nFrames" to the "nChan" channel interleaved signal "dst" from the
 * "nChan" channel seperated signal "src" starting at position "from" in the
 * source
 */
inline void
sample_buf_add_to_interleaved(float *dst, float **src, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst+j;
		float *q = src[j]+from;
		for (long i=0; i<nFrames; i++) {
			*p += *q++;
			p+=nChan;
		}
	}
}



/*
 * accumulate "nFrames" from the "nChan" channel signal "src" in the
 * "nChan" channel signal "dst" 
 */
inline void
sample_buf_add(float **dst, float **src, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		float *q = src[j];
		for (long i=0; i<nFrames; i++) {
			*p++ += *q++;
		}
	}
}

/*
 * accumulate "nFrames" from the mono signal "src" to "nChan" channels of the
 * multi channel "dest" 
 */
inline void
sample_buf_add_mono(float **dst, float *src, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		float *q = src;
		for (long i=0; i<nFrames; i++) {
			*p++ += *q++;
		}
	}
}

/*
 * accumulate "nFrames" from the mono signal "src" to "nChan" channels of the
 * multi channel "dest" starting at position "from" in "dst"
 */
inline void
sample_buf_add_mono(float **dst, float *src, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		float *q = src;
		for (long i=0; i<nFrames; i++) {
			*p++ += *q++;
		}
	}
}

/*
 * accumulate "nFrames" from the "nChan" multi channel signal "src" to the
 * mono "dest", averaging if "nChan" > 1
 */
inline void
sample_buf_add_to_mono(float *dst, float **src, short nChan, long nFrames)
{
	float *p = dst;
	if (nChan == 1) {
		float *q = src[0];
		for (long i=0; i<nFrames; i++) {
			*p++ += *q++;
		}
	} else if (nChan == 2) { // probly the most common case
		float *q = src[0];
		float *r = src[1];
		for (long i=0; i<nFrames; i++) {
			*p++ += ((*q++)+(*r++))/2;
		}
	} else {
		for (long i=0; i<nFrames; i++) {
			float tot = 0;
			for (short j=0; j<nChan; j++) {
				tot += src[j][i];
			}
			*p++ = tot/nChan;
		}
	}
}

/////////////////////////////////////////////////////////////////
// CHANNEL CONVERSION STOOF
////////////////////////////////////////////////////////////////
/*
 * convert "nFrames" in the "nChan" channel signal "dst" to mono
 * result left in dst[0]
 */
inline void
sample_buf_to_mono(float **dst, short nChan, long nFrames)
{
	if (nChan == 1) {
		return;
	} else if (nChan == 2) {
		float *p = dst[0];
		float *r = dst[0];
		float *q = dst[1];
		for (long i=0; i<nFrames; i++) {
			*p++ = ((*q++) + (*r++))/2;
		}
	} else {
		float *p = dst[0];
		for (long i=0; i<nFrames; i++) {
			float tot = 0;
			for (short j=0; j<nChan; j++) {
				tot += dst[j][i];
			}
			*p++ = tot/nChan;
		}
	}
}

/*
 * convert "nFrames" in the "nChan" channel signal "dst" to mono
 * result left in every channel of dst
 */
inline void
sample_buf_to_multi_mono(float **dst, short nChan, long nFrames)
{
	if (nChan == 1) {
		return;
	} else if (nChan == 2) {
		float *p = dst[0];
		float *r = dst[0];
		float *s = dst[1];
		float *q = dst[1];
		for (long i=0; i<nFrames; i++) {
			*s++ = *p++ = ((*q++) + (*r++))/2;
		}
	} else {
		float *p = dst[0];
		float *q;
		for (long i=0; i<nFrames; i++) {
			float tot = 0;
			for (short j=0; j<nChan; j++) {
				tot += dst[j][i];
			}
			*p++ = tot/nChan;
		}
		for (short j=1; j<nChan; j++) {
			p = dst[0];
			q = dst[j];
			for (long i=0; i<nFrames; i++) {
				*q++ = *p++;
			}
		}
	}
}


//////////////////////////////////////////////////////////////////
// FUNDAMENTALS
//////////////////////////////////////////////////////////////////

/*
 * apply a "gain" multiplier to "nFrames" of the "nChan" channel signal "dst" 
 */
inline void
sample_buf_gain(float **dst, float gain, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		for (long i=0; i<nFrames; i++) {
			*p++ *= gain;
		}
	}
}

/*
 * apply a vector of "nChan" "gain" multipliers to "nFrames" of the "nChan" channel
 * signal "dst" 
 */
inline void
sample_buf_gain(float **dst, float *gain, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		float g = gain[j];
		for (long i=0; i<nFrames; i++) {
			*p++ *= g;
		}
	}
}

/*
 * zero "nFrames" in the "nChan" channel signal "dst" , starting at
 * position "from"
 */
inline void
sample_buf_zero(float **dst, short nChan, long from, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j]+from;
		for (long i=0; i<nFrames; i++) {
			*p++ = 0;
		}
	}
}


/*
 * zero "nFrames" in the "nChan" channel signal "dst" 
 */
inline void
sample_buf_zero(float **dst, short nChan, long nFrames)
{
	for (short j=0; j<nChan; j++) {
		float *p = dst[j];
		for (long i=0; i<nFrames; i++) {
			*p++ = 0;
		}
	}
}

/*
 * allocate an "nChan" channel signal of length "nFrames" 
 */
inline float **
sample_buf_alloc(short nChan, long nFrames)
{
	float **p = new float*[nChan];
	for (short i=0; i<nChan; i++) {
		p[i] = new float[nFrames];
	}
	return p;
}

/*
 * free the "nChan" channel signal "p"
 */
inline void
sample_buf_free(float **p, short nChan)
{
	for (short i=0; i<nChan; i++) {
		delete [] p[i];
	}
	delete [] p;
}

#endif