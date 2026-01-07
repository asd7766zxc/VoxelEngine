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
#include "Tunnel.hpp"
#include "GlassSphere.hpp"
#include "SphereMap.hpp"
#include "EnderTexture.hpp"
#include "Skybox.hpp"
#include "ShootingStarTexture.hpp"
#include "LightMap.hpp"

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

float fov = 110.0f;
float zfar = 100.0f;
float ortho_sz = 10.0f;
mat4 proj_record;
mat4 view_record;
FrameBuffer* fbo;
int curselect = 3;

bool light_0_on = true;
bool light_1_on = true;

enum LightIndex {
    SUN = 0,
    POT,
    CAR,
    FOG,
    LightCount
} currentLight;

struct LightParam {
    float value;
    float minValue = 0.0;
    float maxValue = 2 * pi;
    float step = 0.1;
    LightParam(float minValue = 0.0, float maxValue = 2 * pi, float value = 0.0, float step = 0.1) : minValue(minValue), maxValue(maxValue), value(value), step(step) {
        value = std::max(minValue, std::min(maxValue, value));
    }
    void add() {
        value += step;
        value = std::min(value, maxValue);
    }
    void sub() {
        value -= step;
        value = std::max(value, minValue);
    }
};
int selectedLightParam[LightCount];
std::vector<LightParam> lightParams[LightCount] = {
    {
        LightParam(0.0f, 200 * pi,DegreeToRad(60)), //sun angle
        LightParam(0.0f, 1.0,0.0), // sun light intensity
        LightParam(0.0f,2 * pi,DegreeToRad(35)) , // sun light color
        LightParam(0.0f,1.0f,1.0f,1.0f) // sun on/off
    },{
        LightParam(0.0f, 1.0,0.2), // point light intensity
        LightParam(0.0f,2 * pi,DegreeToRad(180)) , // point light color
        LightParam(0.0f,1.0f,1.0f,1.0f) // point light on/off
    },{
        LightParam(0.0f, 1.0,1.0), // car head light intensity
        LightParam(0.0f,2 * pi,DegreeToRad(135)) , // car head light color
        LightParam(0.0f,180.0f,30.0f,1.0f), // car head light cutoff angle
        LightParam(0.0f,1.0f,1.0f,1.0f) // car head light on/off
    },{
        LightParam(0.0f,2 * pi,DegreeToRad(35)), // fog color
        LightParam(0.0f,40.0f,10.0f,1.0f), // fog mode
        LightParam(0.0f,1.0f,0.5f,0.05f), // fog opacity
    }
};

void setup_light() {
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vec4(0.3, 0.3, 0.3, 1.0));

    lightParams[POT].back().value > 0.5f ? glEnable(GL_LIGHT0) : glDisable(GL_LIGHT0);

	//Point light
	auto pot_color = GetColorFrom(lightParams[POT][1].value) * lightParams[POT][0].value;
    pot_color.a = 1.0f;
    glPushMatrix();
        glTranslatef(0,70.0f,0);
        applyColorMaterials({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, pot_color, 0);
        draw_unisphere();
    glPopMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, vec4(0, 70.0,0, 1.0));
    glLightfv(GL_LIGHT0, GL_DIFFUSE, pot_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, pot_color);

	auto sun_angle = lightParams[SUN][0].value;
	auto sun_dir = vec3(cos(sun_angle), sin(sun_angle), 0);
	auto sun_color = GetColorFrom(lightParams[SUN][2].value) * lightParams[SUN][1].value;
	sun_color.a = 1.0f;
    //Directional light (SUN)
	lightParams[SUN].back().value > 0.5f ? glEnable(GL_LIGHT1) : glDisable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, vec4(sun_dir, 0.0));
    glLightfv(GL_LIGHT1, GL_DIFFUSE, sun_color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, sun_color);

    for (auto carlights : car->lights) {
		carlights->light_on = lightParams[CAR].back().value > 0.5f;
		carlights->light_color = GetColorFrom(lightParams[CAR][1].value) * lightParams[CAR][0].value;
		carlights->cutoff_angle = lightParams[CAR][2].value;
        if (camera_following) {
			carlights->rot.y = -camera->yx;
            carlights->rot.x = camera->rx;
        }
    }
}

shared_ptr<SphereMap> sphere_map;
shared_ptr<Skybox> skybox;
shared_ptr<EnderTexture> ender_tex;
std::vector<shared_ptr<Texture>> tex_gen_list;
shared_ptr<ShootingStarTexture> shoot_tex;
std::vector<shared_ptr<billboard>> bill_list;
shared_ptr<LightMap> lightmap;

