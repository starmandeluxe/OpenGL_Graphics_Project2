/*
 *  Alex Kort
 *  Program 2, Flying Logos using Bezier and Catmull-Rom Curves
 *  2/05/2008
 *  CSC 474, Dr. Buckalew
 *
 *------------------------------------------------------------*/


#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>


// some function prototypes
void display(void);
void normalize(float[3]);
void normCrossProd(float[3], float[3], float[3]);

// initial viewer position
static GLdouble viewer[] = {0.0, 0.0, 5.0};
// initial viewer angle
static GLfloat theta[] = {0.0, 0.0, 0.0};

// animation variables
static int frame = 0;
static int startFrame = 0;
static int endFrame = 360;
static int increment = 1;

// animation transform variables
static GLdouble translate[3] = {-10.0, 0.0, 0.0};
static GLdouble cyltrans[3] = {-10.0, 0.0, 0.0};
static GLdouble rotate1[4] = {0, 0, 1, 0};
static GLdouble rotate2[4] = {0, 0, 1, 0};
static GLdouble rotate3[4] = {0, 0, 1, 0};

static double tristart1 = -2.0;
static double tristart2 = -4.0;
static double tristart3 = 3.0;

static GLUquadric* quad;

static GLdouble tri1trans[3] = {tristart1, tristart2, tristart3};
static GLdouble tri2trans[3] = {tristart1, tristart2, tristart3};
static GLdouble tri3trans[3] = {tristart1, tristart2, tristart3};

static GLdouble ctrlpts[9][4];

static GLdouble ptsx[10] = {2,2,0,3,-1,5,1,-2,6,5};
static GLdouble ptsy[10] = {3,4,2,5,-1,2,3,1,7,4};
static GLdouble ptsz[10] = {1,-2,0,3,0,-2,-3,-2,0,3};

//all control points
GLdouble p0[] = {-2,-4,3};
GLdouble p1[] = {6,6,-4};
GLdouble p2[] = {2,9,-7};
GLdouble p3[] = {-2,3,-4};
GLdouble p4[] = {-7,4,-1};
GLdouble p5[] = {0,-4,2};
GLdouble p6[] = {-6,-10,4};
GLdouble p7[] = {-1,-5,-2};
GLdouble p8[] = {6,-7,-3};
GLdouble p9[] = {-2,-4,3};

GLdouble b1[] = {-4,-8,8};
GLdouble b2[] = {-2,6,-3};
GLdouble b3[] = {-5,16,-7};
GLdouble b4[] = {2,21,-4};

GLdouble dist[1600][5];
GLdouble vtable[360];

double v0 = 1.0/0.75;
double t1 = 0.25;
double t2 = 0.75;
double dt = 0;

//counter for ctrl pts
static int count = 0;
//how much to increment counter by (1 or -1)
static int inc = 1;

//define control points
static double n1x = 0;
static double n2x = 0;
static double n3x = 0;
static double n4x = 0;

static double n1y = 0;
static double n2y = 0;
static double n3y = 0;
static double n4y = 0;

static double n1z = 0;
static double n2z = 0;
static double n3z = 0;
static double n4z = 0;

int d = 200;
double ltotal = 0;
double second[100];
double distance[1600][5];
double points[200][3];
double prev = -10;
double len = 0;
double sarray[360];

