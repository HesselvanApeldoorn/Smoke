#include <GL/glui.h>
#include <GL/glut.h>            //the GLUT graphics library
#include <iostream>
#include <rfftw.h>              //the numerical simulation FFTW library
#include <stdio.h>              //for printing the help text

#include "simulation.hpp"
#include "util.hpp"
#include "visualization.hpp"

class Fluids
{

public:
	Fluids(int argc, char **argv);

private:
	static Simulation simulation;
	static Visualization visualization;
	static const int GUI_WIDTH;
	static void update(void);
	static void usage();
	static void build_gui();
	static void myGlutIdle( void );
	static void reset_values();
	static void glui_callback( int control );

	static void display(void);
	static void reshape(int w, int h);
	static void keyboard(unsigned char key, int x, int y);
	static void move_camera(int key, int x, int y);
	static void drag(int mx, int my);

	static void click(int button, int state, int mx, int my);
	
	static int main_window;
	static int winWidth, winHeight;      //size of the graphics window, in pixels

	static float camera_pitch;
	static float camera_heading;


};