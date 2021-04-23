#include <glm/glm.hpp>
#include <GL/glut.h>
#include <vector>
struct Ball{
  float radius;
  Ball(float radius=0.6f):radius(radius){}
  static const glm::vec3 color;
  void draw(glm::vec3 pos=glm::vec3(0.0f)){
    glTranslatef(pos.x,pos.y,pos.z);
    glColor3fv((float*)&color);
    glutSolidSphere(radius,30,30);
  }
};
struct Particle{
  float w;    // 1/mass
  glm::vec3 pos,acc,v,tmp;
  // bool v_updated;
  Particle(glm::vec3 pos,glm::vec3 gravity,float w)
  :w(w),v(0.0f),pos(pos),acc(gravity){}
  Particle():w(w),v(0.0f),pos(0.0f),acc(0.0f){}
  bool collision[2];
  // FIXME: only for convenience, attributed to Constrain
  void prelaunch(){collision[0]=collision[1]=false;}
};

struct Ball_Dynamic : Particle{
  Ball ball;
  Ball_Dynamic(float radius,glm::vec3 pos,glm::vec3 acc,float w)
  :ball(radius),Particle(pos,acc,w){
    
  }
  void draw(){ball.draw(pos);}
};
struct Constrain{
  std::vector<Particle*> m;
  Particle * ext_obj;
  float k,len_slack;
  Constrain(Particle &m1):ext_obj(NULL){
    m.push_back(&m1);
  }
  Constrain(Particle &m1,Particle &m2,float kstretch=1.0f)
  :k(kstretch),len_slack(glm::length(m1.pos-m2.pos)),ext_obj(NULL){
    m.push_back(&m1);
    m.push_back(&m2);
  }
  // Constrain(Particle &m1,Particle &m2,Particle &m3,Particle &m4,float kbend=1.0f)
  // :k(kbend){
  //   m.push_back(&m1);
  //   m.push_back(&m2);
  //   m.push_back(&m3);
  //   m.push_back(&m4);
  // }
  void solve();
};
struct Cloth{
  float x,y,kbend,kstretch;
  static const int iterations=100;
  int slicex,slicey,gerneric_constrains_cnt;

  std::vector<Constrain> constrains;
  std::vector<Particle> particles;
  bool pinned;
  Cloth(float x,float y,int slicex,int slicey,float kstretch=1.0f,float kbend=0.0f,bool pinned=false):
  x(x),y(y),slicex(slicex),slicey(slicey),kstretch(kstretch),kbend(kbend),pinned(pinned){
    particles.resize(slicex*slicey);
    reset();
    gen();
  }
  void gen();
  void reset();
  void draw();
  void step(float dt);
  private:
  inline void DrawTriangle(Particle &p1,Particle &p2,Particle &p3,int cid){
    static glm::vec3 color[2]={
      glm::vec3(1.0f,1.0f,1.0f),
      glm::vec3(1.0f,0.6f,0.6f)
    };
    glColor3fv((float*)&color[cid]);
    glVertex3fv((float*)&p1.pos);
    glVertex3fv((float*)&p2.pos);
    glVertex3fv((float*)&p3.pos);
  }
  inline Particle &GetParticle(int i,int j){
    return particles[i*slicey+j];
  }
};

struct Plane{
  Plane(float w=5.0f):w(w){}
  const float w;
  void draw(){
    glColor3f(0.7f,0.7f,0.7f);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(-w,0.0f, w);
    glVertex3f( w,0.0f, w);
    glVertex3f(-w,0.0f,-w);
    glVertex3f( w,0.0f,-w);
    glEnd();
  }
};
// global variables
#ifdef _MAIN
#else
extern Ball_Dynamic ball;
extern Cloth cloth;
#endif