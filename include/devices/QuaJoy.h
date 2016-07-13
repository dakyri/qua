#ifndef _QUAJOY
#define _QUAJOY

#define QUA_JOY_NOT	0
#define QUA_JOY_AXIS	1
#define QUA_JOY_BUTTON	2
#define QUA_JOY_HAT	3
#define QUA_JOY_SLIDER	4	// may eventually want to treat as seperate or as axes
#define QUA_JOY_FORCE	5	// maybe in too? also envelopes and delays?

#define QUA_JOY_HAT_CENTRED	0
#define QUA_JOY_HAT_UP	1
#define QUA_JOY_HAT_UP_RIGHT	2
#define QUA_JOY_HAT_RIGHT	3
#define QUA_JOY_HAT_DOWN_RIGHT	4
#define QUA_JOY_HAT_DOWN	5
#define QUA_JOY_HAT_DOWN_LEFT	6
#define QUA_JOY_HAT_LEFT	7
#define QUA_JOY_HAT_UP_LEFT	8

typedef float	j_axis_t;
typedef bool	j_button_t;
typedef uchar	j_hat_t;
typedef uchar	j_type_t;
typedef uchar	j_which_t;
typedef uchar	j_stick_t;

class QuaJoy {
public:
	inline QuaJoy& set(j_type_t	_type = 0, j_which_t _which = 0, j_stick_t _stick = 0, j_axis_t _axis = 0) {
		type = _type;
		which = _which;
		stick = _stick;
		spare2 = 0;
		value.axis = _axis;
		return *this;
	}
	union joy_v {
		j_axis_t	axis;
		j_button_t	button;
		j_hat_t		hat;
//		float		slider;
	}			value;
	void		SetAxis(j_which_t, j_stick_t, j_axis_t);
	void		SetButton(j_which_t, j_stick_t, j_button_t);
	void		SetHat(j_which_t, j_stick_t, j_hat_t);
	j_type_t	type;
	j_which_t	which;
	j_stick_t	stick;
	uchar		spare2;
};


#endif