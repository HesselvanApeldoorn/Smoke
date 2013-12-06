// Usage: Drag with the mouse to add smoke to the fluid. This will also move a "rotor" that disturbs
//        the velocity field at the mouse location. Press the indicated keys to change options
//--------------------------------------------------------------------------------------------------


#include "fluids.hpp"

using namespace std;

Simulation Fluids::simulation;		
Visualization Fluids::visualization;	


int Fluids::winWidth;
int Fluids::winHeight;		

int Fluids::main_window;

GLUI_Checkbox   *checkbox;
GLUI_Spinner    *spinner;
GLUI_RadioGroup *radio;
GLUI_EditText   *edittext;

void Fluids::update()
{
	glutSetWindow(main_window);
	simulation.do_one_simulation_step();
	glutPostRedisplay();
}


Fluids::Fluids(int argc, char **argv)
{
	Fluids::usage();


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	
	glutInitWindowPosition( 400, 250 );
	glutInitWindowSize(1000,800);

	main_window = glutCreateWindow("Real-time smoke simulation and visualization");

	glutDisplayFunc(display);
	glutMotionFunc(drag);
	glutKeyboardFunc(keyboard);

	GLUI_Master.set_glutReshapeFunc(reshape);  
	GLUI_Master.set_glutIdleFunc(update);


	simulation.init_simulation(Simulation::DIM);	//initialize the simulation data structures

	Fluids::build_gui();

	glutMainLoop();			//calls do_one_simulation_step, keyboard, display, drag, reshape
}

void Fluids::usage()
{
	cout << "Fluid Flow Simulation and Visualization\n";
	cout << "=======================================\n";
	cout << "Click and drag the mouse to steer the flow!\n";
	cout << "T/t:   increase/decrease simulation timestep\n";
	cout << "S/s:   increase/decrease hedgehog scaling\n";
	cout << "c:     toggle direction coloring on/off\n";
	cout << "V/v:   increase decrease fluid viscosity\n";
	cout << "x:     toggle drawing matter on/off\n";
	cout << "y:     toggle drawing hedgehogs on/off\n";
	cout << "m:     toggle thru scalar coloring\n";
	cout << "a:     toggle the animation on/off\n";
	cout << "q:     quit\n\n";
}

void Fluids::control_cb( int control )
{
  /********************************************************************
    Here we'll print the user id of the control that generated the
    callback, and we'll also explicitly get the values of each control.
    Note that we really didn't have to explicitly get the values, since
    they are already all contained within the live variables:
    'wireframe',  'segments',  'obj',  and 'text'  
    ********************************************************************/

  printf( "callback: %d\n", control );
  printf( "             checkbox: %d\n", checkbox->get_int_val() );
  printf( "              spinner: %d\n", spinner->get_int_val() );
  printf( "          radio group: %d\n", radio->get_int_val() );
  printf( "                 text: %s\n", edittext->get_text() );
  
}

void Fluids::build_gui()
{

	/** These are the live variables passed into GLUI ***/
	int   wireframe = 0;
	int   obj = 0;
	int   segments = 8;
	string text = "Hello World!";
	GLUI *glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);
	glui->set_main_gfx_window(main_window);  // main_window is the main gfx window
	
	new GLUI_StaticText( glui, "Options" );
	new GLUI_Separator( glui );



	checkbox = new GLUI_Checkbox( glui, "Color", &wireframe, 1, control_cb );
	spinner  = new GLUI_Spinner( glui, "Segments:", &segments, 2, control_cb );
	spinner->set_int_limits( 3, 60 );
	edittext = new GLUI_EditText( glui, "Text:", text, 3, control_cb );
	GLUI_Panel *obj_panel = new GLUI_Panel( glui, "Object Type" );
	obj_panel->set_w(200);	
	radio = new GLUI_RadioGroup( obj_panel,&obj,4,control_cb );
	new GLUI_RadioButton( radio, "Sphere" );
	new GLUI_RadioButton( radio, "Torus" );
	new GLUI_RadioButton( radio, "Teapot" );
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );
 

  /* We register the idle callback with GLUI, *not* with GLUT */
  // GLUI_Master.set_glutIdleFunc( myGlutIdle );
  // GLUI_Master.set_glutIdleFunc( NULL );
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
	//TODO: Bug in drawing position...!!
	GLUI_Master.auto_set_viewport(); // Compensation for subwindows

 	// glViewport(0.0f, 0.0f, (GLfloat)w, (GLfloat)h);
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
	  case 'T': simulation.change_timestep(+0.001); break;
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
