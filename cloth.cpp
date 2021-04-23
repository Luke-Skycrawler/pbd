#include "global.h"
#include <GL/glut.h>
using namespace std;
using namespace glm;
static const glm::vec3 g(0.0f,-0.98f,0.0f);
void Cloth::reset(){
  float w=0.1f;
  for(int i=0;i<slicex;i++)
    for(int j=0;j<slicey;j++){
      glm::vec3 t(i*x/slicex-0.5f*x,0.7f,-j*y/slicey+0.5f*y);
      particles[i*slicey+j]=Particle(t,g,w);
    }
  if(pinned){
    GetParticle(0,0).               w=0.0f;
    GetParticle(0,slicey-1).        w=0.0f;
    GetParticle(slicex-1,0).        w=0.0f;
    GetParticle(slicex-1,slicey-1). w=0.0f;
    GetParticle(0,0).               acc=vec3(0.0f);
    GetParticle(0,slicey-1).        acc=vec3(0.0f);
    GetParticle(slicex-1,0).        acc=vec3(0.0f);
    GetParticle(slicex-1,slicey-1). acc=vec3(0.0f);
  }
}
void Cloth::gen(){
  for(int i=0;i<slicex;i++)
    for(int j=0;j<slicey;j++){
      // stretch constrains
      if(i<slicex-1)    constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i+1,j),kstretch));
      if(j<slicey-1)    constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i,j+1),kstretch));
      if(i&&j<slicey-1) constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i-1,j+1),kstretch));
      // TODO: bend constrains
      if(i<slicex-1&&j) constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i+1,j-1),kbend));
      if(i<slicex-2)    constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i+2,j),kbend));
      if(j<slicex-2)    constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i,j+2),kbend));
      // collision constrains
      constrains.push_back(Constrain(particles[i*slicey+j]));
    }
}
void Cloth::draw(){
  int col = 0;
  glBegin(GL_TRIANGLES);
  for(int w = 0; w < slicex - 1; w++){
    for(int h = 0; h < slicey - 1; h++){
      DrawTriangle(GetParticle(w+1,h  ), GetParticle(w,   h), GetParticle(w, h+1), col);
      DrawTriangle(GetParticle(w+1,h+1), GetParticle(w+1, h), GetParticle(w, h+1), col);
      col=!col;
    }
  }
  glEnd();
}
void Cloth::step(float dt){
  for(auto it=particles.begin();it!=particles.end();it++){
    it->v+=dt*it->acc;
    // damp(it->v);
    it->tmp=it->pos+dt*it->v;
    it->prelaunch();
    // if(length(it->tmp)<r+dh)
    //   constrains.push_back(Constrain(*it));
  }
  for(int i=0;i<iterations;i++){
    for(auto it=constrains.begin();it!=constrains.end();it++){
      it->solve();
    }    
  }
  for(auto it=particles.begin();it!=particles.end();it++){
    it->v=(it->tmp-it->pos)/dt;
    // collision set velocity to 0  
    if(it->collision[0])it->v-=it->tmp*(it->v*it->tmp);
    if(it->collision[1])it->v.y=0.0f;
    it->pos=it->tmp;
    // it->v_updated=false;
  }  
}
void Constrain::solve(){
  float l;
  static float r=ball.ball.radius;
  static const float dh=0.02f;
  // to reduce artifacts
  switch(m.size()){
    case 1:
      l=length(m[0]->tmp);
      if(l<r+dh){
        m[0]->tmp+=m[0]->tmp*(r+dh-l)/l;
        m[0]->collision[0]=true;
      }
      if(m[0]->tmp.y<dh){
        m[0]->tmp.y=dh;
        m[0]->collision[1]=true;
      }
      break;
    case 2:
      vec3 p10=m[0]->tmp-m[1]->tmp;
      l=length(p10);
      float C0=m[0]->w/(m[0]->w+m[1]->w),C1=1.0f-C0;
      m[0]->tmp+=C0*k*(len_slack-l)*(p10)/l;
      m[1]->tmp-=C1*k*(len_slack-l)*(p10)/l;
      break;
    // case 4:
  }
}
const vec3 Ball::color(0.4f,0.4f,0.8f);
