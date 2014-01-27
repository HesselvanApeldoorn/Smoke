#ifndef STREAMSURFACE_HPP
#define STREAMSURFACE_HPP



#include <deque>

#include "vector2.hpp"

class Stream_Surface 
{


public:
        Vector2 p1;
        Vector2 p2;
        Stream_Surface(Vector2 p1, Vector2 p2);
        ~Stream_Surface();
};


#endif