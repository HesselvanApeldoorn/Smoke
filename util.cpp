#include "util.hpp"

int clamp(float x)
{ return ((x)>=0.0?((int)(x)):(-((int)(1-(x))))); }

float clamp(float value, float min, float max)
{
	return value < min ? min : ( value > max ? max : value);
}

float rad2deg(float x)
{
	return x * (180/M_PI);
}