//calculates the distance table
/*
void calculate()
{
   double t;
   for (int i = 0; i <= d; i++)
   {
	   points[i][0] = (double)i/d;
	   t = points[i][0];

	   printf("%d: %.2f", i, t);

	   points[i][1] = (1-t)*(1-t)*-10.0 + 2.0*t*(1.0-t)*20 + t*t*10.0;

	   printf(", %6.2f", points[i][1]);

		if ((points[i][1] - prev) < 0)
			len += -1*(points[i][1] - prev);
		else
			len += (points[i][1] - prev);

	   prev = points[i][1];
	   points[i][2] = len;
	   printf(", %6.2f\n", points[i][2]);
   }
   printf("\n\n Second \n\n");
   for (int j = 0; j <= 100; j++)
   {
	second[j] = (double)25/100*j;
	t = second[j];
	printf("%d: %.2f\n", j, t);

	
   }
   double closest = 100;
   for (int n = 1; n <= 100; n++)
   {
	   double ind = 0;
		double s = 0;
      for (int k = 1; k <= d; k++)
	  {

		  double dif = points[k][2] - second[n];

		  if (dif < 0)
			  dif = -1*dif;


		  if (dif < closest)
		  {
				closest = dif;

				ind = points[k][2];
				s = points[k][0];
			
		  }
	  }
	  distance[n][0] = s;

	  distance[n][2] = ind;

	  printf("third[%d] = %6.2f, %6.2f\n", n, distance[n][0], distance[n][2]);
	  closest = 100.0;
   }

}
*/


//---------------------------------------------------------
//   Set up the view

void setUpView() {
   // this code initializes the viewing transform
   glLoadIdentity();

   // moves viewer along coordinate axes
   gluLookAt(viewer[0], viewer[1], viewer[2], 
             0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   // move the view back some relative to viewer[] position
   glTranslatef(0.0f,0.0f, -8.0f);

   // rotates view
   glRotatef(theta[0], 1.0, 0.0, 0.0);
   glRotatef(theta[1], 0.0, 1.0, 0.0);
   glRotatef(theta[2], 0.0, 0.0, 1.0);

   return;
}

//----------------------------------------------------------
//  Set up the light

void setUpLight() {
   // set up the light sources for the scene
   // a directional light source from over the right shoulder

	quad=gluNewQuadric();
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);


   glEnable(GL_LIGHTING);


   GLfloat light0_diffuse[] = {1.0, .84, 0.0, 1.0};
   GLfloat light0_position[] = {-1.0, 0.0, 2.0, 0.0};
   GLfloat light1_diffuse[] = {1.0, 1.0, 0.0, 1.0};
   GLfloat light1_position[] = {1.0, 0.0, 2.0, 0.0};
   GLfloat light2_diffuse[] = {1.0, 1.0, 0.0, 1.0};
   GLfloat light2_position[] = {0.0, 2.0, -2.0, 0.0};
   GLfloat ambient[] = {1.0f, 1.0f, 0.0f, 1.0f};

   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_LIGHT2);
   glEnable(GL_LIGHT3);


	
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
   glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
   glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
   glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);


   return;
}

//--------------------------------------------------------
//  Set up the objects

void drawSphere() {
   // draw an object

   // save the transformation state
   glPushMatrix();

   // set the material

   GLfloat material_ambient[] = {0.0f, 0.0f, 0.0f};
   GLfloat material_diffuse[] = {1, 0.54902, 0};
   GLfloat material_specular[] = {0.20f, 0.140f, 0.00f};
   GLfloat material_shininess = 25.0f;

   glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);

   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // this translation will be used to animate the sphere
   glTranslatef(translate[0], translate[1], translate[2]);	 
   glRotatef(90, 1.0, 0.0, 0.0);  

   // draw the sphere - the parameters are radius, number of
   //   radial slices (longitude lines) and number of vertical
   //   slices (latitude lines)
   glutSolidSphere(3.0, 30, 30);

   // recover the transform state
   glPopMatrix();

   return;
}

void drawCylinder()
{
	// draw an object

   // save the transformation state
   glPushMatrix();

   // set the material

   GLfloat material_ambient[] = {0.0f, 0.0f, 0.0f};
   GLfloat material_diffuse[] = {1, 0.3, 0};
   GLfloat material_specular[] = {0.20f, 0.140f, 0.00f};
   GLfloat material_shininess = 25.0f;

   glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);

   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
	glTranslatef(translate[0], translate[1], translate[2]);
	glRotatef(150, 0.0, .7, 1.5); 

	//2nd param is radius at bottom, 3rd is radius at top
	//4th is height (length), 5th is subdivisions around z,
	//6th is subdivisions along z
	gluCylinder(quad,.3f,.3f,5.0f,32,32);


	// recover the transform state
   glPopMatrix();

   return;
}