void genTexture(){
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    for (auto tex : tex_gen_list) tex->render();
    //glClearColor(0, 0, 0, 1);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
}

float time_elapsed = 0.0f;
//main render event
void display(){
    genTexture();

    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    skybox->render(camera);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    if(lightParams[FOG][1].value >= 1.0f)
        glEnable(GL_FOG);
    else
		glDisable(GL_FOG);

    if (lightParams[FOG][1].value >= 30.0f) {
        glFogi(GL_FOG_MODE, GL_EXP2);
    }
    else if(lightParams[FOG][1].value >= 20.0f) {
        glFogi(GL_FOG_MODE, GL_EXP);
    }
    else if (lightParams[FOG][1].value >= 10.0f) {
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 0);
        glFogf(GL_FOG_END, 200);
    }

    glFogfv(GL_FOG_COLOR, GetColorFrom(lightParams[FOG][0].value));
	glFogf(GL_FOG_DENSITY, lightParams[FOG][2].value);

    for (auto& portal : portals) portal->warp(camera);

    auto drawScence = [&]() {
        setup_light();
        terrain_generator->draw();
        for (auto obj : draw_vec) obj->draw();
        drawAxis(50.0f);

        std::vector<vec3> corners =
        { vec3(-1, -1, -1),
         vec3(-1, 1, -1),
         vec3(1, 1, -1),
         vec3(1, -1, -1) };
         auto toWorld = proj_record.inverse();
         toWorld = (view_record.inverse()) * toWorld;

         auto trans = [&](vec3 a) {
             auto ret = toWorld * vec4(a,1);
             return ret.toVec3() / ret.w;
         };


         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

         glColor4f(1, 1, 0, 0.5);
         glBegin(GL_POLYGON);
         for (auto v : corners) {
             glVertex3fv(trans(v));
         }
         glEnd();
         for(int i = 0; i < 4;++i) {
            glColor4f(1, 1, 0, 0.5);
            glBegin(GL_POLYGON);
             auto p0 = corners[i];
             auto p1 = corners[(i + 1) % 4];
             auto p2 = p1;
             p2.z = 1;
             auto p3 = p0;
             p3.z = 1;

             glVertex3fv(trans(p0));
             glVertex3fv(trans(p1));
             glVertex3fv(trans(p2));
             glVertex3fv(trans(p3));
            glEnd();
            glColor4f(1, 1, 1, 1);

            //drawLine(trans(p0), trans(p3));
            //drawLine(trans(p1), trans(p2));
         }
         glEnable(GL_ALPHA_TEST);
         glAlphaFunc(GL_GREATER, 0.2f);
         glMatrixMode(GL_TEXTURE);
         glLoadIdentity();
         glPushMatrix();
         glTranslatef(time_elapsed, time_elapsed, 0);
         time_elapsed += 0.01f;
         glMatrixMode(GL_MODELVIEW);
         for (auto bill : bill_list) {
			 bill->norm = camera->view.transposed().z_axis();
             bill->draw(camera);
         }
         glMatrixMode(GL_TEXTURE);
         glPopMatrix();
         glMatrixMode(GL_MODELVIEW);
         glDisable(GL_ALPHA_TEST);
         glDisable(GL_BLEND);
    };
    auto draw_world = [&]() {
        for (auto& portal : portals) {
            portal->camDraw(*camera);
            drawScence();
            camera->applyMatrix();
            portal->camEndDraw();
            drawScence();
        }
    };

    //Camera trace
    if (camera_following) {
        camera->front += vec3(0, 5, 0);
        camera->pos = car->pos + vec3(0, 5, 0);
    }
    

    /*sphere_map->generate(drawScence);
	glViewport(0, 0, width, height);*/
    /*if (curselect == 4) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        std::vector<ld> A = { 0.0f, float(width / 2.0) };
        std::vector<ld> B = { 0.0f, float(height / 2.0) };
        std::vector<vec3> axis = {
            {1,0,0},
            {0,1,0},
            {0,0,1},
        };
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                int ind = i * 2 + j;
                auto dx = A[i];
                auto dy = B[j];
                if (ind == 3) {
                    camera->resize(width/2, height/2, 0.01f, zfar, fov);
                    glViewport(0 + dx, 0 + dy, width/2, height/2);
                    draw_world();
                }
                else {
                    vec3 _pos = camera->pos;
                    auto mt = camera->view;
                    camera->pos = axis[ind] * 55.f;
                    camera->lookAt(vec3(0, 0, 0.01));
                    camera->make_ortho(width/2, height/2, ortho_sz);
                    glViewport(0 + dx, 0 + dy, width/2, height/2);
                    draw_world();
                    camera->view = mt;
                    camera->pos = _pos;
                }
            }
        }
    }
    else {
        draw_world();
    }*/
    draw_world();

    //auto [x, y, z, w] = (camera->Matrix() * vec4(car->pos,1));
    //fbo->setCen(x/w, y/w);
    //fbo->generate();
    //fbo->render();
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    //genTexture();

    //glDisable(GL_TEXTURE_2D);
    //glEnable(GL_TEXTURE_2D);
    ender_tex->Bind();
   
    //camera->applyMatrix();
    //glClearColor(0, 0, 0, 0);
    //glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
        glTexCoord2f(0, 0);
        glVertex3f(10, 10,0);
        
        glTexCoord2f(1, 0);
        glVertex3f(20, 10,0);
        
        glTexCoord2f(1, 1);
        glVertex3f(20, 20,0);

        glTexCoord2f(0, 1);
        glVertex3f(10, 20,0);
    glEnd();

    //draw_world();

    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluOrtho2D(0, 1, 0, 1);
    auto a = uni(skybox->bills[0]->pos);
    auto front = uni(-camera->view.transposed().z_axis());
    glEnable(GL_BLEND);
    auto halo = [&](float r, Color color) {
        int slice = 100;
        glPushMatrix();
        glTranslatef(.5, .5, 0);
        for (int i = 0; i < slice; ++i) {
            float theta = (float(i) / slice) * 2 * pi;
            float theta2 = (float(i + 1) / slice) * 2 * pi;
            glBegin(GL_POLYGON);
            glColor4f(0, 0, 0, 0);
            glVertex2f(r * cos(theta), r * sin(theta));
            glColor4f(0, 0, 0, 0);
            glVertex2f(r * cos(theta2), r * sin(theta2));
            glColor4fv(color);
            glVertex2f(0, 0);

            glEnd();
        }
        glPopMatrix();
        };
    halo(0.3, Color(1, 1, 1, std::max(a * front - 0.5f, 0.0f) ));
    halo(0.5, Color(1, 1, 1, std::max(a * front - 0.5f, 0.0f) * 0.1));

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
    
    float axi_len = 55.0f;
    switch (curselect) {
        case 0:
            camera->pos = vec3(axi_len, 0, 0);
            camera->lookAt(vec3(0, 0, 0));
            camera->make_ortho(w, h, ortho_sz);
            break;
        case 1:
            camera->pos = vec3(0, axi_len, 0);
            camera->lookAt(vec3(0, 0, 0.01));
            camera->make_ortho(w, h, ortho_sz);
            break;
        case 2:
            camera->pos = vec3(0, 0, axi_len);
            camera->lookAt(vec3(0, 0.01, 0));
            camera->make_ortho(w, h, ortho_sz);
            break;
        case 3:
            camera->resize(w, h, 0.01f, zfar, fov);
            break;
        case 4:

            break;
    };
	//camera->resize(w, h, 0.01f, zfar, fov);
    //camera->make_ortho(w,h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    fbo->height = h;
    fbo->width = w;
}


