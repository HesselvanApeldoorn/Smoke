#include "simulation.hpp"


vector<Vector2> Simulation::seedpoints;

//------ SIMULATION CODE STARTS HERE -----------------------------------------------------------------
Simulation::Simulation()
{
	init_parameters();
}

void Simulation::init_parameters()
{
	dt = 0.5;               //simulation time step
	visc = 0.001;
	frozen = 0;
	init_simulation();
}

//init_simulation: Initialize simulation data structures as a function of the grid size 'n'.
//                 Although the simulation takes place on a 2D grid, we allocate all data structures as 1D arrays,
//                 for compatibility with the FFTW numerical library.
void Simulation::init_simulation()
{
	int i, n = Simulation::DIM; 
	size_t dim;

	dim     = n * 2*(n/2+1)*sizeof(fftw_real);        //Allocate data structures
	vx       = (fftw_real*) malloc(dim);
	vy       = (fftw_real*) malloc(dim);
	vx0      = (fftw_real*) malloc(dim);
	vy0      = (fftw_real*) malloc(dim);
	dim     = n * n * sizeof(fftw_real);
	fx      = (fftw_real*) malloc(dim);
	fy      = (fftw_real*) malloc(dim);
	rho     = (fftw_real*) malloc(dim);
	rho0    = (fftw_real*) malloc(dim);
	plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
	plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

	for (i = 0; i < n * n; i++)                      //Initialize data structures to 0
	{ vx[i] = vy[i] = vx0[i] = vy0[i] = fx[i] = fy[i] = rho[i] = rho0[i] = 0.0f; }

	seedpoints.clear(); //remove streamlines

	number_of_slices = 20;
	slices.clear(); //remove slices
	for (i = 0; i<number_of_slices; i++)
	{
		Grid* new_grid = new Grid(vx,vy, rho, fx, fy);
		slices.push_back(*new_grid);
	}
	stream_surfaces.clear();

}


//FFT: Execute the Fast Fourier Transform on the dataset 'vx'.
//     'dirfection' indicates if we do the direct (1) or inverse (-1) Fourier Transform
void Simulation::FFT(int direction,void* vx)
{
	if(direction==1) rfftwnd_one_real_to_complex(plan_rc,(fftw_real*)vx,(fftw_complex*)vx);
	else             rfftwnd_one_complex_to_real(plan_cr,(fftw_complex*)vx,(fftw_real*)vx);
}



//solve: Solve (compute) one step of the fluid flow simulation
void Simulation::solve(int n, fftw_real* vx, fftw_real* vy, fftw_real* vx0, fftw_real* vy0, fftw_real visc, fftw_real dt)
{
	fftw_real x, y, x0, y0, f, r, U[2], V[2], s, t;
	int i, j, i0, j0, i1, j1;

	for (i=0;i<n*n;i++)
	{ vx[i] += dt*vx0[i]; vx0[i] = vx[i]; vy[i] += dt*vy0[i]; vy0[i] = vy[i]; }

	for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
	   for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
	   {
		  x0 = n*(x-dt*vx0[i+n*j])-0.5f;
		  y0 = n*(y-dt*vy0[i+n*j])-0.5f;
		  i0 = clamp(x0); s = x0-i0;
		  i0 = (n+(i0%n))%n;
		  i1 = (i0+1)%n;
		  j0 = clamp(y0); t = y0-j0;
		  j0 = (n+(j0%n))%n;
		  j1 = (j0+1)%n;
		  vx[i+n*j] = (1-s)*((1-t)*vx0[i0+n*j0]+t*vx0[i0+n*j1])+s*((1-t)*vx0[i1+n*j0]+t*vx0[i1+n*j1]);
		  vy[i+n*j] = (1-s)*((1-t)*vy0[i0+n*j0]+t*vy0[i0+n*j1])+s*((1-t)*vy0[i1+n*j0]+t*vy0[i1+n*j1]);
	   }

	for(i=0; i<n; i++)
	  for(j=0; j<n; j++)
	  {  vx0[i+(n+2)*j] = vx[i+n*j]; vy0[i+(n+2)*j] = vy[i+n*j]; }

	FFT(1,vx0);
	FFT(1,vy0);

	for (i=0;i<=n;i+=2)
	{
	   x = 0.5f*i;
	   for (j=0;j<n;j++)
	   {
		  y = j<=n/2 ? (fftw_real)j : (fftw_real)j-n;
		  r = x*x+y*y;
		  if ( r==0.0f ) continue;
		  f = (fftw_real)exp(-r*dt*visc);
		  U[0] = vx0[i  +(n+2)*j]; V[0] = vy0[i  +(n+2)*j];
		  U[1] = vx0[i+1+(n+2)*j]; V[1] = vy0[i+1+(n+2)*j];

		  vx0[i  +(n+2)*j] = f*((1-x*x/r)*U[0]     -x*y/r *V[0]);
		  vx0[i+1+(n+2)*j] = f*((1-x*x/r)*U[1]     -x*y/r *V[1]);
		  vy0[i+  (n+2)*j] = f*(  -y*x/r *U[0] + (1-y*y/r)*V[0]);
		  vy0[i+1+(n+2)*j] = f*(  -y*x/r *U[1] + (1-y*y/r)*V[1]);
	   }
	}

	FFT(-1,vx0);
	FFT(-1,vy0);

	f = 1.0/(n*n);
	for (i=0;i<n;i++)
	   for (j=0;j<n;j++)
	   { vx[i+n*j] = f*vx0[i+(n+2)*j]; vy[i+n*j] = f*vy0[i+(n+2)*j]; }
}


