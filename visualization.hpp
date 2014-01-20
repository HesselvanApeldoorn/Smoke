#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#include <algorithm>
#include <cmath>
#include <GL/glut.h>
#include <rfftw.h>              //the numerical simulation FFTW library
#include <sstream>     // for ostringstream
#include <string>
#include <vector>
#include <iostream>

#include "simulation.hpp"
#include "util.hpp"
#include "vector2.hpp"

using namespace std;

class Simulation;

class Visualization {

friend class Fluids;

public:

	enum Option //Different types of options
	{
		DrawSmoke, 		//draw the smoke or not
		DrawVecs,       //draw the vector field or not
		Scaling,		//apply scaling or not
		DrawStreamlines,//draw the streamlines or not
		Slices, 		//draw slices or not
		OptionSize		//auto assigned (last in enum==size of enum)
	};

	enum ColorMap // Different types of colouring methods
	{
		BlackWhite,
		Rainbow,
		RedWhite,
		Fire
	};

	enum ScalarField // Different types of scalar fields
	{
		DensityScalar,
		VelocityScalar,
		ForceScalar
	};

	enum VectorField // Different types of vector fields
	{
		VelocityVector,
		ForceVector,
		GradientVector
	};

	enum GlyphType // Different types of glyphs
	{
		Hedgehog,
		Cone,
		Arrow
	};

	Visualization();
	void init_parameters();

	void set_colormap(Simulation const &simulation, int idx, float min_value, float max_value);
	void visualize(Simulation const &simulation, int winWidth, int winHeight);
	void rainbow(float value,float* R,float* G,float* B);
	void fire(float value,float* R,float* G,float* B);
	void toggle(Option option);
	bool is_enabled(Option option);
	void enable(Option option);
	void disable(Option option);

	void change_hedgehog(double scale);
	
	void toggle_scalarcol();

	float vec_scale;			//scaling of hedgehogs

	int selected_scalar;
	int selected_vector;
	int selected_glyph;

	int number_of_colors;
	int selected_colormap;
	float clamp_min;
	float clamp_max;

	int number_of_glyphs_x;
	int number_of_glyphs_y;

	float number_of_opaque;



private:
	void draw_gradient(int nrRect, int winWidth, int winHeight, float rgbValues[][3], float min_value, float max_value);
	void display_legend(int winWidth, int winHeight, float min_value, float max_value);
	void direction_to_color(float f, float min_value, float max_value, float max_slices_value);

	void draw_string(string text, int x, int y);
	void draw_smoke(Simulation const &simulation, fftw_real wn, fftw_real hn, float min_value, float max_value);
	void interpolation(fftw_real *dataset_x, fftw_real* dataset_y, int i, int j, float *value_x, float *value_y, float *glyph_point_x, float *glyph_point_y);
	void vector_gradient(fftw_real *dataset_x, fftw_real* dataset_y, int i, int j, float *value_x, float *value_y, float *glyph_point_x, float *glyph_point_y, float max_value);
	void draw_glyphs(float value_x, float value_y, fftw_real wn, fftw_real hn, float glyph_point_x, float glyph_point_y, int z);
	void draw_streamlines(Simulation const &simulation, float winWidth, float winHeight, float wn, float hn, float min_value, float max_value, int z, float max_slices_value);
	void apply_scaling(Simulation const &simulation, float *min_value, float *max_value);
	void draw_vectors(fftw_real *dataset_x_scalar, fftw_real *dataset_y_scalar, fftw_real *dataset_x_vector, fftw_real *dataset_y_vector, fftw_real wn, fftw_real hn,  float min_value, float max_value, int z, float max_slices_value);

	int options[OptionSize];

	//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------

};

#endif