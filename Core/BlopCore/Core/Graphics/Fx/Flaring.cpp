#include "projectspace.h"
#include "projectgraphic.h"
#include "projectparticles.h"
#include "Flaring.h"


////////////////// flaring ////////////////////
enum FlareMeshPart
{
	FMP_CENTER_ONLY,
	FMP_LEFT_UP_PART_ONLY,
	FMP_RIGHT_DOWN_PART_ONLY,
	FMP_COMPLETE_MESH // vse 4asti v odnom meshe...
};



CparticleGPUDrawer *MyDrawer;

SmartVector<CParticle> CoronaParticles;

void AddCoronaParticle(const CParticle &Src)
{
  CoronaParticles.push_back(Src);
}

void AddCoronaFlare(const float* color,
					const float* position,
					float start_fallof_factor,
					float max_radius,
					DWORD tex_id)
{
	bVector3f r=World()->Camera()->GetPosition()-position;
	int x=r.Length()*start_fallof_factor;
	int max=max_radius;
	int min=16;
	if(x>max) x=max;
	if(x<min) x=min;
	CParticle Particle;
	Particle.Position=position;
	Particle.Size=x;
	Particle.SetColor(color);
	Particle.Texture=tex_id;
	AddCoronaParticle(Particle);
}