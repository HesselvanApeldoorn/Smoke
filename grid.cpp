#include "grid.hpp"


Grid::Grid()
{
      this->vx = {};
      this->vy = {};
      this->fx = {};
      this->fy = {};
      this->rho = {};
}


Grid::Grid(fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *fx, fftw_real *fy) 
{
      this->vx = copy_array(vx);
      this->vy = copy_array(vy);
      this->rho = copy_array(rho);
      this->fx = copy_array(fx);
      this->fy = copy_array(fy);
}

fftw_real* Grid::copy_array(fftw_real *arr)
{
      int dim = 60 * 2*(60/2+1)*sizeof(fftw_real);
      fftw_real *new_arr =  (fftw_real*) malloc(dim);
      for(int i=0; i<3600;i++)
      {
            new_arr[i] = arr[i];
      }

      return new_arr;
}