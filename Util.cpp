#include "Util.hpp"

int clamp(float x)
{ return ((x)>=0.0?((int)(x)):(-((int)(1-(x))))); }

float max(float x, float y)
{ return x < y ? y : x; }