/*-----------------------------------------------------------
 * An example program to draw a car
 *   Author: S.K. Ueng
 *   Date:  11/4/2001
 */
#include <stdio.h>
#include <math.h>

#include <GL/glut.h>
#include "Camera.hpp"
#include "MathUtility.hpp"
#include "Voxel.hpp"
#include "VoxelDrawer.hpp"
#include "TerrainGenerator.hpp"
#include "SpringGraph.hpp"
#include "SpringSuperGraph.hpp"
#include "DrawingUtility.hpp"
#include "GlobalDefinitions.hpp"
#include "Vehicle.hpp"

#define  PI   3.141592653

#define Step  0.5

/* indices of the box faces */
int    cube[6] = { 0, 1, 2, 3, 4, 5 };

/*-Declare GLU quadric objects, sphere, cylinder, and disk --*/
GLUquadricObj* sphere = NULL, * cylind = NULL, * disk;
Camera* camera;
//TerrainGenerator* terrain_generator;
//TerrainContainer terrains;
SpringSuperGraph* spring_graph;

std::vector<DrawObject* > draw_vec;

/*-Declare car position, orientation--*/
float  self_ang = -90.0, glob_ang = 0.0;
float  position[3] = { 8.0, 0.0, 0.0 };

/*-----Define window size----*/
int width = 512, height = 512;

/*----------------------------------------------------------
 * Procedure to initialize the working environment.
 */
void  myinit()
{
    glDrawBuffer(GL_BACK);
    glClearColor(0.0, 0.0, 0.0, 1.0);      /*set the background color BLACK */
    /*Clear the Depth & Color Buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    /*---Create quadratic objects---*/
    if (sphere == NULL) {
        sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
    }
    if (cylind == NULL) {
        cylind = gluNewQuadric();
        gluQuadricDrawStyle(cylind, GLU_FILL);
        gluQuadricNormals(cylind, GLU_SMOOTH);
    }
    if (disk == NULL) {
        disk = gluNewQuadric();
        gluQuadricDrawStyle(disk, GLU_FILL);
        gluQuadricNormals(disk, GLU_SMOOTH);
    }
}



/*---------------------------------------------------------
 * Procedure to draw the floor.
 */
void draw_floor()
{
    int  i, j;

    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++) {
            if ((i + j) % 2 == 0) glColor3f(1.0, 0.8, 0.8);
            else glColor3f(0.1, 0.1, 0.7);
            glBegin(GL_POLYGON);
            glVertex3f((i - 5.0) * 10.0, -2.5, (j - 5.0) * 10.0);
            glVertex3f((i - 5.0) * 10.0, -2.5, (j - 4.0) * 10.0);
            glVertex3f((i - 4.0) * 10.0, -2.5, (j - 4.0) * 10.0);
            glVertex3f((i - 4.0) * 10.0, -2.5, (j - 5.0) * 10.0);
            glEnd();
        }
}

/*-------------------------------------------------------
 * Procedure to draw three axes and the orign
 */
void draw_axes()
{

    /*----Draw a white sphere to represent the original-----*/
    glColor3f(0.9, 0.9, 0.9);

    gluSphere(sphere, 2.0,   /* radius=2.0 */
        12,            /* composing of 12 slices*/
        12);           /* composing of 8 stacks */

    /*----Draw three axes in colors, yellow, meginta, and cyan--*/
    /* Draw Z axis  */
    glColor3f(0.0, 0.95, 0.95);
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
        10.0,              /* height of the cylinder */
        12,               /* use 12-side polygon approximating circle*/
        3);               /* Divide it into 3 sections */

    /* Draw Y axis */
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);  /*Rotate about x by -90', z becomes y */
    glColor3f(0.95, 0.0, 0.95);
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
        10.0,             /* height of the cylinder */
        12,               /* use 12-side polygon approximating circle*/
        3);               /* Divide it into 3 sections */
    glPopMatrix();

    /* Draw X axis */
    glColor3f(0.95, 0.95, 0.0);
    glPushMatrix();
    glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about y  by 90', x becomes z */
    gluCylinder(cylind, 0.5, 0.5,   /* radius of top and bottom circle */
        10.0,             /* height of the cylinder */
        12,               /* use 12-side polygon approximating circle*/
        3);               /* Divide it into 3 sections */
    glPopMatrix();
    /*-- Restore the original modelview matrix --*/
    glPopMatrix();
}


/*-------------------------------------------------------
 * Display callback func. This func. draws three
 * cubes at proper places to simulate a solar system.
 */
void display()
{
    static float  ang_self = 0.0;  /*Define the angle of self-rotate */
    static float  angle = 0.0;

    /*Clear previous frame and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*----Define the current eye position and the eye-coordinate system---*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Camera trace
    //camera->pos = spring_graph->U->joints[0].pos + vec3(10, 10, 0);
    camera->LookAt();
    terrain_generator->draw();
    for (auto obj : draw_vec) obj->draw();
    //draw_floor();

    //draw_axes();

    /*-------Swap the back buffer to the front --------*/
    glutSwapBuffers();
    return;
}


