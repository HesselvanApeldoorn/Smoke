#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#include <algorithm>
#include <cmath>
#include <GL/glut.h>
#include <rfftw.h>              //the numerical simulation FFTW library

#include "simulation.hpp"
#include "util.hpp"

class Simulation;

class Visualization {

friend class Fluids;

public:

	enum Option
	{
		DrawColor, 		//use direction color-coding or not
		DrawSmoke, 		//draw the smoke or not
		DrawVecs,       //draw the vector field or not
		OptionSize
	};

	Visualization();
	void set_colormap(float vy);
	void visualize(Simulation const &simulation, int winWidth, int winHeight);
	void rainbow(float value,float* R,float* G,float* B);
	void direction_to_color(float x, float y, int method);
	void toggle(Option option);
	bool is_enabled(Option option);
	void enable(Option option);
	void disable(Option option);

	void change_hedgehog(double scale);
	
	void toggle_scalarcol();

	float vec_scale;			//scaling of hedgehogs



private:
	int scalar_col;           //method for scalar coloring
	static const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
	static const int COLOR_RAINBOW=1;
	static const int COLOR_BANDS=2;
	int options[OptionSize];

	//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------

};

#endif