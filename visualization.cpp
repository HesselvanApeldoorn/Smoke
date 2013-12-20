#include "visualization.hpp"

//------ VISUALIZATION CODE STARTS HERE -----------------------------------------------------------------

Visualization::Visualization()
{
    init_parameters();
}

void Visualization::init_parameters()
{
    // color_dir = 0;            //use direction color-coding or not
    vec_scale = 1000;           //scaling of hedgehogs
    number_of_colors = 256;     // number of colors used
    options[DrawSmoke] = true;           //draw the smoke or not
    options[DrawVecs] = false;            //draw the vector field or not
    selected_colormap = Rainbow;
    selected_scalar = VelocityScalar;
    selected_vector = VelocityVector;
    clamp_min = 0;
    clamp_max = 1;
    number_of_glyphs_x = 12;
    number_of_glyphs_y = 12;
}
//rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
void Visualization::rainbow(float value,float* R,float* G,float* B)
{
   const float dx=0.8;
   if (value<0) value=0; if (value>1) value=1;  // clamp value in [0,1]

   value = (6-2*dx)*value+dx; // scale value to [dx, 6-dx]
   *R = max(0.0f,(3-fabs(value-4)-fabs(value-5))/2);
   *G = max(0.0f,(4-fabs(value-2)-fabs(value-4))/2);
   *B = max(0.0f,(3-fabs(value-1)-fabs(value-2))/2);
}

//rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
void Visualization::fire(float value,float* R,float* G,float* B)
{
  if(value>=0.7) {
    *R = 1.0;
    *G = 1.0;
    *B = 1.0;
  } else if(value>=0.5){
    *R = 1.0;
    *G = 1.0;
    *B = (value-0.6)*5; //fluent transition white to yellow
  } else if(value>=0.3) {
    *R = 1.0;
    *G = (value-0.3)*5; //fluent transition yellow to red
    *B = 0;
  } else {
    *R = (value)*(3+1.0/3.0); //fluent transition red to black
    *G = 0.0;
    *B = 0.0;
  }
}

//set_colormap: Sets three different types of colormaps
void Visualization::set_colormap(Simulation const &simulation, int idx, float min_value, float max_value)
{
    // if(vy!=0 && vy <0) cout << vy <<"\n";
    float value, R,G,B;


    switch (selected_scalar) 
    {
        case DensityScalar: 
        {
            value = simulation.rho[idx];
        }
        break;
        case VelocityScalar:
        {
            value = sqrt(simulation.vx[idx]*simulation.vx[idx] + simulation.vy[idx]*simulation.vy[idx])*10;
        }
        break;
        case ForceScalar: 
        {
            value = sqrt(simulation.fx[idx]*simulation.fx[idx] + simulation.fy[idx]*simulation.fy[idx])*10;
        }
        break;
        default: 
        {
            value = simulation.rho[idx];
        }
    }

  
    value = clamp(value, clamp_min, clamp_max); //clamping

    
    value = (value-clamp_min)/(clamp_max-clamp_min); // normalize clamp values 

    if(options[Scaling]) value = (value-min_value)/(max_value-min_value); //scaling

    switch(selected_colormap)
    {
        case BlackWhite: {R = G = B = value;} break;
        case Rainbow: {rainbow(value,&R,&G,&B);} break;
        case RedWhite: {R=1;G=1-value; B=1-value;} break;
        case Fire: {fire(value,&R,&G,&B);} break;
    }

    //Number of colours
    R = round(R*(number_of_colors-1))/(number_of_colors-1);
    G = round(G*(number_of_colors-1))/(number_of_colors-1);
    B = round(B*(number_of_colors-1))/(number_of_colors-1);


    glColor3f(R,G,B);
}


