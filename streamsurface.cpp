#include "streamsurface.hpp"


Stream_Surface::Stream_Surface(Vector2 *seed_points)
{
      this->seed_points = copy_array(seed_points);
}

Stream_Surface::~Stream_Surface()
{
}


Vector2* Stream_Surface::copy_array(Vector2 *arr)
{
      Vector2 *new_arr =  (Vector2*) malloc(8);
      for(int i=0; i<8;i++)
      {
            new_arr[i] = arr[i];
      }

      return new_arr;
}