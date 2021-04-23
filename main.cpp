#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <GL/glut.h>


#include <vector>
#include <string>
#include <glm/glm.hpp>
#define _MAIN
#include "global.h"

void render_string(std::string& str, int w, int h, int x0, int y0) {
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRasterPos2f(x0, y0);
  int size = (int)str.size();
  for(int i = 0; i < size; ++i){
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
  }
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void init(int argc, char* argv[]){
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
  // glEnable(GL_CULL_FACE);

  GLfloat time = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  _main.SetTime(time);
}

void display(void){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glDepthFunc(GL_LESS); 
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);

  glPushMatrix();
  cloth.draw();
  glPopMatrix();

  glPushMatrix();
  ball.draw();
  glPopMatrix();

  glColor3d(1.0f, 1.0f, 1.0f);
  char debug[128];
  sprintf(debug, "ITERATION %d", _main.m_IterationNum);
  std::string iteration_text(debug);
  render_string(iteration_text, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 10, 20);
  sprintf(debug, "%s", MODE_STRING[_main.m_Mode]);
  std::string mode_text(debug);
  render_string(mode_text, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 10, 40);
  sprintf(debug, "TIME %d(ms)", _main.GetSolveTime());
  std::string time_text(debug);
  render_string(time_text, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 10, 60);

  glutSwapBuffers();
}

void reshape(int width, int height){
  static GLfloat lightPosition[4] = {0.0f,  2.5f,  5.5f, 1.0f};
  static GLfloat lightDiffuse[3]  = {1.0f,  1.0f,  1.0f      };
  static GLfloat lightAmbient[3]  = {0.75f, 0.75f, 0.75f     };
  static GLfloat lightSpecular[3] = {1.0f,  1.0f,  1.0f      };

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glShadeModel(GL_SMOOTH);

  glViewport(0, 0, width, height);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (double)width / (double)height, 0.0001f, 1000.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(0.0f, 2.0f, 5.0f, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // pos, tgt, up

  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

void idle(void){
  static const int SlowMotion=5.0f;
  GLfloat time = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  GLfloat dt = time - _main.GetTime();

  dt = (dt > 0.033f) ? 0.033f : dt; // keep 30fps

  cloth.step(dt/SlowMotion);

  _main.SetTime(time);
  glutPostRedisplay();
}

void restart(){
}
void keyboard(unsigned char key , int x , int y){
  switch(key){
    case 27: exit(0); break; // esc
    case '\n':restart();break;
  }
}

void special(int key, int x, int y){
  if (key == GLUT_KEY_UP) {
    _main.m_IterationNum++;
  }
  if (key == GLUT_KEY_DOWN) {
    if (_main.m_IterationNum > 1){
      _main.m_IterationNum--;
    }
  }
  if (key == GLUT_KEY_LEFT) {
    if (_main.m_Mode > eModePBD) {
      _main.m_OldMode = _main.m_Mode;
      _main.m_Mode--;
    }
  }
  if (key == GLUT_KEY_RIGHT) {
    if (_main.m_Mode < eModeMax - 1) {
      _main.m_OldMode = _main.m_Mode;
      _main.m_Mode++;
    }
  }
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(800  , 600);
  glutCreateWindow("Position-Based Dynamics");

  init(argc, argv);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);

  glutMainLoop();
  return 0;
}
