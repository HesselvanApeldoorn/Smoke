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
    options[DrawSmoke] = false;           //draw the smoke or not
    options[DrawVecs] = true;            //draw the vector field or not
    selected_stream = StreamLine;
    options[Slices] = false;
    options[Scaling] = false;
    selected_colormap = Rainbow;
    selected_scalar = VelocityScalar;
    selected_vector = VelocityVector;
    selected_glyph = Hedgehog;
    clamp_min = 0;
    clamp_max = 1;
    number_of_glyphs_x = Simulation::DIM;
    number_of_glyphs_y = Simulation::DIM;
    number_of_opaque = 1;
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
void Visualization::direction_to_color(float f, float min_value, float max_value, float max_slices_value)
{
    float r,g,b;
    f = clamp(f, clamp_min, clamp_max); //clamping

    
    f = (f-clamp_min)/(clamp_max-clamp_min); // normalize clamp values 

    glEnable(GL_COLOR_MATERIAL);
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

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
    if(options[Slices]) 
    {
        glColor4f(r,g,b,(f/max_slices_value)*Visualization::number_of_opaque);

    } else
    {
       glColor3f(r,g,b);
    }
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

void Visualization::interpolation(fftw_real *dataset_x, fftw_real* dataset_y, int i, int j, float *value_x, float *value_y, float *glyph_point_x, float *glyph_point_y)
{
    int DIM = Simulation::DIM;
    *glyph_point_x = (float)i*((float)DIM/(float)number_of_glyphs_x);
    *glyph_point_y = (float)j*((float)DIM/(float)number_of_glyphs_y);


    int idx_lower_left = floor(*glyph_point_x)+DIM*floor(*glyph_point_y);
    int idx_lower_right = ceil(*glyph_point_x)+DIM*floor(*glyph_point_y);
    int idx_upper_left = floor(*glyph_point_x)+DIM*ceil(*glyph_point_y);
    int idx_upper_right = ceil(*glyph_point_x)+DIM*ceil(*glyph_point_y);

    float bottom_value_x, bottom_value_y, top_value_x, top_value_y;
    bottom_value_x = top_value_x = dataset_x[idx_lower_left];
    bottom_value_y = top_value_y = dataset_y[idx_lower_left];
    if(*glyph_point_x != (int) *glyph_point_x) // check if whole number
    {
        float ceil_x = (ceil(*glyph_point_x)-*glyph_point_x);
        float floor_x = (*glyph_point_x-floor(*glyph_point_x));
        bottom_value_x = ceil_x*dataset_x[idx_lower_left]+floor_x*dataset_x[idx_lower_right];
        bottom_value_y = ceil_x*dataset_y[idx_lower_left]+floor_x*dataset_y[idx_lower_right];
        top_value_x =    ceil_x*dataset_x[idx_upper_left]+floor_x*dataset_x[idx_upper_right];
        top_value_y =    ceil_x*dataset_y[idx_upper_left]+floor_x*dataset_y[idx_upper_right];
    } 

    *value_x = bottom_value_x;
    *value_y = bottom_value_y;
    if(*glyph_point_y != (int) *glyph_point_y) // check if whole number
    {
        float ceil_y = (ceil(*glyph_point_y)-*glyph_point_y);
        float floor_y = (*glyph_point_y-floor(*glyph_point_y));
        *value_x = ceil_y*bottom_value_x+floor_y*top_value_x;
        *value_y = ceil_y*bottom_value_y+floor_y*top_value_y;
    }
}

void Visualization::vector_gradient(fftw_real *dataset_x, fftw_real* dataset_y, int i, int j, float *value_x, float *value_y, float *glyph_point_x, float *glyph_point_y, float max_value)
{

    int DIM = Simulation::DIM;
    *glyph_point_x = (float)i*((float)DIM/(float)number_of_glyphs_x);
    *glyph_point_y = (float)j*((float)DIM/(float)number_of_glyphs_y);

    // calculate the indexes of the nearest 4 gridpoints
    int idx_lower_left = floor(*glyph_point_x)+DIM*floor(*glyph_point_y);
    int idx_lower_right = ceil(*glyph_point_x)+DIM*floor(*glyph_point_y);
    int idx_upper_left = floor(*glyph_point_x)+DIM*ceil(*glyph_point_y);
    int idx_upper_right = ceil(*glyph_point_x)+DIM*ceil(*glyph_point_y);
    if(*glyph_point_x == (int) *glyph_point_x && *glyph_point_y == (int) *glyph_point_y) // check if vector lies exactly on gridpoint
    {
        idx_lower_left = (idx_lower_left-1+DIM*DIM-DIM)%(DIM*DIM);
        idx_lower_right = (idx_lower_right+1+DIM*DIM-DIM)%(DIM*DIM);
        idx_upper_left = (idx_upper_left-1+DIM*DIM+DIM)%(DIM*DIM);
        idx_upper_right = (idx_upper_right+1+DIM*DIM+DIM)%(DIM*DIM);
    } else if (*glyph_point_x == (int) *glyph_point_x)  // check if vector lies on gridpoint in x but not in y
    {
        idx_lower_left = (idx_lower_left-1+DIM*DIM)%(DIM*DIM);
        idx_lower_right = (idx_lower_right+1+DIM*DIM)%(DIM*DIM);
        idx_upper_left = (idx_upper_left-1+DIM*DIM)%(DIM*DIM);
        idx_upper_right = (idx_upper_right+1+DIM*DIM)%(DIM*DIM);
    } else if (*glyph_point_y == (int) *glyph_point_y) // check if vector lies on gridpoint in y but not in x
    {
        idx_lower_left = (idx_lower_left+DIM*DIM-DIM)%(DIM*DIM);
        idx_lower_right = (idx_lower_right+DIM*DIM-DIM)%(DIM*DIM);
        idx_upper_left = (idx_upper_left+DIM*DIM+DIM)%(DIM*DIM);
        idx_upper_right = (idx_upper_right+DIM*DIM+DIM)%(DIM*DIM);
    } // else vector lies in between 4 gridpoints, indexes are okay as they are now

    if (selected_scalar == VelocityScalar)
    {
        *value_x = (dataset_x[idx_upper_right]-dataset_x[idx_upper_left])-(dataset_x[idx_lower_right]-dataset_x[idx_lower_left]);
        *value_y = (dataset_y[idx_upper_right]-dataset_y[idx_upper_left])-(dataset_y[idx_lower_right]-dataset_y[idx_lower_left]);    
    } else // selected_scalar == DensityScalar
    { // TODO: fix scaling of glyphs
        float right_x = (dataset_x[idx_upper_right]+dataset_x[idx_lower_right])/2;
        float left_x = (dataset_x[idx_upper_left]+dataset_x[idx_lower_left])/2;
        float top_y = (dataset_y[idx_upper_right]+dataset_y[idx_upper_left])/2;
        float bottom_y = (dataset_y[idx_lower_right]+dataset_y[idx_lower_left])/2;

        // since dataset_x and dataset_y are equal, a difference in computing the derivatives has been used x = dx dy and y = dy dx
        *value_x = right_x - left_x;
        *value_y = top_y - bottom_y;
        *value_x /= max_value * 5;
        *value_y /= max_value * 5;
    }

}

void Visualization::draw_glyphs(float value_x, float value_y, fftw_real wn, fftw_real hn, float glyph_point_x, float glyph_point_y, int z)
{            

    float multiplier = Simulation::DIM/(sqrt(number_of_glyphs_y*number_of_glyphs_x)*3); // divided by 3 to make cones not overlap 
    float x1 = wn + (fftw_real) glyph_point_x * wn;
    float y1 = hn + (fftw_real) glyph_point_y * hn;
    float x2 = x1 + multiplier * vec_scale * value_x; 
    float y2 = y1 + multiplier * vec_scale * value_y;
    
    z*=25+1; // Spacing between glyps

    switch(selected_glyph) {
        case Hedgehog: 
        {
           glBegin(GL_LINES);
            glVertex3f(x1, y1,z);
            glVertex3f(x2, y2,z);
           glEnd();
        } break;
        case Cone:
        {
            float angle = rad2deg(atan2(value_y,value_x));
            float size = sqrt(value_x*value_x+value_y*value_y)*vec_scale;

            glPushMatrix();
            glTranslatef(wn*glyph_point_x, hn*glyph_point_y, z);
            glRotatef(angle, 0.0, 0.0, 1.0);
            glRotatef(270.0, 1.0, 0.0, 0.0);


            glutSolidCone(3*multiplier+5, size*multiplier, 12,12);
            glPopMatrix();
        } break;
        case Arrow:
        {
    
            //arrowhead
            glBegin(GL_TRIANGLES);
             double join_angle = M_PI / 6.0; // 30 degrees
             double stem_angle = atan2(y2-y1, x2-x1);
             int w = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))/1.5;
             glVertex3f(x2, y2, z);
             glVertex3f(x2 - w*cos(stem_angle+join_angle), y2 - w*sin(stem_angle+join_angle), z);
             glVertex3f(x2 - w*cos(stem_angle-join_angle), y2 - w*sin(stem_angle-join_angle), z);
            glEnd();

            //arrow line
            glBegin(GL_LINES);
             glVertex3f(x1, y1, z);
             glVertex3f(x2, y2, z);
            glEnd();
            
        }
    }
}

