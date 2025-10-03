#ifndef _MATH_H
#define _MATH_H

#define MIN(a, b)				(((a) < (b)) ? (a) : (b))
#define MAX(a, b)				(((a) > (b)) ? (a) : (b))
#define CLAMP(x, lower, upper)	(MIN(upper, MAX(x, lower)))

#endif
