#ifndef STREAMSURFACE_HPP
#define STREAMSURFACE_HPP


#include <deque>
#include <stdlib.h>

#include "vector2.hpp"

class Stream_Surface 
{


public:
	Vector2 *seed_points;

	Stream_Surface(Vector2 *seed_points);
	~Stream_Surface();
	Vector2* copy_array(Vector2 *arr);
};

#endif