void drawTri1() {

   // save the transformation state
   glPushMatrix();

   // set the material

   GLfloat material_ambient[] = {0.25f, 0.25f, 0.25f};
   GLfloat material_diffuse[] = {0.90f, 0.90f, 0.90f};
   GLfloat material_specular[] = {0.90f, 0.90f, 0.90f};
   GLfloat material_shininess = 25.0f;

   glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);
 

   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // Adjust the translate and scale to make the monitor the right
   //    shape and in the right place
   glTranslatef(tri1trans[0], tri1trans[1], tri1trans[2]);
	glRotatef(rotate1[0], rotate1[1], rotate1[2], rotate1[3]);

	//TRI 1
   glBegin(GL_TRIANGLES);

	//front face
	glColor3f(1,1,0);			
	glVertex3f(-2, 0, 0);
	glColor3f(1,1,0);
	glVertex3f(2, 0, 0);	
	glColor3f(1,1,0);
	glVertex3f(0, 3, 0);

	//back face
	glColor3f(1,1,0);			
	glVertex3f(-2, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(2, 0, -1);	
	glColor3f(1,1,0);
	glVertex3f(0, 3, -1);

   glEnd();

	glBegin(GL_QUADS);

	//left face
	glColor3f(1,1,0);
	glVertex3f(-2, 0, 0);
	glColor3f(1,1,0);
	glVertex3f(-2, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(0, 3, 0);
	glColor3f(1,1,0);
	glVertex3f(0, 3, -1);

	//right face
	glColor3f(1,1,0);
	glVertex3f(2, 0, 0);
	glColor3f(1,1,0);
	glVertex3f(2, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(0, 3, -1);
	glColor3f(1,1,0);
	glVertex3f(0, 3, 0);

	//bottom face
	glColor3f(1,1,0);
	glVertex3f(-2, 0, 0);
	glColor3f(1,1,0);
	glVertex3f(-2, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(2, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(2, 0, 0);	

	glEnd();

   // recover the transform state
   glPopMatrix();

   return;
}

void drawTri2() {

   // save the transformation state
   glPushMatrix();

   // set the material
   //GLfloat diffuseColor[] = {0.8, 1.0, 0.8, 1.0};
   //glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);

   GLfloat material_ambient[] = {0.25f, 0.25f, 0.25f};
   GLfloat material_diffuse[] = {0.90f, 0.90f, 0.90f};
   GLfloat material_specular[] = {0.90f, 0.90f, 0.90f};
   GLfloat material_shininess = 25.0f;

   glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);
 

   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // Adjust the translate and scale to make the monitor the right
   //    shape and in the right place
   glTranslatef(tri2trans[0], tri2trans[1], tri2trans[2]);
	glRotatef(rotate2[0], rotate2[1], rotate2[2], rotate2[3]);

	//TRI 2
   glBegin(GL_TRIANGLES);

	//front face
	glColor3f(1,1,0);			
	glVertex3f(2, 0, 0);
	glColor3f(1,1,0);
	glVertex3f(6, 0, 0);	
	glColor3f(1,1,0);
	glVertex3f(4, 3, 0);

	//back face
	glColor3f(1,1,0);			
	glVertex3f(2, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(6, 0, -1);	
	glColor3f(1,1,0);
	glVertex3f(4, 3, -1);

   glEnd();

	glBegin(GL_QUADS);

	//left face
	glColor3f(1,1,0);
	glVertex3f(2, 0, 0);
	glColor3f(1,1,0);
	glVertex3f(2, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(4, 3, 0);
	glColor3f(1,1,0);
	glVertex3f(4, 3, -1);

	//right face
	glColor3f(1,1,0);
	glVertex3f(6, 0, 0);
	glColor3f(1,1,0);
	glVertex3f(6, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(4, 3, -1);
	glColor3f(1,1,0);
	glVertex3f(4, 3, 0);

	//bottom face
	glColor3f(1,1,0);
	glVertex3f(2, 0, 0);
	glColor3f(1,1,0);
	glVertex3f(2, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(6, 0, -1);
	glColor3f(1,1,0);
	glVertex3f(6, 0, 0);	

	glEnd();
      
   // recover the transform state
   glPopMatrix();

   return;
}

void drawTri3() {

   // save the transformation state
   glPushMatrix();

   // set the material


   GLfloat material_ambient[] = {0.25f, 0.25f, 0.25f};
   GLfloat material_diffuse[] = {0.90f, 0.90f, 0.90f};
   GLfloat material_specular[] = {0.90f, 0.90f, 0.90f};
   GLfloat material_shininess = 25.0f;

   glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);
 

   // locate it in the scene
   glMatrixMode(GL_MODELVIEW);
   // Adjust the translate and scale to make the monitor the right
   //    shape and in the right place
	glTranslatef(tri3trans[0], tri3trans[1], tri3trans[2]);
	glRotatef(rotate3[0], rotate3[1], rotate3[2], rotate3[3]);

	//TRI 3
   glBegin(GL_TRIANGLES);

	//front face
	glColor3f(1,1,0);			
	glVertex3f(0, 3, 0);
	glColor3f(1,1,0);
	glVertex3f(4, 3, 0);	
	glColor3f(1,1,0);
	glVertex3f(2, 6, 0);

	//back face
	glColor3f(1,1,0);			
	glVertex3f(0, 3, -1);
	glColor3f(1,1,0);
	glVertex3f(4, 3, -1);	
	glColor3f(1,1,0);
	glVertex3f(2, 6, -1);

   glEnd();

	glBegin(GL_QUADS);

	//left face
	
	glColor3f(1,1,0);
	glVertex3f(0, 3, -1);
	glColor3f(1,1,0);
	glVertex3f(0, 3, 0);
	glColor3f(1,1,0);
	glVertex3f(2, 6, 0);
	glColor3f(1,1,0);
	glVertex3f(2, 6, -1);

	//right face
	glColor3f(1,1,0);
	glVertex3f(4, 3, 0);
	glColor3f(1,1,0);
	glVertex3f(4, 3, -1);
	glColor3f(1,1,0);
	glVertex3f(2, 6, -1);
	glColor3f(1,1,0);
	glVertex3f(2, 6, 0);

	//bottom face
	glColor3f(1,1,0);
	glVertex3f(0, 3, 0);
	glColor3f(1,1,0);
	glVertex3f(0, 3, -1);
	glColor3f(1,1,0);
	glVertex3f(4, 3, -1);
	glColor3f(1,1,0);
	glVertex3f(4, 3, 0);	

	glEnd();


      
   // recover the transform state
   glPopMatrix();

   return;
}

//draw a control point
void drawCtrlPt(GLdouble pt[])
{

		glPushMatrix();
		GLfloat material_ambient[] = {0.0f, 0.0f, 0.8f};
		GLfloat material_diffuse[] = {0.6f, 0.8f, 0.90f};
		GLfloat material_specular[] = {0.8f, 0.8f, 0.90f};
		GLfloat material_shininess = 25.0f;

		glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
		glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);	
		glTranslatef(pt[0],pt[1],pt[2]);
		glutSolidSphere(.3,10,10);
		glPopMatrix();
}

void drawBezPt(GLdouble pt[])
{

		glPushMatrix();
		GLfloat material_ambient[] = {0.0f, 0.0f, 0.8f};
		GLfloat material_diffuse[] = {1, 0.0f, 0.0f};
		GLfloat material_specular[] = {1, 0.0f, 0.0f};
		GLfloat material_shininess = 25.0f;

		glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
		glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);	
		glTranslatef(pt[0],pt[1],pt[2]);
		glutSolidSphere(.3,10,10);
		glPopMatrix();
}

//-----------------------------------------------------------
//  Callback functions

void reshapeCallback(int w, int h) {
   // from Angel, p.562

   glViewport(0,0,w,h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w < h) {
      glFrustum(-2.0, 2.0, -2.0*(GLfloat) h / (GLfloat) w,
                2.0*(GLfloat) h / (GLfloat) w, 2.0, 20.0);
   }
   else {
      glFrustum(-2.0, 2.0, -2.0*(GLfloat) w / (GLfloat) h,
                2.0*(GLfloat) w / (GLfloat) h, 2.0, 20.0);
   }

   glMatrixMode(GL_MODELVIEW);
}

void getDtable()
{
	double t = 0.0;
	double inc = .01;
	double tempx = 0.0;
	double tempy = 0.0;
	double tempz = 0.0;
	dist[0][1] = p0[0] * (-0.5f*t*t*t + t*t - 0.5f*t) +
				p0[0] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
				p1[0] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
				p2[0] * (0.5f*t*t*t - 0.5f*t*t);
	dist[0][2] = p0[1] * (-0.5f*t*t*t + t*t - 0.5f*t) +
				p0[1] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
				p1[1] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
				p2[1] * (0.5f*t*t*t - 0.5f*t*t);
	dist[0][3] = p0[2] * (-0.5f*t*t*t + t*t - 0.5f*t) +
				p0[2] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
				p1[2] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
				p2[2] * (0.5f*t*t*t - 0.5f*t*t);
	tempx = dist[0][1];
	tempy = dist[0][2];
	tempz = dist[0][3];
	distance[0][0] = 0;
	distance[0][1] =  0;

	for (int i = 1; i <= 200; i++)
	{
		t = (double)i / 1600;
		//printf("t = %f\n", t);
		dist[i][0] = t;
		dist[i][1] = p0[0] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p0[0] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p1[0] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p2[0] * (0.5f*t*t*t - 0.5f*t*t);
		//printf("dist[%d] = %f\n", i, dist[i][1]);
		dist[i][2] = p0[1] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p0[1] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p1[1] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p2[1] * (0.5f*t*t*t - 0.5f*t*t);
		//printf("dist[%d] = %f\n", i, dist[i][2]);
		dist[i][3] = p0[2] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p0[2] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p1[2] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p2[2] * (0.5f*t*t*t - 0.5f*t*t);
		//printf("dist[%d] = %f\n", i, dist[i][3]);

		distance[i][0] = t;
		distance[i][1] = distance[i-1][1] + sqrt(pow((dist[i][1] - tempx), 2) 
			+ pow((dist[i][2] - tempy), 2) 
			+ pow((dist[i][3] - tempz), 2));
		//printf("distance[%d] = %f\n", i, distance[i][1]);

		tempx = dist[i][1];
		tempy = dist[i][2];
		tempz = dist[i][3];

	}
	for (i = 200; i <= 400; i++)
	{
		t = (double)i / 1600;
		dist[i][0] = t;
		dist[i][1] = p0[0] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p1[0] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p2[0] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p3[0] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][2] = p0[1] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p1[1] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p2[1] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p3[1] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][3] = p0[2] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p1[2] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p2[2] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p3[2] * (0.5f*t*t*t - 0.5f*t*t);
		distance[i][1] = distance[i-1][1] + sqrt(pow((dist[i][1] - tempx), 2) 
			+ pow((dist[i][2] - tempy), 2) 
			+ pow((dist[i][3] - tempz), 2));
		//printf("distance[%d] = %f\n", i, distance[i][1]);

		tempx = dist[i][1];
		tempy = dist[i][2];
		tempz = dist[i][3];
	}
	for (i = 400; i <= 600; i++)
	{
		t = (double)i / 1600;
		dist[i][0] = t;
		dist[i][1] = p1[0] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p2[0] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p3[0] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p4[0] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][2] = p1[1] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p2[1] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p3[1] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p4[1] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][3] = p1[2] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p2[2] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p3[2] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p4[2] * (0.5f*t*t*t - 0.5f*t*t);
		distance[i][1] = distance[i-1][1] + sqrt(pow((dist[i][1] - tempx), 2) 
			+ pow((dist[i][2] - tempy), 2) 
			+ pow((dist[i][3] - tempz), 2));
		//printf("distance[%d] = %f\n", i, distance[i][1]);

		tempx = dist[i][1];
		tempy = dist[i][2];
		tempz = dist[i][3];
	}
	for (i = 600; i <= 800; i++)
	{
		t = (double)i / 1600;
		dist[i][0] = t;
		dist[i][1] = p2[0] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p3[0] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p4[0] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p5[0] * (0.5f*t*t*t - 0.5f*t*t);
		
		dist[i][2] = p2[1] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p3[1] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p4[1] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p5[1] * (0.5f*t*t*t - 0.5f*t*t);
		
		dist[i][3] = p2[2] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p3[2] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p4[2] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p5[2] * (0.5f*t*t*t - 0.5f*t*t);
		
		distance[i][1] = distance[i-1][1] + sqrt(pow((dist[i][1] - tempx), 2) 
			+ pow((dist[i][2] - tempy), 2) 
			+ pow((dist[i][3] - tempz), 2));
		//printf("distance[%d] = %f\n", i, distance[i][1]);

		tempx = dist[i][1];
		tempy = dist[i][2];
		tempz = dist[i][3];
	}
	for (i = 800; i <= 1000; i++)
	{
		t = (double)i / 1600;
		dist[i][0] = t;
		dist[i][1] = p3[0] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p4[0] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p5[0] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p6[0] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][2] = p3[1] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p4[1] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p5[1] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p6[1] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][3] = p3[2] * (-0.5f*t*t*t + t*t - 0.5f*t) +
					p4[2] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
					p5[2] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
					p6[2] * (0.5f*t*t*t - 0.5f*t*t);
		distance[i][1] = distance[i-1][1] + sqrt(pow((dist[i][1] - tempx), 2) 
			+ pow((dist[i][2] - tempy), 2) 
			+ pow((dist[i][3] - tempz), 2));
		//printf("distance[%d] = %f\n", i, distance[i][1]);

		tempx = dist[i][1];
		tempy = dist[i][2];
		tempz = dist[i][3];
	}
	for (i = 1000; i <= 1200; i++)
	{
		t = (double)i / 1600;
		dist[i][0] = t;
		dist[i][1] = p4[0] * (-0.5f*t*t*t + t*t - 0.5f*t) +
							p5[0] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							p6[0] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							p7[0] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][2] = p4[1] * (-0.5f*t*t*t + t*t - 0.5f*t) +
							p5[1] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							p6[1] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							p7[1] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][3] = p4[2] * (-0.5f*t*t*t + t*t - 0.5f*t) +
							p5[2] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							p6[2] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							p7[2] * (0.5f*t*t*t - 0.5f*t*t);
		distance[i][1] = distance[i-1][1] + sqrt(pow((dist[i][1] - tempx), 2) 
			+ pow((dist[i][2] - tempy), 2) 
			+ pow((dist[i][3] - tempz), 2));
		//printf("distance[%d] = %f\n", i, distance[i][1]);

		tempx = dist[i][1];
		tempy = dist[i][2];
		tempz = dist[i][3];
	}
	for (i = 1200; i <= 1400; i++)
	{
		t = (double)i / 1600;
		dist[i][0] = t;
		dist[i][1] = p5[0] * (-0.5f*t*t*t + t*t - 0.5f*t) +
							p6[0] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							p7[0] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							p8[0] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][2] = p5[1] * (-0.5f*t*t*t + t*t - 0.5f*t) +
							p6[1] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							p7[1] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							p8[1] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][3] = p5[2] * (-0.5f*t*t*t + t*t - 0.5f*t) +
							p6[2] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							p7[2] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							p8[2] * (0.5f*t*t*t - 0.5f*t*t);
		distance[i][1] = distance[i-1][1] + sqrt(pow((dist[i][1] - tempx), 2) 
			+ pow((dist[i][2] - tempy), 2) 
			+ pow((dist[i][3] - tempz), 2));
		//printf("distance[%d] = %f\n", i, distance[i][1]);

		tempx = dist[i][1];
		tempy = dist[i][2];
		tempz = dist[i][3];
	}
	for (i = 1400; i <= 1600; i++)
	{
		t = (double)i / 1600;
		dist[i][0] = t;

		dist[i][1] = p6[0] * (-0.5f*t*t*t + t*t - 0.5f*t) +
							p7[0] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							p8[0] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							p8[0] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][2] = p6[1] * (-0.5f*t*t*t + t*t - 0.5f*t) +
							p7[1] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							p8[1] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							p8[1] * (0.5f*t*t*t - 0.5f*t*t);
		dist[i][3] = p6[2] * (-0.5f*t*t*t + t*t - 0.5f*t) +
							p7[2] * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							p8[2] * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							p8[2] * (0.5f*t*t*t - 0.5f*t*t);
		distance[i][1] = distance[i-1][1] + sqrt(pow((dist[i][1] - tempx), 2) 
			+ pow((dist[i][2] - tempy), 2) 
			+ pow((dist[i][3] - tempz), 2));

		//printf("dist[%d] = %f\n", i, dist[i][0]);
		printf("distance[%d] = %f\n", i, distance[i][1]);

		tempx = dist[i][1];
		tempy = dist[i][2];
		tempz = dist[i][3];

	}
	ltotal = distance[1600][1];
	//printf("ltotal = %f\n", ltotal);
}

