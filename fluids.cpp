// Usage: Drag with the mouse to add smoke to the fluid. This will also move a "rotor" that disturbs
//        the velocity field at the mouse location. Press the indicated keys to change options
//--------------------------------------------------------------------------------------------------


#include "fluids.hpp"

using namespace std;

#define RESET_VALUES 0 //For resetting values in glui
#define TIMESTEP 1 //For timestep spinner in glui
#define HEDGEHOGSCALE 2 // For hedgehog spinner in glui
#define VISCOSITY 3 // For viscosity spinner in glui
#define NUMBEROFCOLORS 4 // For viscosity spinner in glui
#define CLAMPMIN 5 // For clamp min spinner in glui
#define CLAMPMAX 6 // For clamp min spinner in glui
#define NRGLYPHSX 7 // For number of glyphs x spinner in glui
#define NRGLYPHSY 8 // For number of glyphs y spinner in glui

Simulation Fluids::simulation;		
Visualization Fluids::visualization;	


int Fluids::winWidth;
int Fluids::winHeight;		

int Fluids::main_window;

const int Fluids::GUI_WIDTH = 200;
//Spinners in glui
GLUI_Spinner *timestep_spinner;
GLUI_Spinner *hedgehog_spinner;
GLUI_Spinner *viscosity_spinner;
GLUI_Spinner *nrcolor_spinner;
GLUI_Spinner *clampmin_spinner;
GLUI_Spinner *clampmax_spinner;
GLUI_Spinner *glyph_x_spinner;
GLUI_Spinner *glyph_y_spinner;

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
	
	glutInitWindowPosition( 400, 200 );
	glutInitWindowSize(1000,800);

	main_window = glutCreateWindow("Real-time smoke simulation and visualization");

	glutDisplayFunc(display);
	glutMouseFunc(click);
	glutMotionFunc(drag);

	glutKeyboardFunc(keyboard);

	GLUI_Master.set_glutReshapeFunc(reshape);  
	GLUI_Master.set_glutIdleFunc(update);
	

// 	glEnable(GL_COLOR_MATERIAL);
//     glEnable (GL_DEPTH_TEST);
//     glEnable (GL_LIGHTING);
//     glEnable (GL_LIGHT0);

// //Add directed light
// 	GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
// 	//Coming from the direction (-1, 0.5, 0.5)
// 	GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
// glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor1);

// glShadeModel(GL_SMOOTH); glEnable(GL_COLOR_MATERIAL); glEnable(GL_DEPTH_TEST);

	simulation.init_simulation();	//initialize the simulation data structures

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
	cout << "V/v:   increase decrease fluid viscosity\n";
	cout << "x:     toggle drawing matter on/off\n";
	cout << "y:     toggle drawing hedgehogs on/off\n";
	// cout << "m:     toggle thru scalar coloring\n";
	cout << "a:     toggle the animation on/off\n";
	cout << "r:     Reset to default parameters\n";
	cout << "q:     quit\n\n";
}

void Fluids::reset_values()
{
	simulation.init_parameters();
	visualization.init_parameters();

	GLUI_Master.sync_live_all(); 	// sync live variables

}


void Fluids::glui_callback( int control )
{
    switch(control)
    {
    	case RESET_VALUES: reset_values();break;
    	case TIMESTEP: simulation.dt = timestep_spinner->get_float_val(); break;
    	case HEDGEHOGSCALE: visualization.vec_scale = hedgehog_spinner->get_float_val(); break;
		case VISCOSITY: simulation.visc = viscosity_spinner->get_float_val(); break;
		case NUMBEROFCOLORS: visualization.number_of_colors = nrcolor_spinner->get_int_val(); break;
		//Don't let the min value exceed the max value and let the max value be higher than the min value
		case CLAMPMIN: clampmin_spinner->set_float_limits(0,visualization.clamp_max); break;
		case CLAMPMAX: clampmax_spinner->set_float_limits(visualization.clamp_min,10); break;
    }

  
}