void Visualization::draw_streamlines(Simulation const &simulation, float winWidth, float winHeight, float wn, float hn, float min_value, float max_value, int z, float max_slices_value)
{
    const size_t segments_per_line = Simulation::STREAMLINE_LENGTH;  // fixed max segments

    const float xscale = static_cast<float>(Simulation::DIM) / winWidth;
    const float yscale = static_cast<float>(Simulation::DIM) / winHeight;

    // grid render size in pixels
    const float grid_area_w = winWidth - 2.0 * wn;
    const float grid_area_h = winHeight - 2.0 * hn;

    z*=25+1; // Spacing between streamlines

    float window_correction = (winWidth-200)*0.0015625; 

    for (int i = 0; i < Simulation::seedpoints.size(); ++i)
    {
  
       
        Vector2 p0  = Simulation::seedpoints[i]; // seedpoint
        Vector2 p1;
        size_t segments = 0;
        while(segments < segments_per_line)
        {
            if (p0.x < wn) p0.x += grid_area_w;
            if (p0.y < hn) p0.y += grid_area_h;
            if (p0.x >= (wn + winWidth)) p0.x -= grid_area_w;
            if (p0.y >= winHeight - hn) p0.y -= grid_area_h;

            // nearest-neighbor interpolation
            size_t i = static_cast<int>(p0.x * xscale);
            size_t j = static_cast<int>(p0.y * yscale);

            size_t idx = j * (Simulation::DIM-1) + i;
            // velocity at nearest grid location
            Vector2 velocity = Vector2(simulation.vx[idx], simulation.vy[idx]);

            if (velocity.length() > 0) // don't divide by zero
                velocity.normalize();

            p1 = p0 + velocity*5;

            float f = atan2(simulation.vy[idx],simulation.vx[idx]) / M_PI + 1;

            direction_to_color(f, min_value, max_value, max_slices_value);

            if(segments==0) {
                glBegin(GL_QUADS); //Begin gl_quads
                    glVertex3f(p0.x*window_correction-2.5, p0.y+2.5,z);  //Top left
                    glVertex3f(p0.x*window_correction-2.5, p0.y-2.5,z); // Bottom left
                    glVertex3f(p0.x*window_correction+2.5, p0.y-2.5,z); // Bottom right
                    glVertex3f(p0.x*window_correction+2.5, p0.y+2.5,z); // Top right
                glEnd(); //End gl_quads
            }
            glLineWidth((GLfloat)5);

            glBegin(GL_LINES);
                glVertex3f(p0.x*window_correction, p0.y,z);
                glVertex3f(p1.x*window_correction, p1.y,z);
            glEnd();
            glLineWidth(1.0f);

            segments++;
            p0=p1;

        }
    }
}

