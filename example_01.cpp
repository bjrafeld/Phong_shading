
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>


#define PI 3.14159265  // Should be used from mathlib

#define POINT_LIGHT 0
#define DIRECTION_LIGHT 1
inline float sqr(float x) { return x*x; }

using namespace std;

float max(float a, float b)
{
    if(a>b) {
        return a;
    } else {
        return b;
    }
}

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};


//****************************************************
// Vector3 Class
//****************************************************

class Vector3;

class Vector3 
{
public:
    Vector3();
    Vector3(float x, float y, float z);
    float x, y, z;
    Vector3 dot_product(Vector3 vect);
    Vector3 normalize();
    Vector3 scalar_mult(Vector3 value);
    Vector3 operator+(Vector3 vect);
    Vector3 operator-(Vector3 vect);
    static Vector3 max_zero(Vector3 vect);
    Vector3 power(Vector3 vect);
};

Vector3::Vector3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3::Vector3()
{
    this->x=0;
    this->y=0;
    this->z=0;
}

Vector3 Vector3::power(Vector3 value)
{
    Vector3 temp;
    temp.x = pow(this->x, value.x);
    temp.y = pow(this->y, value.y);
    temp.z = pow(this->z, value.z);
    return temp;
}

Vector3 Vector3::dot_product(Vector3 vect) 
{
    float dot_prod = (this->x * vect.x)+(this->y * vect.y)+(this->z * vect.z);
    Vector3 dot_vect = Vector3(dot_prod, dot_prod, dot_prod);
    return dot_vect;
}

Vector3 Vector3::normalize()
{
    Vector3 temp;
    float magnitude = sqrt(sqr(this->x)+sqr(this->y)+sqr(this->z));
    temp.x = this->x/magnitude;
    temp.y = this->y/magnitude;
    temp.z = this->z/magnitude;
    return temp;
}

Vector3 Vector3::scalar_mult(Vector3 value)
{
    Vector3 temp;
    temp.x = this->x*value.x;
    temp.y = this->y*value.y;
    temp.z = this->z*value.z;
    return temp;
}

Vector3 Vector3::operator+(Vector3 vect)
{
    Vector3 temp;
    temp.x = this->x + vect.x;
    temp.y = this->y + vect.y;
    temp.z = this->z + vect.z;
    return temp;
}

Vector3 Vector3::operator-(Vector3 vect)
{
    Vector3 temp;
    temp.x = this->x - vect.x;
    temp.y = this->y - vect.y;
    temp.z = this->z - vect.z;
    return temp;
}

Vector3 Vector3::max_zero(Vector3 vect)
{
    Vector3 result;
    result.x = max(0.0, vect.x);
    result.y = max(0.0, vect.y);
    result.z = max(0.0, vect.z);
    return result;
}

class Light;

class Light {
public:
    Light();
    Light(float x, float y, float z, float r, float g, float b, int light_type);
    Vector3 intensity;
    Vector3 coordinates;
};

Light::Light()
{
    Vector3 color;
    Vector3 vect;
    this->coordinates=vect;
    this->intensity=color;
}
Light::Light(float x, float y, float z, float r, float g, float b, int light_type)
{
    Vector3 color = Vector3(r, g, b);
    Vector3 vector;
    if(light_type==POINT_LIGHT)
        vector = Vector3(x, y, z);
    else if(light_type==DIRECTION_LIGHT)
        vector = Vector3(-x, -y, -z);
    this->coordinates=vector;
    this->intensity=color;
}


//****************************************************
// Global Variables
//****************************************************

Viewport	viewport;
Vector3 k_a;
Vector3 k_d;
Vector3 k_s;
float p;
bool toon = false;

//Vectors containing the 5 directional lights and the 5 point lights

std::vector<Light> d_lights;

std::vector<Light> p_lights;


//****************************************************
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

Vector3 reflectedVector(Light light, Vector3 normal)
{
    Vector3 two_scalar = Vector3(2.0,2.0,2.0);
    Vector3 negative_scalar = Vector3(-1.0,-1.0,-1.0);
    Vector3 negative_light = negative_scalar.scalar_mult(light.coordinates);
    Vector3 new_light = light.coordinates.normalize();
    negative_light = negative_light.normalize();
    
    Vector3 inner = normal.dot_product(new_light);
    inner = Vector3::max_zero(inner);
    Vector3 n_scalar = inner.scalar_mult(two_scalar);
    Vector3 right = normal.scalar_mult(n_scalar);
    return negative_light + right;
}

Vector3 ambientTerm(Vector3 intensity)
{
    Vector3 result = intensity.scalar_mult(k_a);
    return result;
}

Vector3 diffuseTerm(Light light, Vector3 normal)
{
    Vector3 new_light = light.coordinates.normalize();
    Vector3 new_normal = normal.normalize();
    
    Vector3 temp = light.intensity.scalar_mult(k_d);
    Vector3 temp3 = new_light.dot_product(new_normal);
    Vector3 temp2 = Vector3::max_zero(temp3);
    temp = temp.scalar_mult(temp2);
    /*cout << "light x is: "<< light.coordinates.x << " light y is: "<< light.coordinates.y << " light z is: "<< light.coordinates.z << endl;
    cout << "x is: "<< normal.x << " y is: "<< normal.y << " z is: "<< normal.z << endl;*/
    //if(temp.x==0&&temp.y==0&&temp.z==0)
        //cout << "r is: "<<temp.x << " g is: " << temp.y<<" z is: "<<temp.z<<endl;
    return temp;
}

