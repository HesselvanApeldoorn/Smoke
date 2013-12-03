class Simulation;
class Visualization {

public:

	void set_colormap(float vy);
	void visualize(Simulation const &simulation, int winWidth, int winHeight);
	void rainbow(float value,float* R,float* G,float* B);
	void direction_to_color(float x, float y, int method);

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