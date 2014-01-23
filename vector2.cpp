#include "vector2.hpp"
 
Vector2::Vector2(void)
{
}
 
Vector2::Vector2(float x, float y)
{
    this->x = x;
    this->y = y;
}
 
Vector2::Vector2(const Vector2 &v)
{
	this->x = v.x;
	this->y = v.y;
}
// Returns the length of the vector
float Vector2::length()
{
    return sqrt(x * x + y * y);
}
 
// Normalizes the vector
void Vector2::normalize()
{
    Vector2 vector;
    float length = this->length();
 
    if(length != 0)
    {
        this->x = x/length;
        this->y = y/length;
    }
 }
