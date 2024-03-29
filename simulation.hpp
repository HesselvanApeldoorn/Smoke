#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <GL/glut.h>
#include <math.h>               //for various math functions
#include <rfftw.h>              //the numerical simulation FFTW library
#include <string>
#include <vector>
#include <deque>

#include <iostream>

#include "grid.hpp"
#include "streamsurface.hpp"
#include "util.hpp"
#include "vector2.hpp"

using namespace std;

class Simulation {

friend class Visualization;		

public:
	Simulation();
	void init_parameters();
	void init_simulation();

	void do_one_simulation_step(void);
	void change_timestep(float step);
	void change_viscosity(double viscosity);
	void toggle_frozen();
	void insert_forces(int X, int Y, double dx, double dy);
	void add_seedpoint(Vector2 point);
	void add_streamsurface(Vector2 p1, Vector2 p2);

    static const int DIM = 60;				//size of simulation grid
    static const int STREAMLINE_LENGTH = 60; // length of a streamline
    static const int SEEDPOINTS_AMOUNT = 100; // amount of seedpoints
    static const int STREAMSURFACE_SIZE = 30; // max amount of streamsurfaces
	float dt;				//simulation time step
	float visc;				//fluid viscosity
	int   frozen ;               //toggles on/off the animation
	// static Vector2 seedpoints[SEEDPOINTS_AMOUNT][STREAMLINE_LENGTH];
	static vector<Vector2> seedpoints;
	deque<Stream_Surface> stream_surfaces;
	deque<Grid> slices;
	int number_of_slices;
private:
	void FFT(int direction,void* vx);
	float max(float x, float y);
	void solve(int n, fftw_real* vx, fftw_real* vy, fftw_real* vx0, fftw_real* vy0, fftw_real visc, fftw_real dt);
	void diffuse_matter(int n, fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *rho0, fftw_real dt);
	void set_forces(void);
	void change_number_of_slices();
	void add_slice();
	
	//--- SIMULATION PARAMETERS ------------------------------------------------------------------------
	fftw_real *vx, *vy;             //(vx,vy)   = velocity field at the current moment
	fftw_real *vx0, *vy0;           //(vx0,vy0) = velocity field at the previous moment
	fftw_real *fx, *fy;	            //(fx,fy)   = user-controlled simulation forces, steered with the mouse
	fftw_real *rho, *rho0;			//smoke density at the current (rho) and previous (rho0) moment
	rfftwnd_plan plan_rc, plan_cr;  //simulation domain discretization
};

#endif