void getVtable()
{
	for (int i = 0; i <= 120; i++)
	{
		vtable[i] = 5 * ((i*i) / (2*120));
	}
	for (i = 121; i <= 240; i++)
	{
		vtable[i] = 5 * ((i*i) / (2*120));
	}

}

void initSarray()
{
	for (int i = 0; i < 360; i++)
	{
		double t = (double) i / 360;

		if (t <= t1) 
		{
			dt = ltotal * (v0 * t*t/(2.0 * t1));
		}
		else if (t <= t2) 
		{
			dt = ltotal * ((v0 * t1 / 2.0) + v0 * (t - t1));
		}
		else 
		{
			dt = ltotal * ((v0 * t1 / 2.0) + v0 * (t2 - t1) + v0 * (t - t2) * (1.0 - (t - t2)/(2.0 * (1.0 - t2))));
		}

		for (int j = 0; j < 1600; j++)
		{
			//printf("distance[%d][1] = %f\n", j, distance[j][1]);
			//printf("dt = %f\n", dt);
			if ((dt  - distance[j][1]) <= .02 || (dt  - distance[j][1]) >= -.02)
			{
				
				double u = (dt - distance[j][1]) / (distance[j+1][1] - distance[j][1]);
				printf("u = %f\n", u);
				sarray[i] = (double)(i / 360) + u*((double)(i + 1)/360 - (double)i / 360);
				printf("sarray[%d] = %f\n", i, sarray[i]);
				break;
			}
		}
	}
}


