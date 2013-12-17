#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#include <algorithm>
#include <cmath>
#include <GL/glut.h>
#include <rfftw.h>              //the numerical simulation FFTW library
#include <sstream>     // for ostringstream
#include <string>
#include <iostream>

#include "simulation.hpp"
#include "util.hpp"

using namespace std;

class Simulation;

class Visualization {

friend class Fluids;

public:

	enum Dataset // Different types of datasets
	{
		Density,
		Velocity,
		Force
	};

	enum ColorMap // Different types of colouring methods
	{
		BlackWhite,
		Rainbow,
		RedWhite,
		Fire
	};

	enum Option //Different types of options
	{
		DrawSmoke, 		//draw the smoke or not
		DrawVecs,       //draw the vector field or not
		Scaling,		//Apply scaling or not
		OptionSize		//auto assigned (last in enum==size of enum)
	};

	Visualization();
	void init_parameters();

	void set_colormap(Simulation const &simulation, int idx, float min_value, float max_value);
	void visualize(Simulation const &simulation, int winWidth, int winHeight);
	void rainbow(float value,float* R,float* G,float* B);
	void fire(float value,float* R,float* G,float* B);
	void direction_to_color(float x, float y);
	void toggle(Option option);
	bool is_enabled(Option option);
	void enable(Option option);
	void disable(Option option);

	void change_hedgehog(double scale);
	
	void toggle_scalarcol();

	float vec_scale;			//scaling of hedgehogs
	int selected_dataset;
	int number_of_colors;
	int selected_colormap;
	float clamp_min;
	float clamp_max;



private:
	void draw_gradient(int nrRect, int winWidth, int winHeight, float rgbValues[][3], float min_value, float max_value);
	void display_legend(int winWidth, int winHeight, float min_value, float max_value);
	void draw_string(string text, int x, int y);

	int options[OptionSize];

	//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------

};

#endif