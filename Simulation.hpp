#include <string>
#include <rfftw.h>              //the numerical simulation FFTW library
#include <math.h>               //for various math functions

class Simulation {
		friend class Visualization;		
public:
	Simulation();
	void init_simulation(int n);
	void do_one_simulation_step(void);
	void change_timestep(double step);
	void change_viscosity(double viscosity);
	void toggle_frozen();
	void insert_forces(int X, int Y, double dx, double dy);

    static const int DIM = 50;				//size of simulation grid

private:
	void FFT(int direction,void* vx);
	float max(float x, float y);
	void solve(int n, fftw_real* vx, fftw_real* vy, fftw_real* vx0, fftw_real* vy0, fftw_real visc, fftw_real dt);
	void diffuse_matter(int n, fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *rho0, fftw_real dt);
	void set_forces(void);

		//--- SIMULATION PARAMETERS ------------------------------------------------------------------------
	int   frozen ;               //toggles on/off the animation
	double dt;				//simulation time step
	float visc;				//fluid viscosity
	fftw_real *vx, *vy;             //(vx,vy)   = velocity field at the current moment
	fftw_real *vx0, *vy0;           //(vx0,vy0) = velocity field at the previous moment
	fftw_real *fx, *fy;	            //(fx,fy)   = user-controlled simulation forces, steered with the mouse
	fftw_real *rho, *rho0;			//smoke density at the current (rho) and previous (rho0) moment
	rfftwnd_plan plan_rc, plan_cr;  //simulation domain discretization
};