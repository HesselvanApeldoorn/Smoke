#ifndef GRID_HPP
#define GRID_HPP

#include <rfftw.h>              //the numerical simulation FFTW library
#include <cstring>

#include <iostream>


class Grid 
{


public:
	Grid();
	~Grid();
	Grid(fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *fx, fftw_real *fy);
	fftw_real *vx;
	fftw_real *vy;
	fftw_real *fx;
	fftw_real *fy;
	fftw_real *rho;

private:
	fftw_real* copy_array(fftw_real *arr);

};

#endif