// diffuse_matter: This function diffuses matter that has been placed in the velocity field. It's almost identical to the
// velocity diffusion step in the function above. The input matter densities are in rho0 and the result is written into rho.
void Simulation::diffuse_matter(int n, fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *rho0, fftw_real dt)
{
	fftw_real x, y, x0, y0, s, t;
	int i, j, i0, j0, i1, j1;

	for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
		for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
		{
			x0 = n*(x-dt*vx[i+n*j])-0.5f;
			y0 = n*(y-dt*vy[i+n*j])-0.5f;
			i0 = clamp(x0);
			s = x0-i0;
			i0 = (n+(i0%n))%n;
			i1 = (i0+1)%n;
			j0 = clamp(y0);
			t = y0-j0;
			j0 = (n+(j0%n))%n;
			j1 = (j0+1)%n;
			rho[i+n*j] = (1-s)*((1-t)*rho0[i0+n*j0]+t*rho0[i0+n*j1])+s*((1-t)*rho0[i1+n*j0]+t*rho0[i1+n*j1]);
		}
}

//set_forces: copy user-controlled forces to the force vectors that are sent to the solver.
//            Also dampen forces and matter density to get a stable simulation.
void Simulation::set_forces(void)
{
	int i;
	for (i = 0; i < DIM * DIM; i++)
	{
		rho0[i]  = 0.995 * rho[i];
		fx[i] *= 0.85;
		fy[i] *= 0.85;
		vx0[i]    = fx[i];
		vy0[i]    = fy[i];
	}
}

void Simulation::insert_forces(int X, int Y, double dx, double dy)
{
	fx[Y * DIM + X] += dx;
	fy[Y * DIM + X] += dy;
	rho[Y * DIM + X] = 10.0f;
}

void Simulation::change_number_of_slices() 
{
	int difference = Simulation::number_of_slices-slices.size();

	for(int i=0; i<abs(difference);i++)
	{
		if(difference<0) slices.pop_front();
		if(difference>0) {
			Grid *new_grid = new Grid(vx,vy,rho,fx,fy);
			slices.push_back(*new_grid);
		}
	}
}


void Simulation::add_slice()
{

	Grid *new_grid = new Grid(vx, vy, rho, fx, fy);
	slices.pop_front();
	slices.push_back(*new_grid);

}

//do_one_simulation_step: Do one complete cycle of the simulation:
//      - set_forces:
//      - solve:            read forces from the user
//      - diffuse_matter:   compute a new set of velocities
//      - gluPostRedisplay: draw a new visualization frame
void Simulation::do_one_simulation_step(void)
{
	if (!frozen)
	{
		set_forces();
		solve(DIM, vx, vy, vx0, vy0, visc, dt);
		diffuse_matter(DIM, vx, vy, rho, rho0, dt);
		change_number_of_slices();
		add_slice();
		glutPostRedisplay();
	}
}

void Simulation::change_timestep(float step)
{
	dt += step;
}

void Simulation::change_viscosity(double viscosity)
{
	visc *= viscosity;
}

void Simulation::toggle_frozen()
{
	frozen = 1-frozen;
}

void Simulation::add_seedpoint(Vector2 point)
{
	if(seedpoints.size()<SEEDPOINTS_AMOUNT) seedpoints.push_back(point);
}

void Simulation::add_streamsurface(Vector2 p1, Vector2 p2)
{
	Vector2 diff = p1-p2;
	Vector2 *seed_points = (Vector2*) malloc(8);
	for(int i=0; i<8;i++)
	{
		seed_points[i]=p1+(diff*(float)i)/7.0;
		// cout << seed_points[i].x << " , y: " << seed_points[i].y << "\n";
	}

	Stream_Surface* stream_surface = new Stream_Surface(seed_points);
	// cout << stream_surfaces.size() << "\n";
	if(stream_surfaces.size()<STREAMSURFACE_SIZE) 	stream_surfaces.push_front(*stream_surface);


}