void Fluids::build_gui()
{
	GLUI *glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);
	glui->set_main_gfx_window(main_window);  // main_window is the main gfx window


	GLUI_Panel *options_panel = glui->add_panel("Options");	
	glui->add_checkbox_to_panel(options_panel, "Draw Smoke", &visualization.options[Visualization::DrawSmoke] );
	glui->add_checkbox_to_panel(options_panel, "Draw Vector", &visualization.options[Visualization::DrawVecs] );
	glui->add_checkbox_to_panel(options_panel, "Draw Streamlines", &visualization.options[Visualization::DrawStreamlines] );
	glui->add_checkbox_to_panel(options_panel, "Scaling", &visualization.options[Visualization::Scaling] );
	glui->add_checkbox_to_panel(options_panel, "Freeze", &simulation.frozen );
	options_panel->set_w(Fluids::GUI_WIDTH);

	//Time step spinner
	timestep_spinner = glui->add_spinner("Time Step",GLUI_SPINNER_FLOAT , &simulation.dt, TIMESTEP, glui_callback );
	timestep_spinner->set_speed(0.001); 
	timestep_spinner->set_float_limits(0,100);
	timestep_spinner->set_float_val(simulation.dt);

	//Hedgehog scale spinner
	hedgehog_spinner = glui->add_spinner("Hedgehog Scale",GLUI_SPINNER_FLOAT , &visualization.vec_scale, HEDGEHOGSCALE, glui_callback );
	hedgehog_spinner->set_speed(1); 
	hedgehog_spinner->set_float_limits(0,50000);
	hedgehog_spinner->set_float_val(visualization.vec_scale);

	//Fluid viscosity spinner
	viscosity_spinner = glui->add_spinner("Fluid viscosity",GLUI_SPINNER_FLOAT , &simulation.visc, VISCOSITY, glui_callback );
	viscosity_spinner->set_speed(0.1); 
	viscosity_spinner->set_float_limits(0.001,100);
	viscosity_spinner->set_float_val(simulation.visc);

	//number of colors spinner
	nrcolor_spinner = glui->add_spinner("Number of colors",GLUI_SPINNER_INT , &visualization.number_of_colors, NUMBEROFCOLORS, glui_callback );
	nrcolor_spinner->set_speed(1); 
	nrcolor_spinner->set_int_limits(2,256);
	nrcolor_spinner->set_int_val(visualization.number_of_colors);

	GLUI_Panel *color_panel = glui->add_panel("Color map");	
	GLUI_RadioGroup *color_radio = glui->add_radiogroup_to_panel(color_panel, &visualization.selected_colormap);
	glui->add_radiobutton_to_group(color_radio, "Black/White");
	glui->add_radiobutton_to_group(color_radio, "Rainbow");
	glui->add_radiobutton_to_group(color_radio, "Red/White");
	glui->add_radiobutton_to_group(color_radio, "Fire");

	GLUI_Panel *clamp_panel = glui->add_panel("Clamping");	
	clampmin_spinner = glui->add_spinner_to_panel(clamp_panel, "min", GLUI_SPINNER_FLOAT , &visualization.clamp_min, CLAMPMIN, glui_callback );	
	clampmin_spinner->set_speed(0.01); 
	clampmin_spinner->set_float_limits(visualization.clamp_min,visualization.clamp_max);
	clampmin_spinner->set_float_val(visualization.clamp_min);
	clampmax_spinner = glui->add_spinner_to_panel(clamp_panel, "max", GLUI_SPINNER_FLOAT , &visualization.clamp_max, CLAMPMAX, glui_callback );	
	clampmax_spinner->set_speed(0.01); 
	clampmax_spinner->set_float_limits(visualization.clamp_min,10);
	clampmax_spinner->set_float_val(visualization.clamp_max);

	GLUI_Panel *scalar_panel = glui->add_panel("Scalar field");	
	GLUI_RadioGroup *scalar_radio = glui->add_radiogroup_to_panel(scalar_panel, &visualization.selected_scalar);
	glui->add_radiobutton_to_group(scalar_radio, "Density (Rho)");
	glui->add_radiobutton_to_group(scalar_radio, "Velocity (vx,vy)");
	glui->add_radiobutton_to_group(scalar_radio, "Force (fx, fy)");

	GLUI_Panel *vector_panel = glui->add_panel("Vector field");	
	GLUI_RadioGroup *vector_radio = glui->add_radiogroup_to_panel(vector_panel, &visualization.selected_vector);
	glui->add_radiobutton_to_group(vector_radio, "Velocity (vx,vy)");
	glui->add_radiobutton_to_group(vector_radio, "Force (fx, fy)");
	glui->add_radiobutton_to_group(vector_radio, "Gradient");

	GLUI_Rollout *glyph_rollout = new GLUI_Rollout(glui, "Glyphs", true );

	GLUI_Panel *nr_glyph_panel = new GLUI_Panel(glyph_rollout, "Number of glyphs");	
	glyph_x_spinner = glui->add_spinner_to_panel(nr_glyph_panel, "X", GLUI_SPINNER_INT , &visualization.number_of_glyphs_x, NRGLYPHSX, glui_callback );	
	glyph_x_spinner->set_speed(1); 
	glyph_x_spinner->set_int_limits(0,Simulation::DIM);
	glyph_x_spinner->set_int_val(visualization.number_of_glyphs_x);
	glyph_y_spinner = glui->add_spinner_to_panel(nr_glyph_panel, "Y", GLUI_SPINNER_INT , &visualization.number_of_glyphs_y, NRGLYPHSY, glui_callback );	
	glyph_y_spinner->set_speed(1); 
	glyph_y_spinner->set_int_limits(0,Simulation::DIM);
	glyph_y_spinner->set_int_val(visualization.number_of_glyphs_y);

	GLUI_Panel *type_glyph_panel = new GLUI_Panel(glyph_rollout, "Glyph type");	
	GLUI_RadioGroup *glyph_radio = glui->add_radiogroup_to_panel(type_glyph_panel, &visualization.selected_glyph);
	glui->add_radiobutton_to_group(glyph_radio, "Hedgehog");
	glui->add_radiobutton_to_group(glyph_radio, "Cone");
	glui->add_radiobutton_to_group(glyph_radio, "Arrow");


	new GLUI_Button( glui, "Reset", RESET_VALUES, glui_callback ); //Reset button
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit ); //Quit button
}