//direction_to_color: Set the current color by mapping a direction vector (x,y), using
//                    the color mapping method 'method'. If method==1, map the vector direction
//                    using a rainbow colormap. If method==0, simply use the white color
void Visualization::direction_to_color(float f, float min_value, float max_value)
{
    float r,g,b;
    f = clamp(f, clamp_min, clamp_max); //clamping

    
    f = (f-clamp_min)/(clamp_max-clamp_min); // normalize clamp values 

    if(options[Scaling]) f = (f-min_value)/(max_value-min_value); //scaling
    switch (selected_colormap)
    {
        case BlackWhite:{r = g = b = 1; } break;
        case Rainbow:
        {
            r = f;
            if(r > 1) r = 2 - r;
            g = f + (2.0/3.0);
            if(g > 2) g -= 2;
            if(g > 1) g = 2 - g;
            b = f + 2 * (2.0/3.0);
            if(b > 2) b -= 2;
            if(b > 1) b = 2 - b;
        } break;
        case RedWhite:
        {
            r=1;
            g=b=(1-f);
        } break;
        case Fire:
        {
            if(f>=0.7) {
                r = 1.0;
                g = 1.0;
                b = 1.0;
            } else if(f>=0.5){
                r = 1.0;
                g = 1.0;
                b = (f-0.5)*5; //fluent transition white to yellow
            } else if(f>=0.3) {
                r = 1.0;
                g = (f-0.3)*5; //fluent transition yellow to red
                b = 0;
            } else {
                r = (f)*(3+1.0/3.0); //fluent transition red to black
                g = 0.0;
                b = 0.0;
            }
        }
    }
    glColor3f(r,g,b);
}

void Visualization::draw_string(string text, int x, int y)
{
    glRasterPos2i(x,y);
    for(int i = 0; i < (int) text.length(); i++){ 
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text.data()[i]);
    }
}

void Visualization::draw_gradient(int nrRect, int winWidth, int winHeight, float rgbValues[][3], float min_value, float max_value)
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

    float min,max;
    if(max_value<clamp_max) {
        max = max_value;
    } else {
        max = clamp_max;
    }
    if(min_value>clamp_min) {
        min = min_value;
    } else {
        min = clamp_min;
    }
    if(max<min) {
        max = min;  // min cannot exceed max, clamp_min might be bigger than the max value in the field
    }
    ostringstream string_min;
    string_min << min;
    draw_string(string_min.str(), winWidth/4,barHeight+bottomSpace+5);
    ostringstream string_max;
    string_max << max;
    draw_string(string_max.str(), 3*winWidth/4,barHeight+bottomSpace+5);
    

}
// Display color legend for current colormap
void Visualization::display_legend(int winWidth, int winHeight, float min_value, float max_value)
{

    switch(selected_colormap)
    {
        case BlackWhite: {
            float rgbValues[2][3] = {{0,0,0}, {1,1,1}};
            draw_gradient(1,winWidth, winHeight, rgbValues, min_value, max_value);
        } break;
        case Rainbow: {
            float rgbValues[3][3] = {{0,0,1}, {0,1,0},{1,0,0}};
            draw_gradient(2,winWidth, winHeight, rgbValues, min_value, max_value);
        } break;
        case RedWhite: {
            float rgbValues[2][3] = {{1,1,1}, {1,0,0}};
            draw_gradient(1,winWidth, winHeight, rgbValues, min_value, max_value);
        } break;
        case Fire: {
            float rgbValues[4][3] = {{0,0,0}, {1,0,0}, {1,1,0}, {1,1,1}};
            draw_gradient(3,winWidth, winHeight, rgbValues, min_value, max_value);
        } break;
    }
}