void onMouse(int button, int state, int x, int y) {

    //Scroll up
    if (button == 3) {
		lightParams[currentLight][selectedLightParam[currentLight]].add();
        if(currentLight == CAR && selectedLightParam[currentLight] == 2) {
            for (auto light : car->lights) {
                light->t = light->animation_time;
            }
		}
    }

	//Scroll down
    if (button == 4) {
        lightParams[currentLight][selectedLightParam[currentLight]].sub();
        if (currentLight == CAR && selectedLightParam[currentLight] == 2) {
            for (auto light : car->lights) {
                light->t = light->animation_time;
            }
        }
	}
}

//TODO : Key handling (remainder: 記得變成 key up，down (非 repeat key))
void keyboard_func(unsigned char key, int x, int y){
    if (key == '0') {
        light_0_on = !light_0_on;
    }
    if (key == '9') {
		light_1_on = !light_1_on;
    }
    if ('1' <= key && key <= '5') {
        curselect = key - '1';
        onReshape(width, height);
        return;
    }
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

    if (key == 'p') {
        proj_record = camera->proj;
        view_record = camera->view;
    }

    if (key == 'r') {
        camera->rz += 0.01f;
    }

    if (key == 'x') {
        fov += 1.0f;
        ortho_sz += 1.0f;
        onReshape(width, height);
    }
    if (key == 'z') {
        fov -= 1.0f;
        ortho_sz -= 1.0f;
        onReshape(width, height);
    }
    if (key == ',') {
		currentLight = currentLight <= 0 ? LightIndex(0) : LightIndex(currentLight - 1);
    }
    if (key == '.') {
        currentLight = currentLight >= (LightCount - 1) ? LightIndex(LightCount - 1) : LightIndex(currentLight + 1);
    }
    if (key == 'l') {
        auto& p_ind = selectedLightParam[currentLight];
		p_ind = (p_ind + 1) % lightParams[currentLight].size();
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#pragma region Terrain
    terrain_generator = new TerrainGenerator(
        [](ld x, ld y) { //F
            if (-10 <= x && x < 80 && -30 <= y && y < 80) {
                return (double)10.0f;
            }
            return 5 * (cos(x / 10.0) + sin(y / 10.0)); 
        }, 
        [](ld x, ld y) { //dF
            if (-10 <= x && x < 80 && -30 <= y && y < 80) {
                return vec3(0,1,0);
            }
            return vec3(-sin(x/10.0) * 0.5, 1, cos(y / 10.0) * 0.5);
        },
        [](ld x, ld y) { //colorF
            return Color{ 1,1,1 };
            //return Color{ 1,1,1 } * ((cos(x / 10) + sin(y / 10.0) + 2) / 4.0);
        },
        vec3{ -50,1,-50},
            200, 200,4);
    //terrain_generator->ground_tex = ender_tex;
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
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);

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
        auto bx = new Box(vec3{1,1,1} * 4);
 
        bx->vert.pos = vec3(10  - (i+1) * 4, 12 , -4 - (i + 1));
        bx->color = i % 2 ? Color{ 1,1,1 } : Color{ 1,0,0 };
        bx->isBuilding = 1;
        BBs.push_back(bx);
        draw_vec.push_back(bx);
    }
    vd = new VoxelDrawer();
	sphere_map = make_shared<SphereMap>();