void timeStep(int step) {
   // animation code goes here
   
   // This function is called for each frame of animation
   double t = (double) (frame - startFrame) / (endFrame - startFrame);
   //printf("t = %f\n", t); 

	double phi = 0 + t * (3.14159 - 0);
   
   //cubic bezier interpolation
   //translate[0] = (1-t*t)*10 + 2*t*(1-t)*-10 + t*t*0; 
   translate[0] = pow((1-t), 3)*b1[0] + 3*t*(1-t)*(1-t)*b2[0] + 3*t*t*(1-t)*b3[0] + t*t*t*b4[0]; 
   translate[1] = pow((1-t), 3)*b1[1] + 3*t*(1-t)*(1-t)*b2[1] + 3*t*t*(1-t)*b3[1] + t*t*t*b4[1];
   translate[2] = pow((1-t), 3)*b1[2] + 3*t*(1-t)*(1-t)*b2[2] + 3*t*t*(1-t)*b3[2] + t*t*t*b4[2];

	


/*
		tri1trans[0] = n1x * (-0.5f*t*t*t + t*t - 0.5f*t) +
							n2x * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							n3x * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							n4x * (0.5f*t*t*t - 0.5f*t*t);

		tri1trans[1] = n1y * (-0.5f*t*t*t + t*t - 0.5f*t) +
							n2y * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							n3y * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							n4y * (0.5f*t*t*t - 0.5f*t*t);

		tri1trans[2] = n1z * (-0.5f*t*t*t + t*t - 0.5f*t) +
							n2z * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							n3z * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							n4z * (0.5f*t*t*t - 0.5f*t*t);

		tri2trans[0] = n1x * (-0.5f*t*t*t + t*t - 0.5f*t) +
							n2x * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							n3x * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							n4x * (0.5f*t*t*t - 0.5f*t*t);

		tri2trans[1] = n1y * (-0.5f*t*t*t + t*t - 0.5f*t) +
							n2y * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							n3y * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							n4y * (0.5f*t*t*t - 0.5f*t*t);

		tri2trans[2] = n1z * (-0.5f*t*t*t + t*t - 0.5f*t) +
							n2z * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							n3z * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							n4z * (0.5f*t*t*t - 0.5f*t*t);

		tri3trans[0] = n1x * (-0.5f*t*t*t + t*t - 0.5f*t) +
							n2x * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							n3x * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							n4x * (0.5f*t*t*t - 0.5f*t*t);

		tri3trans[1] = n1y * (-0.5f*t*t*t + t*t - 0.5f*t) +
							n2y * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							n3y * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							n4y * (0.5f*t*t*t - 0.5f*t*t);

		tri3trans[2] = n1z * (-0.5f*t*t*t + t*t - 0.5f*t) +
							n2z * (1.5f*t*t*t + -2.5f*t*t + 1.0f) +
							n3z * (-1.5f*t*t*t + 2.0f*t*t + 0.5f*t) +
							n4z * (0.5f*t*t*t - 0.5f*t*t);
	}
		*/


	if (frame == 360) increment = 0;
   else if (frame == startFrame) increment = 1;

   if (frame == endFrame) 
	{
		count += inc;
		//printf("count = %d\n", count);
	   startFrame=endFrame;	
	   endFrame+=40;
   }
    
   frame = frame + increment;

   display();

   glutTimerFunc(40,timeStep, 0);

}

