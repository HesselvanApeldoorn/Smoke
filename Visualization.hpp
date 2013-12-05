#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#include <cmath>
#include <algorithm>
#include <GL/glut.h>
#include <rfftw.h>              //the numerical simulation FFTW library

#include "Util.hpp"
#include "Simulation.hpp"

class Simulation;
class Visualization {

public:
	Visualization();
	void set_colormap(float vy);
	void visualize(Simulation const &simulation, int winWidth, int winHeight);
	void rainbow(float value,float* R,float* G,float* B);
	void direction_to_color(float x, float y, int method);
	void toggle_color();
	void change_hedgehog(double scale);
	void toggle_smoke();
	bool isSmoke();
	void turn_vector(int turn);
	void toggle_vector();
	bool isVector();
	void turn_smoke(int turn);
	void toggle_scalarcol();


private:
	int scalar_col;           //method for scalar coloring
	static const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
	static const int COLOR_RAINBOW=1;
	static const int COLOR_BANDS=2;


//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
	int   color_dir;            //use direction color-coding or not
	float vec_scale;			//scaling of hedgehogs
	int   draw_smoke;           //draw the smoke or not
	int   draw_vecs;            //draw the vector field or not
};

#endif