//keyboard: Handle key presses
void Fluids::keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  // case 's': simulation.add_seedpoint(); break;
	  case 't': simulation.change_timestep(-0.001); break;
	  case 'T': simulation.change_timestep(+0.001); break;
	  case 'H': visualization.change_hedgehog(1.2); break;
	  case 'h': visualization.change_hedgehog(0.8); break;
	  case 'V': simulation.change_viscosity(5); break;
	  case 'v': simulation.change_viscosity(0.2);; break;
	  case 'x': visualization.toggle(Visualization::DrawSmoke);
		    if (visualization.is_enabled(Visualization::DrawSmoke))
		    	visualization.enable(Visualization::DrawVecs); break;
	  case 'y': visualization.toggle(Visualization::DrawVecs);
		    if (visualization.is_enabled(Visualization::DrawVecs))
		    	visualization.enable(Visualization::DrawSmoke); break;
	  //case 'm': visualization.toggle_scalarcol(); break;
	  case 'a': simulation.toggle_frozen(); break;
  	  case 'r': reset_values(); break;
	  case 'q': exit(0);
	}

	GLUI_Master.sync_live_all(); 	// sync live variables

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
	glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -100,100);
	winWidth = w; winHeight = h;
}


// drag: When the user drags with the mouse, add a force that corresponds to the direction of the mouse
//       cursor movement. Also inject some new matter into the field at the mouse location.
void Fluids::drag(int mx, int my)
{
	int xi,yi,X,Y; double  dx, dy, len;
	static int lmx=0,lmy=0;				//remembers last mouse location

	// Compute the array index that corresponds to the cursor location
	xi = (int)clamp((double)(Simulation::DIM + 1) * ((double)mx / (double)(winWidth-Fluids::GUI_WIDTH)));
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

void Fluids::click(int button, int state, int mx, int my)
{

	if (button == GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
    	my = winHeight-my;
    	if(visualization.options[Visualization::DrawStreamlines]) simulation.add_seedpoint(mx,my);
    }
}
