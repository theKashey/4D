#include "ProjectSpace.h"
#include "ProjectPhisic.h"
#include "ProjectParticles.h"
//#include "ParticleTemplate.h"

#define RegisterEmiter(a) RegisterParticleSybset(new a,"#a");

void InitParticleSybset()
  {
	  RegisterEmiter(CPointParticleEmiter);
	  RegisterEmiter(CPointSpeedEmiter);
	  RegisterEmiter(CPlaneParticleEmiter);
	  RegisterEmiter(CSphereParticleEmiter);
	  RegisterEmiter(CCloudRotater);
	  RegisterEmiter(CCloudRotaterMin);
	  RegisterEmiter(CGravitator);
	  RegisterEmiter(CPointParticleEmiter);

/*  RegisterParticleSybset(new CParticleSet<CPointParticleEmiter,PARD,PARD,PARD,CparticleCPUDrawer>(0),"CPointParticleEmiter,PARD,PARD,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPointParticleEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer>(0),"CPointParticleEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPointParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer>(0),"CPointParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPointParticleEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer>(0),"CPointParticleEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPointSpeedEmiter,PARD,PARD,PARD,CparticleCPUDrawer>(0),"CPointSpeedEmiter,PARD,PARD,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPointSpeedEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer>(0),"CPointSpeedEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPointSpeedEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer>(0),"CPointSpeedEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPointSpeedEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer>(0),"CPointSpeedEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPlaneParticleEmiter,PARD,PARD,PARD,CparticleCPUDrawer>(0),"CPlaneParticleEmiter,PARD,PARD,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPlaneParticleEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer>(0),"CPlaneParticleEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPlaneParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer>(0),"CPlaneParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPlaneParticleEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer>(0),"CPlaneParticleEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereParticleEmiter,PARD,PARD,PARD,CparticleCPUDrawer>(0),"CSphereParticleEmiter,PARD,PARD,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereParticleEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer>(0),"CSphereParticleEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer>(0),"CSphereParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereParticleEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer>(0),"CSphereParticleEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereSpeedParticleEmiter,PARD,PARD,PARD,CparticleCPUDrawer>(0),"CSphereSpeedParticleEmiter,PARD,PARD,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereSpeedParticleEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer>(0),"CSphereSpeedParticleEmiter,PARD,CCloudRotater,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereSpeedParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer>(0),"CSphereSpeedParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereSpeedParticleEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer>(0),"CSphereSpeedParticleEmiter,PARD,CGravitator,PARD,CparticleCPUDrawer");
  RegisterParticleSybset(new CParticleSet<CPointParticleEmiter,PARD,PARD,PARD,CparticleEmiterDrawer>(0),"CPointParticleEmiter,PARD,PARD,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPointParticleEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer>(0),"CPointParticleEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPointParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer>(0),"CPointParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPointParticleEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer>(0),"CPointParticleEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPointSpeedEmiter,PARD,PARD,PARD,CparticleEmiterDrawer>(0),"CPointSpeedEmiter,PARD,PARD,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPointSpeedEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer>(0),"CPointSpeedEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPointSpeedEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer>(0),"CPointSpeedEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPointSpeedEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer>(0),"CPointSpeedEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPlaneParticleEmiter,PARD,PARD,PARD,CparticleEmiterDrawer>(0),"CPlaneParticleEmiter,PARD,PARD,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPlaneParticleEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer>(0),"CPlaneParticleEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPlaneParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer>(0),"CPlaneParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CPlaneParticleEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer>(0),"CPlaneParticleEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereParticleEmiter,PARD,PARD,PARD,CparticleEmiterDrawer>(0),"CSphereParticleEmiter,PARD,PARD,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereParticleEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer>(0),"CSphereParticleEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer>(0),"CSphereParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereParticleEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer>(0),"CSphereParticleEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereSpeedParticleEmiter,PARD,PARD,PARD,CparticleEmiterDrawer>(0),"CSphereSpeedParticleEmiter,PARD,PARD,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereSpeedParticleEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer>(0),"CSphereSpeedParticleEmiter,PARD,CCloudRotater,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereSpeedParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer>(0),"CSphereSpeedParticleEmiter,PARD,CCloudRotaterMin,PARD,CparticleEmiterDrawer");
  RegisterParticleSybset(new CParticleSet<CSphereSpeedParticleEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer>(0),"CSphereSpeedParticleEmiter,PARD,CGravitator,PARD,CparticleEmiterDrawer");
}
//generated
//total:
// DRAWERS    : 2
// EMITERS    : 5
// REGISTERS  : 1
// CONTROLLERS: 4
// KILLERS    : 1
//
// total templates: 40
*/
  }