void Visualization::draw_streamsurfaces(Simulation const &simulation, float winWidth, float winHeight, float wn, float hn, float min_value, float max_value)
{
    float window_correction = (winWidth-200)*0.0015625; 
    const float xscale = static_cast<float>(Simulation::DIM) / winWidth;
    const float yscale = static_cast<float>(Simulation::DIM) / winHeight;

    for (int i = 0; i < simulation.stream_surfaces.size(); ++i)
    {

        for(int j=simulation.slices.size()-1; j>=0;j--) 
        {


            // TODO: fix running out of screen
            // if (p0.x < wn) p0.x += grid_area_w;
            // if (p0.y < hn) p0.y += grid_area_h;
            // if (p0.x >= (wn + winWidth)) p0.x -= grid_area_w;
            // if (p0.y >= winHeight - hn) p0.y -= grid_area_h;
            for(int streampoint=0; streampoint<7; streampoint++) 
            {

                Vector2 p1_current = simulation.stream_surfaces[i].seed_points[streampoint];
                Vector2 p2_current = simulation.stream_surfaces[i].seed_points[streampoint+1];


                

                // nearest-neighbor interpolation
                size_t ii = static_cast<int>(p1_current.x * xscale);
                size_t jj = static_cast<int>(p1_current.y * yscale);

                size_t idx = jj * (Simulation::DIM-1) + ii;
                // velocity at nearest grid location
                Vector2 velocity = Vector2(simulation.slices[j].vx[idx], simulation.slices[j].vy[idx]);

                // cout << "\nbe4 normalize" << velocity.x;

                if (velocity.length() > 0) velocity.normalize();
                               
                // cout << ", after normalize" << velocity.y;

                Vector2 p1_next = p1_current + velocity*5 ; 

                // cout << "\nI: " << i << ", j: " << j << ", streampoint: " << streampoint;

                if (p1_next.x < wn) p1_next.x = -1;
                if (p1_next.y < hn) p1_next.x = -1;
                if (p1_next.x >= (wn + winWidth)) p1_next.x = -1;
                if (p1_next.y >= winHeight - hn) p1_next.x = -1;


                if(p1_next.x!=-1)
                {
                    ii = static_cast<int>(p2_current.x * xscale);
                    jj = static_cast<int>(p2_current.y * yscale);

                    idx = jj * (Simulation::DIM-1) + ii;
                    // velocity at nearest grid location
                    velocity = Vector2(simulation.slices[j].vx[idx], simulation.slices[j].vy[idx]);

                    if (velocity.length() > 0) velocity.normalize();
                    
                    Vector2 p2_next = p2_current + velocity*5 ; 

                    if (p2_next.x < wn) p2_next.x = -1;
                    if (p2_next.y < hn) p2_next.x = -1;
                    if (p2_next.x >= (wn + winWidth)) p2_next.x = -1;
                    if (p2_next.y >= winHeight - hn) p2_next.x = -1;

                    if(p2_next.x!=-1)
                    {
                        Vector2 vertex[4] = {p1_next, p1_current, p2_next, p2_current};
                        Vector2 normal(0,0) ;


                        //Newell's method

                        for (int l=0; l<4; l++)
                        {
                            int k = (l+1)%4;
                            float vertex_l = (j+l%2)*25;
                            float vertex_k = (j+k%2)*25;
                            normal.x += (vertex[l].y - vertex[k].y)*(vertex_l + vertex_k);
                            normal.y += (vertex_l - vertex_k)*(vertex[l].x + vertex[k].x);
                        //     normal.z += (vertex[i].x - vertex[j].x)
                        //                *(vertex[i].y + vertex[j].y);
                        }
                        normal.normalize() ;


                        float f = atan2(normal.y,normal.x) / M_PI + 1;
                        direction_to_color(f, min_value, max_value, 0);


                        glBegin(GL_QUADS); //Begin gl_quads
                            glVertex3f(p1_next.x*window_correction, p1_next.y,j*25);  //Top left
                            glVertex3f(p1_current.x*window_correction, p1_current.y,(j+1)*25); // Bottom left
                            glVertex3f(p2_current.x*window_correction, p2_current.y,(j+1)*25); // Bottom right
                            glVertex3f(p2_next.x*window_correction, p2_next.y,j*25); // Top right
                        glEnd(); //End gl_quads

                        simulation.stream_surfaces[i].seed_points[streampoint] = p1_next;
                // if (i==6) simulation.stream_surfaces[i].seed_points[streampoint+1] = p2_next;
                    }
                }
            }
        }
    }
}

