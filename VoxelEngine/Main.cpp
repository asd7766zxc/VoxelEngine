/*-----------------------------------------------------------
 *   An non-euclidan engine implementation
 *   Author: asd
 */

 //TODO:replace pointers with shared_ptr

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>


#include "Camera.hpp"
#include "Voxel.hpp"
#include "VoxelDrawer.hpp"
#include "SpringGraph.hpp"
#include "SpringSuperGraph.hpp"
#include "DrawingUtility.hpp"
#include "GlobalDefinitions.hpp"
#include "Vehicle.hpp"
#include "Portal.hpp"
#include "FrameBuffer.hpp"


Camera* camera;
SpringSuperGraph* spring_graph;

std::vector<GameObject* > draw_vec;
std::vector<GameObject* > portal_obj;
std::vector<Portal* > portals;
VoxelDrawer* vd;


int width = 512, height = 512;


Vehicle* car;
bool camera_following = false;

void mainLoop(int val) {
    for (auto obj : draw_vec) obj->update();
    glutTimerFunc(0, mainLoop, -1);
}

//main render event
void display(){
    for (auto& portal : portals) portal->warp(camera);
    static FrameBuffer fbo;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //全部變換 (包含投影都塞MODELVIEW裡)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Camera trace
    if (camera_following) {
        camera->front += vec3(0, 5, 0);
        camera->pos = car->pos + vec3(0, 5, 0);
    }

    auto drawScence = [&]() {
        terrain_generator->draw();
        for (auto obj : draw_vec) obj->draw();
        drawAxis();
	};

    for (auto& portal : portals) {
        portal->camDraw(*camera);
	        drawScence();
            camera->useMatrix();
	    portal->camEndDraw();
            drawScence();
    }

    fbo.generate();
    fbo.render();
    glutSwapBuffers();
    return;
}


void onReshape(int w, int h){

    width = w;
    height = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect_ratio = w / (float)h;
    float fov = 110.0f;
    float zfar = 10000.0f;
	camera->resize(w, h, 0.01f, zfar, fov);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//TODO : Key handling (remainder: 記得變成 key up，down (非 repeat key))
void keyboard_func(unsigned char key, int x, int y){

    if (key == 'Q' || key == 'q') exit(0);

    if (key == 'w') {
        camera->pos += uni(camera->front) * 1.0f;
    }
    if (key == 's') {
        camera->pos += uni(camera->front) * -1.0f;
    }
    if (key == 'a') {
        camera->pos += camera->getRight() * -1.0f;
    }
    if (key == 'd') {
        camera->pos += camera->getRight();
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

void passive_func(int x, int y) {
    camera->mouseMove(x - width/2,y - height/2);
    glutWarpPointer(width / 2, height / 2);
}

void main(int argc, char** argv){
    /*-----Initialize the GLUT environment-------*/
    glutInit(&argc, argv);

    /*-----Depth buffer is used, be careful !!!----*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
#pragma region Terrain
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
#pragma endregion

    glutInitWindowSize(width, height);
    glutCreateWindow("i.car");

#pragma region glInitializations
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
#pragma endregion


camera = new Camera(vec3(0, 12, -10),vec3(0,0,0));

#pragma region setup car

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

    car = new Vehicle();
    draw_vec.push_back(car);
    for (int i = 0; i < 4; ++i) {
        auto bx = new Box();
        bx->vert.pos = vec3(10  - (i+1) * 4, 12 , -4 - (i + 1));
        bx->color = i % 2 ? Color{ 1,1,1 } : Color{ 1,0,0 };
        bx->isBuilding = i % 2;
        BBs.push_back(bx);
        draw_vec.push_back(bx);
    }
    vd = new VoxelDrawer();
#pragma endregion
#pragma	region setup portals
    Portal* B = new Portal();
    B->pos = vec3(0, 20, -20);
    B->scale = vec3(10, 10, 1);

    Portal* A = new Portal(B);
    A->pos = vec3(20, 40, 0);
    B->linkto = A;

	portals.push_back(A);
#pragma endregion

    // Event registrations 
#pragma region EventRegistration
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(keyboard_func);
    glutPassiveMotionFunc(passive_func);
#pragma endregion

    glutTimerFunc(1, mainLoop, -1);
    glutMainLoop();
}

