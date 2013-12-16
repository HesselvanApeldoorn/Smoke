#include "visualization.hpp"
#include <iostream>

//------ VISUALIZATION CODE STARTS HERE -----------------------------------------------------------------

Visualization::Visualization()
{
    init_parameters();
}

void Visualization::init_parameters()
{
    options[DrawColor] = false; // don't draw color
    // color_dir = 0;            //use direction color-coding or not
    vec_scale = 1000;           //scaling of hedgehogs
    number_of_colors = 256;     // number of colors used
    options[DrawSmoke] = true;           //draw the smoke or not
    options[DrawVecs] = false;            //draw the vector field or not
    selected_colormap = Rainbow;
    clamp_min = 1;
    clamp_max = 256;
}
//rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
void Visualization::rainbow(float value,float* R,float* G,float* B)
{
   const float dx=0.8;
   if (value<0) value=0; if (value>1) value=1;

   value = (6-2*dx)*value+dx;
   *R = max(0.0f,(3-fabs(value-4)-fabs(value-5))/2);
   *G = max(0.0f,(4-fabs(value-2)-fabs(value-4))/2);
   *B = max(0.0f,(3-fabs(value-1)-fabs(value-2))/2);
}

//rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
void Visualization::fire(float value,float* R,float* G,float* B)
{
  if (value<0) value=0; if (value>1) value=1;
  if(value>=0.7) {
    *R = 0.8;
    *G = 0.8;
    *B = 0.8;
  } else if(value>=0.6){
    *R = 0.8;
    *G = 0.8;
    *B = 0;
  } else if(value>=0.3) {
    *R = 0.8;
    *G = 0;
    *B = 0;
  } else {
    *R = 0.0;
    *G = 0.0;
    *B = 0.0;
  }
}

//set_colormap: Sets three different types of colormaps
void Visualization::set_colormap(float vy)
{
    // if(vy!=0 && vy <0) cout << vy <<"\n";
    float R,G,B;
    switch(selected_colormap)
    {
        case BlackWhite: {R = G = B = vy;} break;
        case Rainbow: {rainbow(vy,&R,&G,&B);} break;
        case RedWhite: {R=1;G=1-vy; B=1-vy;} break;
        case Fire: {fire(vy,&R,&G,&B);} break;
    }
    R = round(R*(number_of_colors-1))/(number_of_colors-1);
    G = round(G*(number_of_colors-1))/(number_of_colors-1);
    B = round(B*(number_of_colors-1))/(number_of_colors-1);

    //clamping
    //max
    R = min(R,clamp_max/256.0f);
    G = min(G,clamp_max/256.0f);
    B = min(B,clamp_max/256.0f);
    //min
    R = max(R,clamp_min/256.0f);
    G = max(G,clamp_min/256.0f);
    B = max(B,clamp_min/256.0f);


    glColor3f(R,G,B);
}


//direction_to_color: Set the current color by mapping a direction vector (x,y), using
//                    the color mapping method 'method'. If method==1, map the vector direction
//                    using a rainbow colormap. If method==0, simply use the white color
void Visualization::direction_to_color(float x, float y, int method)
{
    float r,g,b,f;
    if (method)
    {
        f = atan2(y,x) / M_PI + 1;
        r = f;
        if(r > 1) r = 2 - r;
        g = f + (2/3);
        if(g > 2) g -= 2;
        if(g > 1) g = 2 - g;
        b = f + 2 * (2/3);
        if(b > 2) b -= 2;
        if(b > 1) b = 2 - b;
    }
    else
    { r = g = b = 1; }
    glColor3f(r,g,b);
}

void Visualization::draw_string(string text, int x, int y)
{
    glRasterPos2i(x,y);
    for(int i = 0; i < (int) text.length(); i++){ 
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text.data()[i]);
    }
}