void Visualization::apply_scaling(Simulation const &simulation, float *min_value, float *max_value)
{
    const int DIM = Simulation::DIM;
    *max_value=0;
    *min_value=10;

    for(int i=0; i<DIM*DIM-1; i++)
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

        if(value>*max_value) //find maximum
        {
            *max_value=value;
        }
        if(value<*min_value) //find minimum
        {
            *min_value=value;
        }
    }
}
void Visualization::draw_vectors(fftw_real *dataset_x_scalar, fftw_real *dataset_y_scalar, fftw_real *dataset_x_vector, fftw_real *dataset_y_vector, fftw_real wn, fftw_real hn, float min_value, float max_value, int z, float max_slices_value)
{
    int i,j;
    for (i = 0; i < number_of_glyphs_x; i++)
    {
        for (j = 0; j < number_of_glyphs_y; j++)
        {
            float value_x, value_y, glyph_point_x, glyph_point_y;

            interpolation(dataset_x_scalar, dataset_y_scalar, i,j, &value_x, &value_y, &glyph_point_x, &glyph_point_y);

            float f;
            if(selected_scalar==DensityScalar) {
                f = value_x;
            } else {
                f = atan2(value_y,value_x) / M_PI + 1;
            }

           direction_to_color(f, min_value, max_value, max_slices_value);

            if (selected_vector != GradientVector) 
                interpolation(dataset_x_vector, dataset_y_vector, i,j, &value_x, &value_y, &glyph_point_x, &glyph_point_y);
            else
                vector_gradient(dataset_x_scalar, dataset_y_scalar, i, j, &value_x, &value_y, &glyph_point_x, &glyph_point_y, max_value);
            draw_glyphs(value_x, value_y, wn, hn, glyph_point_x, glyph_point_y, z);
        }
    }
}