//---------------------------------------------------------
//  Main routines

void display (void) {
   // this code executes whenever the window is redrawn (when opened,
   //   moved, resized, etc.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // set the viewing transform
   setUpView();

   // set up light source
   setUpLight();

   // start drawing objects
   drawSphere();

	drawCylinder();

	drawTri1();
	drawTri2();
	drawTri3();

	drawCtrlPt(p0);
	drawCtrlPt(p1);
	drawCtrlPt(p2);
	drawCtrlPt(p3);
	drawCtrlPt(p4);
	drawCtrlPt(p5);
	drawCtrlPt(p6);
	drawCtrlPt(p7);
	drawCtrlPt(p8);
	drawCtrlPt(p9);

	drawBezPt(b1);
	drawBezPt(b2);
	drawBezPt(b3);
	drawBezPt(b4);

   glutSwapBuffers();
}

// create a double buffered 500x500 pixel color window
int main(int argc, char* argv[]) 
{
	//calculate();
	getDtable();
	initSarray();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Bezier Interpolation: Lab 3");
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);
   glutReshapeFunc(reshapeCallback);
	glutTimerFunc(50,timeStep, 0);  // 50 millisecond callback
	glutMainLoop();
	return 0;
}

//---------------------------------------------------------
//  Utility functions

void normalize(float v[3]) {
   // normalize v[] and return the result in v[]
   // from OpenGL Programming Guide, p. 58
   GLfloat d = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
   if (d == 0.0) {
      printf("zero length vector");
      return;
   }
   v[0] = v[0]/d; v[1] = v[1]/d; v[2] = v[2]/d;
}

void normCrossProd(float v1[3], float v2[3], float out[3]) {
   // cross v1[] and v2[] and return the result in out[]
   // from OpenGL Programming Guide, p. 58
   out[0] = v1[1]*v2[2] - v1[2]*v2[1];
   out[1] = v1[2]*v2[0] - v1[0]*v2[2];
   out[2] = v1[0]*v2[1] - v1[1]*v2[0];
   normalize(out);
}
