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
#include "Portal.hpp"

Camera* camera;
SpringSuperGraph* spring_graph;

std::vector<GameObject* > draw_vec;
VoxelDrawer* vd;


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
}


void drawAxis() {
    std::vector<Color> colors = { {1,0,0},{0,1,0},{0,0,1} };
    std::vector<vec3> axiss;

    for (int i = 0; i < 3; i++) {
        glColor3f(TC(colors[i % 3]));
        auto u = vec3(TC(colors[i])) * 10;
        axiss.push_back(u);
    }
    for (int i = 0; i < axiss.size(); ++i) {
        glColor3f(TC(colors[i % 3]));
        glPushMatrix();
        //glTranslatef(/*TP(pos)*/);
        alignZTo(axiss[i]);
        glScalef(.2, .2, abs(axiss[i]));
        draw_unicylind();
        glPopMatrix();
    }
}


Vehicle* car;
bool camera_following = false;
/*-------------------------------------------------------
 * Display callback func. This func. draws three
 * cubes at proper places to simulate a solar system.
 */
void display()
{
    static float ang_self = 0.0;  /*Define the angle of self-rotate */
    static float angle    = 0.0;

    static Voxel vox;
    static Portal A;
    static Portal B;
	A.pos = vec3(20, 40 ,0);
	B.pos = vec3(0, 15 , -20);
    A.scale = vec3(10, 10, 1);
    B.scale = vec3(10, 10, 1);
    //B.rot.x = pi / 4;
    //A.rot.x = pi / 4;
    vox.scale = vec3(4,4,4);
    vox.rot.x += 0.01;
    vox.rot.y += 0.01;

    vec3 v = camera->pos - A.pos;

    if (uni(A.forward()) * v < 2.0f && abs(v) < 10.0f) {
        camera->pos = (B.localToWorld() * A.worldToLocal() * vec4(camera->pos, 1)).toVec3();
    }

    /*Clear previous frame and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*----Define the current eye position and the eye-coordinate system---*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Camera trace
    if (camera_following) {
        camera->front += vec3(0, 5, 0);
        camera->pos = car->pos + vec3(0, 5, 0);
    }

    auto drawScence = [&]() {
        terrain_generator->draw();
        vd->drawSingleVoxel(vox);
        for (auto obj : draw_vec) obj->draw();
        drawAxis();
	};
	A.camDraw(*camera,B);

	drawScence();

    glLoadMatrixf(camera->Matrix().transposed().mt);
	A.camEndDraw();

    drawScence();
    //B.draw();
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
    float fov = 110.0f;
    float zfar = 10000.0f;
	camera->resize(w, h, 0.01f, zfar, fov);
	//glLoadMatrixf(camera->proj.transposed().mt);
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

    if (key == 'w') {
        camera->pos = uni(camera->front) * 1.0f + camera->pos;
    }
    if (key == 's') {
        camera->pos = uni(camera->front) * -1.0f + camera->pos;
    }
    if (key == 'f') {
        camera_following = !camera_following;
    }
    if (key == 't') {
        auto ori = uni(camera->front);
        ori.y = 0;
        car->applyForce( ori * 1 );
    }

    if (key == 'g') {
        auto ori = uni(camera->front);
        ori.y = 0;
        car->applyForce(ori * -1);
    }

    if (key == 'y') {
        car->totTorque += vec3(0, -100, 0);
    }

    if (key == 'h') {
        car->totTorque += vec3(0, 100, 0);
    }
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
    terrain_generator = new TerrainGenerator(
        [](ld x, ld y) {
            if (-10 <= x && x < 10 && -30 <= y && y < 40) {
                return (double)10.0f;
            }
            return 5 * (cos(x / 10.0) + sin(y / 10.0)); 
        }, 
        [](ld x, ld y) {
            return vec3();
        },
        [](ld x, ld y) {
            return Color{ 1,1,1 } * ((cos(x / 10) + sin(y / 10.0) + 2) / 4.0);
        },
        vec3{ -50,1,-50}, 100, 100,2);

    terrain_generator->generate();
    //terrains.U.push_back(terrain_generator);

    glutInitWindowSize(width, height);
    glutCreateWindow("i.car");

    myinit();      /*---Initialize other state varibales----*/
    camera = new Camera(vec3(0, 12, -10),vec3(0,0,0));
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
    //draw_vec.push_back(spring_graph);
    car = new Vehicle();
    draw_vec.push_back(car);
    for (int i = 0; i < 4; ++i) {
        auto bx = new BoundingBox();
        bx->vert.pos = vec3(10  - (i+1) * 4, 12 , -4 - (i + 1));
        bx->color = i % 2 ? Color{ 1,1,1 } : Color{ 1,0,0 };
        bx->isBuilding = i % 2;
        BBs.push_back(bx);
        draw_vec.push_back(bx);
    }
    vd = new VoxelDrawer();
    /*----Associate callback func's whith events------*/
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(my_reshape);
    glutKeyboardFunc(my_quit);
    glutPassiveMotionFunc(passive_func);
    glutTimerFunc(1, mainLoop, -1);
    glutMainLoop();
}