Vector3 specularTerm(Light light, Vector3 normal)
{
    //Vector3 new_light = light.coordinates.normalize();
    Vector3 new_normal = normal.normalize();
    Vector3 reflected = reflectedVector(light, new_normal);
    
    reflected = reflected.normalize();
    Vector3 viewer = Vector3(0.0, 0.0, 1.0);
    Vector3 dot2 = reflected.dot_product(viewer);
    Vector3 pvect = Vector3(p,p,p);
    dot2 = dot2.power(pvect);
    Vector3 dot = Vector3::max_zero(dot2);
    Vector3 temp = light.intensity.scalar_mult(k_s);
    temp = temp.scalar_mult(dot);
    //if(temp.x==0 && temp.y==0 && temp.z !=0 && normal.x < 0 && normal.y<0)
        //cout << "specular r is: "<<temp.x << " specular g is: " << temp.y<<" specular z is: "<<temp.z<<endl;
    return temp;
}

void setPixel(float x, float y, float z, float a, float b) {
    Vector3 normal = Vector3(x, y, z);
    normal = normal.normalize();
    Vector3 total_color;
    Vector3 ambientTotal;
    for (int i=0; i<d_lights.size(); i++) {
        Light current_light;
        current_light = d_lights.at(i);
        ambientTotal = ambientTotal + current_light.intensity;
        total_color=total_color + diffuseTerm(current_light,normal)+specularTerm(current_light,normal);
    }
    for (int j=0; j<p_lights.size(); j++) {
        //point-light
        Light current_light;
        current_light = p_lights.at(j);
        Vector3 coords = Vector3(current_light.coordinates.x-normal.x, current_light.coordinates.y-normal.y, current_light.coordinates.z-normal.z);
        Light new_light = Light(coords.x, coords.y, coords.z, current_light.intensity.x, current_light.intensity.y, current_light.intensity.z, POINT_LIGHT);
        ambientTotal = ambientTotal + current_light.intensity;
        total_color= total_color+/*ambientTerm(new_light)+*/diffuseTerm(new_light,normal)+specularTerm(new_light,normal);
    }
    total_color = total_color + ambientTerm(ambientTotal);
    
  glColor3f(total_color.x, total_color.y, total_color.z);
  glVertex2f(a + 0.5, b + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}


//****************************************************
// Draw a filled circle.  
//****************************************************


void circle(float centerX, float centerY, float radius) {
  // Draw inner circle
  glBegin(GL_POINTS);

  // We could eliminate wasted work by only looping over the pixels
  // inside the sphere's radius.  But the example is more clear this
  // way.  In general drawing an object by loopig over the whole
  // screen is wasteful.

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));



  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {

      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);

      float dist = sqrt(sqr(x) + sqr(y));

      if (dist<=radius) {

        // This is the front-facing Z coordinate
        float z = sqrt(radius*radius-dist*dist);

        setPixel(x, y, z, i, j);

        // This is amusing, but it assumes negative color values are treated reasonably.
        // setPixel(i,j, x/radius, y/radius, z/radius );
      }

    }
  }

  glEnd();


  glEnd();
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"


  // Start drawing
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}

//****************************************************
// function that handles the keyboard
//****************************************************
void myKeyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 32:
            exit(0);
            break;
        default:
            break;
    }
}

// functions for handling command line stuff

void set_ka(float r, float g, float b)
{
    Vector3 temp = Vector3(r, g, b);
    k_a = temp;
}

void set_kd(float r, float g, float b)
{
    Vector3 temp = Vector3(r, g, b);
    k_d = temp;
}

void set_ks(float r, float g, float b)
{
    Vector3 temp = Vector3(r, g, b);
    k_s = temp;
}

void set_sp(float value)
{
    p = value;
}

void add_d_light(float x, float y, float z, float r, float g, float b)
{
    Light temp = Light(x, y, z, r, g, b, DIRECTION_LIGHT);
    if(d_lights.size() >= 5)
        return;
    else
        d_lights.push_back(temp);
        
}

void add_p_light(float x, float y, float z, float r, float g, float b)
{
    Light temp = Light(x, y, z, r, g, b, POINT_LIGHT);
    if(p_lights.size() >= 5)
        return;
    else
        p_lights.push_back(temp);
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
    
    for(int i=0; i < argc; i++)
    {
        string arg = argv[i];
        if(arg== "-ka"){
            set_ka(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]));
            i+=3;
        }
        else if(arg == "-kd") {
            set_kd(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]));
            i+=3;
        }
        else if(arg == "-ks") {
            set_ks(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]));
            i+=3;
        }
        else if (arg=="-sp") {
            set_sp(atof(argv[i+1]));
            i++;
        }
        else if(arg == "-pl") {
            add_p_light(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4]),atof(argv[i+5]), atof(argv[i+6]));
            i+=6;
        }
        else if(arg == "-dl") {
            add_d_light(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4]), atof(argv[i+5]), atof(argv[i+6]));
            i+=6;
        }
        else if(arg == "-tn") {
            toon = true;
        }
    }
    
    
  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();							// quick function to set up scene

  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized
    glutKeyboardFunc(myKeyboard);             // function to run when keyboard is used

  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}