void Visualization::draw_gradient(int nrRect, int winWidth, int winHeight, float rgbValues[][3])
{
    int barHeight = 20;
    int bottomSpace = 10;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int barWidth = winWidth/2;
    int barPiece = barWidth/nrRect;

   
    //Draw rectangles inluding gradient
    for(int i=0; i<nrRect;i++)
    {
        int barStart = winWidth/4+i*barPiece;
        glBegin(GL_QUADS); //Begin gl_quads

        glColor3f(rgbValues[i][0],rgbValues[i][1],rgbValues[i][2] );
        glVertex2f(barStart, barHeight+bottomSpace);  //Top left
        glVertex2f(barStart, bottomSpace); // Bottom left
        glColor3f(rgbValues[i+1][0],rgbValues[i+1][1],rgbValues[i+1][2] );
        glVertex2f(barStart+barPiece, bottomSpace); // Bottom right
        glVertex2f(barStart+barPiece, barHeight+bottomSpace); // Top right

        glEnd(); //End gl_quads
    }

    // Draw numbers
    glColor3f(1-rgbValues[0][0],1-rgbValues[0][1],1-rgbValues[0][2]);
    draw_string("0", winWidth/4,barHeight+bottomSpace+5);
    draw_string("1", 3*winWidth/4,barHeight+bottomSpace+5);
    

}
// Display color legend for current colormap
void Visualization::display_legend(int winWidth, int winHeight)
{

    switch(selected_colormap)
    {
        case BlackWhite: {
            float rgbValues[2][3] = {{0,0,0}, {1,1,1}};
            draw_gradient(1,winWidth, winHeight, rgbValues);
        } break;
        case Rainbow: {
            float rgbValues[3][3] = {{0,0,1}, {0,1,0},{1,0,0}};
            draw_gradient(2,winWidth, winHeight, rgbValues);
        } break;
        case RedWhite: {
            float rgbValues[2][3] = {{1,1,1}, {1,0,0}};
            draw_gradient(1,winWidth, winHeight, rgbValues);
        } break;
    }
}

//visualize: This is the main visualization function
void Visualization::visualize(Simulation const &simulation, int winWidth, int winHeight)
{
    int i, j, idx; double px,py;
    const int DIM = Simulation::DIM;
    fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  // Grid cell heigh

    float max_value=1, min_value=0;
    if(options[Scaling])
    {
        max_value=0;
        min_value=10;
        for(i=0; i<DIM*DIM-1; i++)
        {
            if(simulation.rho[i]>max_value) //find maximum
            {
                max_value=simulation.rho[i];
            }
            if(simulation.rho[i]<min_value) //find minimum
            {
                min_value=simulation.rho[i];
            }
        }
    }

    if (options[DrawSmoke])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        for (j = 0; j < DIM - 1; j++)           //draw smoke
        {
            glBegin(GL_TRIANGLE_STRIP);

            i = 0;
            px = wn + (fftw_real)i * wn;
            py = hn + (fftw_real)j * hn;
            idx = (j * DIM) + i;
            glColor3f(simulation.rho[idx],simulation.rho[idx],simulation.rho[idx]);
            glVertex2f(px,py);

            for (i = 0; i < DIM - 1; i++)
            {
                px = wn + (fftw_real)i * wn;
                py = hn + (fftw_real)(j + 1) * hn;
                idx = ((j + 1) * DIM) + i;
                set_colormap((simulation.rho[idx]-min_value)/(max_value-min_value));
                glVertex2f(px, py);
                px = wn + (fftw_real)(i + 1) * wn;
                py = hn + (fftw_real)j * hn;
                idx = (j * DIM) + (i + 1);
                set_colormap((simulation.rho[idx]-min_value)/(max_value-min_value));
                glVertex2f(px, py);
            }

            px = wn + (fftw_real)(DIM - 1) * wn;
            py = hn + (fftw_real)(j + 1) * hn;
            idx = ((j + 1) * DIM) + (DIM - 1);
            set_colormap((simulation.rho[idx]-min_value)/(max_value-min_value));
            glVertex2f(px, py);
            glEnd();
        }
    }

    if (options[DrawVecs])
    {
      glBegin(GL_LINES);                //draw velocities
      for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
        {
          idx = (j * DIM) + i;
          direction_to_color(simulation.vx[idx],simulation.vy[idx],options[DrawColor]);
          glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
          glVertex2f((wn + (fftw_real)i * wn) + vec_scale * simulation.vx[idx], (hn + (fftw_real)j * hn) + vec_scale * simulation.vy[idx]);
        }
      glEnd();
    }

    // Draw color legend
    if(options[DrawSmoke]) 
    {
        display_legend(winWidth, winHeight);
    }
}

void Visualization::toggle(Option option)
{
    options[option] = !options[option];
}


void Visualization::change_hedgehog(double scale)
{
    vec_scale *= scale;
}

bool Visualization::is_enabled(Option option) 
{
    return options[option];
}

void Visualization::enable(Option option) 
{
    options[option] = true;
}

void Visualization::disable(Option option) 
{
    options[option] = false;
}



// void Visualization::toggle_scalarcol()
// {
//  scalar_col++; 
//  if (scalar_col>COLOR_BANDS) scalar_col=COLOR_BLACKWHITE;
// }
