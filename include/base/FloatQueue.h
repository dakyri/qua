#ifndef __FLOATQUEUE
#define __FLOATQUEUE

class FloatQueue {
public:
	inline FloatQueue(long sz)
	{
		data = new float[datalen=sz];
		in = out = 0;
		nitems = 0;
	}
	inline ~FloatQueue()
	{
		delete data;
	}
	
	float		*data;
	long		in;
	long		out;
	long		nitems;
	long		datalen;
	
	inline long
	Has(long n)
	{
		return nitems >= n;
	}
	
	inline bool
	Fits(long n)
	{
		return nitems + n <= datalen;
	}
	
	inline long
	Take(long n)
	{
		if (n > nitems)
			n = nitems;
		nitems -= n;
		out += n;
		if (out >= datalen) {
			out -= datalen;
		}
		return n;
	}
			
	inline long
	Add(long n)
	{
		if (nitems+n > datalen)
			n = datalen-nitems;
		nitems += n;
		in += n;
		if (in >= datalen) {
			in -= datalen;
		}
		return n;
	}			
	
	inline float *InPtr() { return &data[in]; }
	inline float *OutPtr() { return &data[out]; }
};

#endif