#pragma endregion

#pragma	region setup portals
    Portal* B = new Portal();
    B->pos = vec3(0, 20, -20);
    B->scale = vec3(10,10, 1);

    Portal* A = new Portal(B);
    A->pos = vec3(20, 40, 0);
    B->linkto = A;

    Portal* mirr = new Portal();
    mirr->pos = vec3(20, 25, -10);
    mirr->rot = vec3(0,pi,0);
    mirr->scale = vec3(10, 10, 1);

    Portal* mirro = new Portal(mirr);
    mirro->pos = vec3(20, 25, -10);
    mirro->linkto = mirr;
	//portals.push_back(A);
    portals.push_back(mirro);
#pragma endregion
    
    fbo = new FrameBuffer();
    // Event registrations 
#pragma region EventRegistration
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(keyboard_func);
    glutPassiveMotionFunc(passive_func);
    glutMouseFunc(onMouse);
#pragma endregion

#pragma region Texture

    //terrain_generator->ground_tex = make_shared<EnderTexture>(512, 512, 1.0f, 0.1f, Color(0), 200,Color(1,1,1,1));
    skybox = make_shared<Skybox>();
    //tex_gen_list.push_back(terrain_generator->ground_tex);
    tex_gen_list.push_back(skybox->shoot_tex);

    ender_tex = make_shared<EnderTexture>(512, 512);
    //shoot_tex = make_shared<ShootingStarTexture>(512,512);
    tex_gen_list.push_back(ender_tex);
    
    lightmap = make_shared<LightMap>();
    lightmap->light = car->lights[0];
    tex_gen_list.push_back(lightmap);

    tex_gen_list.push_back(skybox->end_tex);
    for (int i = 0; i < 100; ++i) {
        auto bill = make_shared<billboard>(ender_tex);
		bill->rand = true;
	    bill->pos = vec3(30 + random_double() * 5, 30 + random_double() * 10, 30 + random_double() * 5);
	    bill->size = random_double()*2;
        bill_list.push_back(bill);

    }


    terrain_generator->ground_tex = lightmap;
    terrain_generator->isTexturing = true;

#pragma endregion

	//glPolygonOffset(1.0f, 1.0f);
    glutTimerFunc(1, mainLoop, -1);
    glutMainLoop();
}

