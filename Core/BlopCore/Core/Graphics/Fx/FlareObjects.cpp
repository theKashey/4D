#include "projectspace.h"
#include "FlareObjects.h"
#include "Flaring.h"

CFlareObject::CFlareObject()
{
	Init();SetType(LFT_NONE);
}
CFlareObject::CFlareObject(light_flare_type FlareT)
{
	Init();
	SetType(FlareT);
}

CFlareObject::~CFlareObject()
{
	delete MyCull;
}

void CFlareObject::SetPosition(const float *Pos)
{
	Position=Pos;
	MyCull->SetPosition(Pos);
}
void CFlareObject::SetMaterial(DWORD Mat)
{
	Material=Mat;
}
void CFlareObject::SetParentOcc(  CCullElement *Cull)
{
	MyCull->AddParent(Cull);
}

void CFlareObject::Init()
{
	MyCull=NULL;
	MyCull=CreateCullElement();
	FallOff=5;
	CurRad =48;
	Visibled=false;
}

bool CFlareObject::CheckCull()
{
//	float dist=(Position-world->current_cam->pos).Length()/FallOff;
	float dist=0;
	if(dist>CurRad) dist=CurRad;
	if(dist<16) dist=16;

	MyCull->SetUseHalf(0);
	MyCull->SetPosition(Position);
	MyCull->SetForm    (cullform_box);
	MyCull->SetRad     (dist*0.2f);
	//MyCull->FrameOCCFreq(
	Visibled= MyCull->IsVisibled();
	return Visibled;
}

void CFlareObject::Render()
{
	float flare_w=0;
	float flare_h=64;
	float flare_r=32;
	if(flare_type==LFT_NONE)return;
	if(!Visibled)return;
	FallOff=5;
	CurRad =48;

	if(flare_type==LFT_NEON_MESH)
	{
//		AddNeonMeshFlare(Color,Position,flare_w,flare_h,flare_r);
	}
	else if(flare_type==LFT_TEXTURED_CORONA)
	{
		AddCoronaFlare(Color,Position,FallOff,CurRad,Material);//,0,-1);
	}
	Visibled=false;
}