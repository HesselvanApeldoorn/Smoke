#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <GL/glut.h>
#include <math.h>               //for various math functions
#include <rfftw.h>              //the numerical simulation FFTW library
#include <string>

#include "util.hpp"


class Simulation {

friend class Visualization;		

public:
	Simulation();
	void init_parameters();
	void init_simulation(int n);

	void do_one_simulation_step(void);
	void change_timestep(float step);
	void change_viscosity(double viscosity);
	void toggle_frozen();
	void insert_forces(int X, int Y, double dx, double dy);

    static const int DIM = 120;				//size of simulation grid
	float dt;				//simulation time step
	float visc;				//fluid viscosity
	int   frozen ;               //toggles on/off the animation

private:
	void FFT(int direction,void* vx);
	float max(float x, float y);
	void solve(int n, fftw_real* vx, fftw_real* vy, fftw_real* vx0, fftw_real* vy0, fftw_real visc, fftw_real dt);
	void diffuse_matter(int n, fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *rho0, fftw_real dt);
	void set_forces(void);

	//--- SIMULATION PARAMETERS ------------------------------------------------------------------------
	fftw_real *vx, *vy;             //(vx,vy)   = velocity field at the current moment
	fftw_real *vx0, *vy0;           //(vx0,vy0) = velocity field at the previous moment
	fftw_real *fx, *fy;	            //(fx,fy)   = user-controlled simulation forces, steered with the mouse
	fftw_real *rho, *rho0;			//smoke density at the current (rho) and previous (rho0) moment
	rfftwnd_plan plan_rc, plan_cr;  //simulation domain discretization
};

#endif