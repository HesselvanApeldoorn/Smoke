// Usage: Drag with the mouse to add smoke to the fluid. This will also move a "rotor" that disturbs
//        the velocity field at the mouse location. Press the indicated keys to change options
//--------------------------------------------------------------------------------------------------


#include "Fluids.hpp"
#include "Util.hpp"


Simulation Fluids::simulation;		
Visualization Fluids::visualization;	


int Fluids::winWidth;
int Fluids::winHeight;		

int Fluids::main_window;

//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
// int   color_dir = 0;            //use direction color-coding or not
// float vec_scale = 1000;			//scaling of hedgehogs
// int   draw_smoke = 0;           //draw the smoke or not
// int   draw_vecs = 1;            //draw the vector field or not



void Fluids::update()
{
	glutSetWindow(main_window);
	simulation.do_one_simulation_step();
	glutPostRedisplay();
}

Fluids::Fluids(int argc, char **argv)
{
	printf("Fluid Flow Simulation and Visualization\n");
	printf("=======================================\n");
	printf("Click and drag the mouse to steer the flow!\n");
	printf("T/t:   increase/decrease simulation timestep\n");
	printf("S/s:   increase/decrease hedgehog scaling\n");
	printf("c:     toggle direction coloring on/off\n");
	printf("V/v:   increase decrease fluid viscosity\n");
	printf("x:     toggle drawing matter on/off\n");
	printf("y:     toggle drawing hedgehogs on/off\n");
	printf("m:     toggle thru scalar coloring\n");
	printf("a:     toggle the animation on/off\n");
	printf("q:     quit\n\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500,500);

	main_window = glutCreateWindow("Real-time smoke simulation and visualization");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(update);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(drag);


	simulation.init_simulation(Simulation::DIM);	//initialize the simulation data structures
	glutMainLoop();			//calls do_one_simulation_step, keyboard, display, drag, reshape
}



//------ INTERACTION CODE STARTS HERE -----------------------------------------------------------------

//display: Handle window redrawing events. Simply delegates to visualize().
void Fluids::display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	visualization.visualize(simulation, winWidth, winHeight);
	glFlush();
	glutSwapBuffers();
}

//reshape: Handle window resizing (reshaping) events
void Fluids::reshape(int w, int h)
{
 	glViewport(0.0f, 0.0f, (GLfloat)w, (GLfloat)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	winWidth = w; winHeight = h;
}

//keyboard: Handle key presses
void Fluids::keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  case 't': simulation.change_timestep(-0.001); break;
	  case 'T': simulation.change_timestep(-0.001); break;
	  case 'c': visualization.toggle_color(); break;
	  case 'S': visualization.change_hedgehog(1.2); break;
	  case 's': visualization.change_hedgehog(0.8); break;
	  case 'V': simulation.change_viscosity(5); break;
	  case 'v': simulation.change_viscosity(0.2);; break;
	  case 'x': visualization.toggle_smoke();
		    if (visualization.isSmoke()==0) visualization.turn_vector(1); break;
	  case 'y': visualization.toggle_vector();
		    if (visualization.isVector()==0) visualization.turn_smoke(1); break;
	  case 'm': visualization.toggle_scalarcol(); break;
	  case 'a': simulation.toggle_frozen(); break;
	  case 'q': exit(0);
	}
}



// drag: When the user drags with the mouse, add a force that corresponds to the direction of the mouse
//       cursor movement. Also inject some new matter into the field at the mouse location.
void Fluids::drag(int mx, int my)
{
	int xi,yi,X,Y; double  dx, dy, len;
	static int lmx=0,lmy=0;				//remembers last mouse location

	// Compute the array index that corresponds to the cursor location
	xi = (int)clamp((double)(Simulation::DIM + 1) * ((double)mx / (double)winWidth));
	yi = (int)clamp((double)(Simulation::DIM + 1) * ((double)(winHeight - my) / (double)winHeight));

	X = xi; Y = yi;

	if (X > (Simulation::DIM - 1))  X = Simulation::DIM - 1; if (Y > (Simulation::DIM - 1))  Y = Simulation::DIM - 1;
	if (X < 0) X = 0; if (Y < 0) Y = 0;

	// Add force at the cursor location
	my = winHeight - my;
	dx = mx - lmx; dy = my - lmy;
	len = sqrt(dx * dx + dy * dy);
	if (len != 0.0) {  dx *= 0.1 / len; dy *= 0.1 / len; }

	simulation.insert_forces(X, Y, dx, dy);
	
	lmx = mx; lmy = my;
}
