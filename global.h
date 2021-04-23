#include <glm/glm.hpp>
#include <GL/glut.h>
#include <vector>
enum eMode {
  eModePBD,
  eModeXPBD_Concrete,
  eModeXPBD_Wood,
  eModeXPBD_Leather,
  eModeXPBD_Tendon,
  eModeXPBD_Rubber,
  eModeXPBD_Muscle,
  eModeXPBD_Fat,
  eModeMax,
};

static const char* MODE_STRING[eModeMax] = {
  "PBD",
  "XPBD(Concrete)",
  "XPBD(Wood)",
  "XPBD(Leather)",
  "XPBD(Tendon)",
  "XPBD(Rubber)",
  "XPBD(Muscle)",
  "XPBD(Fat)",
};

static const float MODE_COMPLIANCE[eModeMax] = {
  0.0f,            // Miles Macklin's blog (http://blog.mmacklin.com/2016/10/12/xpbd-slides-and-stiffness/)
  0.00000000004f, // 0.04 x 10^(-9) (M^2/N) Concrete
  0.00000000016f, // 0.16 x 10^(-9) (M^2/N) Wood
  0.000000001f,   // 1.0  x 10^(-8) (M^2/N) Leather
  0.000000002f,   // 0.2  x 10^(-7) (M^2/N) Tendon
  0.0000001f,     // 1.0  x 10^(-6) (M^2/N) Rubber
  0.00002f,       // 0.2  x 10^(-3) (M^2/N) Muscle
  0.0001f,        // 1.0  x 10^(-3) (M^2/N) Fat
};

class CApplication{
private:
  float m_Time;
  int   m_SolveTime;
public:
  int   m_IterationNum;
  int   m_Mode;
  int   m_OldMode;
  CApplication() :
  m_Time(0.0f), m_SolveTime(0), m_IterationNum(20), m_Mode(eModePBD), m_OldMode(eModeMax){}

  float GetTime(){ return m_Time; }
  void  SetTime(float time){ m_Time = time; }
  int   GetSolveTime(){ return m_SolveTime; }
  void  SetSolveTime(float time){ m_SolveTime = time; }
};
struct Particle{
  float w;    // 1/mass
  glm::vec3 pos,acc,v,tmp;
  // bool v_updated;
  Particle(glm::vec3 pos,glm::vec3 gravity,float w)
  :w(w),v(0.0f,0.0f,0.0f),pos(pos),acc(gravity){}
  Particle():w(w),v(0.0f),pos(0.0f),acc(0.0f){}
};
struct Constrain{
  std::vector<Particle*> m;
  float kbend,kstretch,len_slack;
  Constrain(Particle &m1){
    m.push_back(&m1);
  }
  Constrain(Particle &m1,Particle &m2,float kstretch=1.0f)
  :kstretch(kstretch),len_slack(glm::length(m1.pos-m2.pos)){
    m.push_back(&m1);
    m.push_back(&m2);
  }
  Constrain(Particle &m1,Particle &m2,Particle &m3,Particle &m4,float kbend=1.0f)
  :kbend(kbend){
    m.push_back(&m1);
    m.push_back(&m2);
    m.push_back(&m3);
    m.push_back(&m4);
  }
  void solve();
};
static const float dh=0.005f;
struct Cloth{
  float x,y,k;
  static const int iterations=20;
  int slicex,slicey,gerneric_constrains_cnt;

  std::vector<Constrain> constrains;
  std::vector<Particle> particles;
  
  Cloth(float x,float y,int slicex,int slicey,float k=1.0f):
  x(x),y(y),slicex(slicex),slicey(slicey),k(k){
    particles.resize(slicex*slicey);
    reset();
  }
  void reset(){
    glm::vec3 g(0.0f,-0.98f,0.0f);
    float w=0.1f;
    for(int i=0;i<slicex;i++)
      for(int j=0;j<slicey;j++){
        glm::vec3 t(i*x/slicex-0.5f*x,0.7f,-j*y/slicey+0.5f*y);
        particles[i*slicey+j]=Particle(t,g,w);
        // stretch constrains
        if(i<slicex-1)    constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i+1,j)));
        if(j<slicey-1)    constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i,j+1)));
        if(i&&j<slicey-1) constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i-1,j+1)));
        // TODO: bend constrains
        
      }
  }
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
struct Ball{
  float radius;
  Ball(float radius=0.6f):radius(radius),color(0.4f,0.4f,0.8f){}
  const glm::vec3 color;
  void draw(){
    glColor3fv((float*)&color);
    glutSolidSphere(radius,30,30);
  }
};

// global variables
#ifdef _MAIN
float r;
Ball ball;
Cloth cloth(2.0f,2.0f,50,50);
CApplication _main;
#else
extern float r;
extern CApplication _main;
extern Ball ball;
extern Cloth cloth;
#endif