//visualize: This is the main visualization function
void Visualization::visualize(Simulation const &simulation, int winWidth, int winHeight)
{
    const int DIM = Simulation::DIM;
    fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  // Grid cell heigh

    float max_value=10, min_value=0;

    if(options[Scaling])
    {
       apply_scaling(simulation, &min_value, &max_value);
    }

    if(options[Slices])
    {
        float max_slices_value=0;
        for(int i=simulation.slices.size()-1; i>=0;i--) 
        {
            fftw_real *dataset_x_scalar, *dataset_y_scalar;
            switch(selected_scalar)
            {
                case DensityScalar: 
                {
                    dataset_x_scalar=simulation.slices[i].rho; dataset_y_scalar=simulation.slices[i].rho;
                } break;
                case VelocityScalar: 
                {
                    dataset_x_scalar=simulation.slices[i].vx; dataset_y_scalar=simulation.slices[i].vy;
                } break;
                case ForceScalar: 
                {
                    dataset_x_scalar=simulation.slices[i].fx; dataset_y_scalar=simulation.slices[i].fy;
                } break;
            }
            for(int j= 0; j<DIM*DIM;j++)
            {

                float f;
                if(selected_scalar==DensityScalar) {
                    f = dataset_x_scalar[j];
                } else {
                    f = atan2(dataset_y_scalar[j],dataset_x_scalar[j]) / M_PI + 1;
                }
                if(f>max_slices_value) max_slices_value = f;

            }
        }

        for(int i=simulation.slices.size()-1; i>=0;i--) 
        {

            if(options[DrawVecs])
            {
                fftw_real *dataset_x_scalar, *dataset_y_scalar, *dataset_x_vector, *dataset_y_vector;
                switch(selected_scalar)
                {
                    case DensityScalar: 
                    {
                        dataset_x_scalar=simulation.slices[i].rho; dataset_y_scalar=simulation.slices[i].rho;
                    } break;
                    case VelocityScalar: 
                    {
                        dataset_x_scalar=simulation.slices[i].vx; dataset_y_scalar=simulation.slices[i].vy;
                    } break;
                    case ForceScalar: 
                    {
                        dataset_x_scalar=simulation.slices[i].fx; dataset_y_scalar=simulation.slices[i].fy;
                    } break;
                }

                switch(selected_vector)
                {
                    case VelocityVector: {dataset_x_vector=simulation.slices[i].vx; dataset_y_vector=simulation.slices[i].vy;} break;
                    case ForceVector: {dataset_x_vector=simulation.slices[i].fx; dataset_y_vector=simulation.slices[i].fy;} break;
                }

                draw_vectors(dataset_x_scalar, dataset_y_scalar, dataset_x_vector, dataset_y_vector, wn, hn, min_value, max_value, i, max_slices_value);
            }
            if (selected_stream==StreamLine) draw_streamlines(simulation,winWidth, winHeight, wn, hn, min_value, max_value, i, max_slices_value); 


        }

        if(selected_stream==StreamSurface) draw_streamsurfaces(simulation,winWidth, winHeight, wn, hn, min_value, max_value);
        

    } else {
        if (options[DrawSmoke])
        {
            draw_smoke(simulation,wn,hn, min_value, max_value);
        }

        if (options[DrawVecs])
        {

            fftw_real *dataset_x_scalar, *dataset_y_scalar, *dataset_x_vector, *dataset_y_vector;
            switch(selected_scalar)
            {
                case DensityScalar: 
                {
                    dataset_x_scalar=simulation.rho; dataset_y_scalar=simulation.rho;
                } break;
                case VelocityScalar: 
                {
                    dataset_x_scalar=simulation.vx; dataset_y_scalar=simulation.vy;
                } break;
                case ForceScalar: 
                {
                    dataset_x_scalar=simulation.fx; dataset_y_scalar=simulation.fy;
                } break;
            }

            switch(selected_vector)
            {
                case VelocityVector: {dataset_x_vector=simulation.vx; dataset_y_vector=simulation.vy;} break;
                case ForceVector: {dataset_x_vector=simulation.fx; dataset_y_vector=simulation.fy;} break;
            }

            draw_vectors(dataset_x_scalar, dataset_y_scalar, dataset_x_vector, dataset_y_vector, wn, hn, min_value, max_value, 0, 1);
        }
        if (selected_stream==StreamLine) draw_streamlines(simulation,winWidth, winHeight, wn, hn, min_value, max_value, 0, 1);
    }
    glDisable(GL_DEPTH_TEST); // to draw legend on top
    glDisable (GL_LIGHTING);
    glPopMatrix(); // Pop in order to not let the transformations affect the legend
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