void Visualization::draw_smoke(Simulation const &simulation, fftw_real wn, fftw_real hn, float min_value, float max_value)
{
    const int DIM = Simulation::DIM;
    double px,py;
    int i, j, idx;  
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (j = 0; j < DIM - 1; j++)           //draw smoke
    {
        glBegin(GL_TRIANGLE_STRIP);

        i = 0;
        px = wn + (fftw_real)i * wn;
        py = hn + (fftw_real)j * hn;
        idx = (j * DIM) + i;
        
        set_colormap(simulation, idx, min_value, max_value);
        glVertex2f(px,py);

        for (i = 0; i < DIM - 1; i++)
        {
            px = wn + (fftw_real)i * wn;
            py = hn + (fftw_real)(j + 1) * hn;
            idx = ((j + 1) * DIM) + i;
            set_colormap(simulation, idx, min_value, max_value);
            glVertex2f(px, py);
            px = wn + (fftw_real)(i + 1) * wn;
            py = hn + (fftw_real)j * hn;
            idx = (j * DIM) + (i + 1);
            set_colormap(simulation, idx, min_value, max_value);
            glVertex2f(px, py);
        }

        px = wn + (fftw_real)(DIM - 1) * wn;
        py = hn + (fftw_real)(j + 1) * hn;
        idx = ((j + 1) * DIM) + (DIM - 1);
        set_colormap(simulation, idx, min_value, max_value);
        glVertex2f(px, py);
        glEnd();
    }

}

void Visualization::draw_glyphs(float value_x, float value_y, fftw_real wn, fftw_real hn, float glyph_point_x, float glyph_point_y)
{
    switch(selected_glyph) {
        case Hedgehog: 
        {
            glVertex2f(wn + (fftw_real) glyph_point_x * wn, hn + (fftw_real) glyph_point_y * hn);
            glVertex2f((wn + (fftw_real) glyph_point_x * wn) + vec_scale * value_x, (hn + (fftw_real) glyph_point_y * hn) + vec_scale * value_y);
        } break;
        case Cone:
        {
            float angle = rad2deg(atan2(value_x,value_y));
            float size = sqrt(value_x*value_x+value_y*value_y)*10;

            glPushMatrix();
            glTranslatef(wn*glyph_point_x, hn*glyph_point_y, 0);
            glRotatef(angle, 1.0, 1.0, 0.0);

            glRotatef(90, 0.0, 0, 1.0);
            glutSolidCone(5, 15, 12, 12);
            glPopMatrix();
        }
    }
}

