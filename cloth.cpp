#include "global.h"
#include <GL/glut.h>
using namespace std;
using namespace glm;

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
    it->tmp+=dt*dt*it->v;
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
  for(int i=gerneric_constrains_cnt;i<constrains.size();i++){
    constrains[i].m[0]->v=vec3(0.0f);
  }
  
  // cancel the collision constrains
  // constrains.resize(gerneric_constrains_cnt);
}
void Constrain::solve(){
  
  switch(m.size()){
    case 1:
      if(length(m[0]->pos)<r+dh){
        float l=length(m[0]->pos);
        m[0]->pos+=m[0]->pos*(r+dh-l)/l;
      }
      break;
    case 2:
      vec3 p10=m[0]->pos-m[1]->pos;
      float l=length(p10),C0=m[0]->w/(m[0]->w+m[1]->w),C1=1-C0;
      m[0]->pos+=C0*kstretch*(len_slack-l)*(p10)/l;
      m[1]->pos-=C1*kstretch*(len_slack-l)*(p10)/l;
      break;
    // case 4:
  }
}
// void Cloth::reset(){

// }