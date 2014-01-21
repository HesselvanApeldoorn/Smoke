#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <math.h>
 
class Vector2
{
public:
	float x,y;

    Vector2(void);
    Vector2(float x, float y);
    Vector2(const Vector2 &v);

    float length();
    Vector2 normalize();
    
	Vector2 operator*(float n)
	{
	    return Vector2(x * n, y * n);
	}

	Vector2 operator+(Vector2 n)
	{
	    return Vector2(x + n.x, y + n.y);
	}
	Vector2 operator+(float n)
	{
	    return Vector2(x + n, y + n);
	}
};

#endif