//visualize: This is the main visualization function
void Visualization::visualize(Simulation const &simulation, int winWidth, int winHeight)
{
    int i, j, idx; 
    const int DIM = Simulation::DIM;
    fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  // Grid cell heigh

    float max_value=10, min_value=0;

    if(options[Scaling])
    {
        max_value=0;
        min_value=10;

        for(i=0; i<DIM*DIM-1; i++)
        {
            float value;
            switch(selected_scalar)
            {
                case DensityScalar: 
                {
                    value = simulation.rho[i];
                } break;
                case VelocityScalar: 
                {
                    value = sqrt(simulation.vy[i]*simulation.vy[i]+simulation.vx[i]*simulation.vx[i])*10;

                } break;
                case ForceScalar: 
                {
                    value = sqrt(simulation.fy[i]*simulation.fy[i]+simulation.fx[i]*simulation.fx[i])*10;
                } break;
            }

            if(value>max_value) //find maximum
            {
                max_value=value;
            }
            if(value<min_value) //find minimum
            {
                min_value=value;
            }
        }
    }

    if (options[DrawSmoke])
    {
        draw_smoke(simulation,wn,hn, min_value, max_value);
    }

    if (options[DrawVecs])
    {
        glBegin(GL_LINES);                //draw vectors
        for (i = 0; i < number_of_glyphs_x; i++)
        {
            for (j = 0; j < number_of_glyphs_y; j++)
            {
                idx = (j * number_of_glyphs_y) + i; // take 4 surrounding points: floor(glyphx), floor(glyphy); ceil(glyphx), floor(glyphy); floor(glyphx), ceil(glyphy); ceil(glyphx), ceil(glyphy);

                float glyph_point_x = (float)i*((float)DIM/(float)number_of_glyphs_x) + 0.00001;
                float glyph_point_y = (float)j*((float)DIM/(float)number_of_glyphs_y) + 0.00001;
                
                // if(glyph_point_x%1==0)
                // if(glyph_point_x%1==0)
                
                fftw_real *dataset_x, *dataset_y;
                switch(selected_scalar)
                {
                    case DensityScalar: 
                    {
                        dataset_x=simulation.rho; dataset_y=simulation.rho;
                    } break;
                    case VelocityScalar: 
                    {
                        dataset_x=simulation.vx; dataset_y=simulation.vy;
                    } break;
                    case ForceScalar: 
                    {
                        dataset_x=simulation.fx; dataset_y=simulation.fy;
                    } break;
                }
                int idx_lower_left = floor(glyph_point_x)+DIM*floor(glyph_point_y);
                int idx_lower_right = ceil(glyph_point_x)+DIM*floor(glyph_point_y);
                int idx_upper_left = floor(glyph_point_x)+DIM*ceil(glyph_point_y);
                int idx_upper_right = ceil(glyph_point_x)+DIM*ceil(glyph_point_y);
                // if (glyph_point_x > 58 && glyph_point_x < 60 && glyph_point_y > 58 && glyph_point_y < 60) {
                // cout << idx_lower_left << " krakra" << idx_upper_right << "\n";
                // }

                float bottom_value_x = (ceil(glyph_point_x)-glyph_point_x)*dataset_x[idx_lower_left]+(glyph_point_x-floor(glyph_point_x))*dataset_x[idx_lower_right];
                float bottom_value_y = (ceil(glyph_point_x)-glyph_point_x)*dataset_y[idx_lower_left]+(glyph_point_x-floor(glyph_point_x))*dataset_y[idx_lower_right];
                float top_value_x = (ceil(glyph_point_x)-glyph_point_x)*dataset_x[idx_upper_left]+(glyph_point_x-floor(glyph_point_x))*dataset_x[idx_upper_right];
                float top_value_y = (ceil(glyph_point_x)-glyph_point_x)*dataset_y[idx_upper_left]+(glyph_point_x-floor(glyph_point_x))*dataset_y[idx_upper_right];
                // if (glyph_point_x > 58 && glyph_point_x < 60 && glyph_point_y > 58 && glyph_point_y < 60) {
                //     cout << bottom_value_x << "  values bottom x" << bottom_value_y << " values bottom y" <<  "\n"; // altijd 0 nu
                //     cout << top_value_x << "  values top x" << top_value_y << " values top y" <<  "\n"; // altijd 0 nu
                // }
                float value_x = (ceil(glyph_point_y)-glyph_point_y)*bottom_value_x+(glyph_point_y-floor(glyph_point_y))*top_value_x;
                float value_y = (ceil(glyph_point_y)-glyph_point_y)*bottom_value_y+(glyph_point_y-floor(glyph_point_y))*top_value_y;
                // if (glyph_point_x > 58 && glyph_point_x < 60 && glyph_point_y > 58 && glyph_point_y < 60) {
                //     cout << value_x << "  values x y" << value_y << "\n"; // altijd 0 nu
                // }

                float f;
                if(selected_scalar==DensityScalar) {
                    f = value_x;
                } else {
                    f = atan2(value_y,value_x) / M_PI + 1;

                }
                direction_to_color(f, min_value, max_value);
                switch(selected_vector)
                {
                    case VelocityVector: {value_x=simulation.vx[idx]; value_y=simulation.vy[idx];} break;
                    case ForceVector: {value_x=simulation.fx[idx]; value_y=simulation.fy[idx];} break;
                }
                draw_glyphs(value_x, value_y, wn, hn, glyph_point_x, glyph_point_y);
            }
        }
        glEnd();
    }

    display_legend(winWidth, winHeight, min_value, max_value);

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
