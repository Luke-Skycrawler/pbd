#include "global.h"
#include <GL/glut.h>
using namespace std;
using namespace glm;
void Cloth::reset(){
  glm::vec3 g(0.0f,-0.98f,0.0f);
  float w=0.1f;
  for(int i=0;i<slicex;i++)
    for(int j=0;j<slicey;j++){
      glm::vec3 t(i*x/slicex-0.5f*x,0.7f,-j*y/slicey+0.5f*y);
      particles[i*slicey+j]=Particle(t,g,w);
    }
}
void Cloth::gen(){
  for(int i=0;i<slicex;i++)
    for(int j=0;j<slicey;j++){
      // stretch constrains
      if(i<slicex-1)    constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i+1,j),k));
      if(j<slicey-1)    constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i,j+1),k));
      if(i&&j<slicey-1) constrains.push_back(Constrain(GetParticle(i,j),GetParticle(i-1,j+1),k));
      // TODO: bend constrains
      
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
    it->pos=it->tmp;
    // it->v_updated=false;
  }
  // collision set velocity to 0  
  // for(int i=gerneric_constrains_cnt;i<constrains.size();i++){
  //   constrains[i].m[0]->v=vec3(0.0f);
  // }
  
  // cancel the collision constrains
  // constrains.resize(gerneric_constrains_cnt);
}
void Constrain::solve(){
  float l;
  static float r=ball.radius;
  static const float dh=0.02f;
  // to reduce artifacts
  switch(m.size()){
    case 1:
      l=length(m[0]->tmp);
      if(l<r+dh){
        m[0]->tmp+=m[0]->tmp*(r+dh-l)/l;
      }
      break;
    case 2:
      vec3 p10=m[0]->tmp-m[1]->tmp;
      l=length(p10);
      float C0=m[0]->w/(m[0]->w+m[1]->w),C1=1.0f-C0;
      m[0]->tmp+=C0*kstretch*(len_slack-l)*(p10)/l;
      m[1]->tmp-=C1*kstretch*(len_slack-l)*(p10)/l;
      break;
    // case 4:
  }
}
// void Cloth::reset(){

// }