/*--------------------------------------------------
 * Reshape callback function which defines the size
 * of the main window when a reshape event occurrs.
 */
void my_reshape(int w, int h)
{

    width = w;
    height = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect_ratio = w / (float)h;
    float fov = 10.0f;
    float zfar = 100.0f;
    gluPerspective(90, aspect_ratio,0.01, zfar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


/*--------------------------------------------------
 * Keyboard callback func. When a 'q' key is pressed,
 * the program is aborted.
 */
void my_quit(unsigned char key, int x, int y)
{
    if (key == 'Q' || key == 'q') exit(0);
    if (key == 'f') {
        position[0] += Step * cos(self_ang * PI / 180.0);
        position[2] -= Step * sin(self_ang * PI / 180.0);
    }
    else if (key == 'b') {
        position[0] -= Step * cos(self_ang * PI / 180.0);
        position[2] += Step * sin(self_ang * PI / 180.0);
    }
    else if (key == 'r') {
        self_ang += 10.0;
    }
    else if (key == 'c') self_ang -= 10.0;

    if (key == 'w') {
        camera->pos = uni(camera->front - camera->pos) * 1.0f + camera->pos;
        //spring_graph->joints[0].velocity += vec3(1, 0, 0);
    }
    if (key == 's') {
        camera->pos = uni(camera->front - camera->pos) * -1.0f + camera->pos;
    }
    if (key == 'e') {
        spring_graph->U->joints[0].force += vec3(1000, 0, 0);
    }
    /*if (key == 'e') {
        spring_graph->joints[0].force += vec3(1000, 0, 0);
    }*/
    //display();
}
void mainLoop(int val) {
    for (auto obj : draw_vec) obj->update();
    glutTimerFunc(0, mainLoop, -1);
}
int mx = 0, my = 0;
void passive_func(int x, int y) {
    camera->mouseMove(x - width/2,y - height/2);
    //mx = x;
    //my = y;
    glutWarpPointer(width / 2, height / 2);
}

/*---------------------------------------------------
 * Main procedure which defines the graphics environment,
 * the operation style, and the callback func's.
 */
void main(int argc, char** argv)
{
    /*-----Initialize the GLUT environment-------*/
    glutInit(&argc, argv);

    /*-----Depth buffer is used, be careful !!!----*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /*terrain_generator = new TerrainGenerator(
        [](ld x, ld y) {
            return 5 * (cos(x / 10.0) + sin(y / 10.0)); 
        }, 
        [](ld x, ld y) {
            return vec3();
        },
        [](ld x, ld y) {
            return Color{ 1,1,1 } * ((cos(x / 10.0) + sin(y / 10.0) + 2) / 4.0);
        },
        vec3{ -50,1,-50}, 100, 100,0.5);*/
    terrain_generator = new TerrainGenerator(
        [](ld x, ld y) {
            return 1;
        },
        [](ld x, ld y) {
            return vec3(0,1,0);
        },
        [](ld x, ld y) {
            int i = x / 10;
            int j = y / 10;
            return ((i + j) % 2 ? Color{ 1,1,1 } : Color{0,0,0});
        },
        vec3{ -50,0 ,-50 }, 100, 100, 0.5);
    terrain_generator->generate();
    //terrains.U.push_back(terrain_generator);

    glutInitWindowSize(width, height);
    glutCreateWindow("i.car");

    myinit();      /*---Initialize other state varibales----*/
    camera = new Camera(vec3(-20, 10, 20),vec3(0,0,0));
    auto g = std::vector<Joint>{ {vec3(0,0,0)},{vec3(10,0,0)},{vec3(0,10,0)},{vec3(0,0,10)},{vec3(10,10,0)},{vec3(10,10,10)},{vec3(10,0,10)}};
    SpringGraph * sg = new SpringGraph(g);
    SpringGraph * sg1 = new SpringGraph(g);
    spring_graph = new SpringSuperGraph();
    sg->addEdge(Spring{ 0,1 });
    sg->addEdge(Spring{ 0,2 });
    sg->addEdge(Spring{ 0,3 });
    sg->addEdge(Spring{ 1,2 });
    sg->addEdge(Spring{ 1,3 });
    sg->addEdge(Spring{ 2,3 });

    sg1->addEdge(Spring{ 0,1 });
    sg1->addEdge(Spring{ 0,2 });
    sg1->addEdge(Spring{ 0,3 });
    sg1->addEdge(Spring{ 1,2 });
    sg1->addEdge(Spring{ 1,3 });
    sg1->addEdge(Spring{ 2,3 });

    spring_graph->addGraph(sg);
    spring_graph->addGraph(sg1);
    spring_graph->build();
    spring_graph->addEdge(sg,sg1,Spring{0,1});
    draw_vec.push_back(spring_graph);
    auto car = new Vehicle();
    draw_vec.push_back(car);
    /*----Associate callback func's whith events------*/
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(my_reshape);
    glutKeyboardFunc(my_quit);
    glutPassiveMotionFunc(passive_func);
    glutTimerFunc(1, mainLoop, -1);
    glutMainLoop();
}

