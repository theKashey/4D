#ifndef _PARTICLE_SHADER_H_
#define _PARTICLE_SHADER_H_

namespace Particles
{

DWORD DLLTYPE RegisterTemplate(LPCSTR Name,DWORD Enter);
DWORD DLLTYPE LoadShaders(LPCSTR FileName);
PBaseParticleSystem DLLTYPE CreateParticle(LPCSTR Name,int NumParticles=-1);

};

#endif