#ifndef	_RECT_H
#define	_RECT_H


#if defined(WIN32)

class BRect: public CRect
{
public:
	inline BRect(long l, long t, long r, long b)
		{ Set(l,t,r,b); }
	inline BRect()
		{ Set(0,0,0,0); }
	inline bool	Contains(int x, int y)
		{ return x >= left && x <= right && y >= top && y <= bottom; }
	inline bool	Contains(CPoint &p)
		{ return p.x >= left && p.x <= right && p.y >= top && p.y <= bottom; }
	inline bool	Intersects(CRect &r)
		{ return r.right >= left && r.left <= right && r.top <= bottom && r.bottom >= top; }
	inline bool	Intersects(BRect &r)
		{ return r.right >= left && r.left <= right && r.top <= bottom && r.bottom >= top; }
	inline void operator=(CRect&r)
		{ left = r.left; right = r.right; top = r.top; bottom = r.bottom; }
	inline void Set(long l, long t, long r, long b)
		{ left = l; right = r; top = t; bottom = b; }
};

#endif

#endif
