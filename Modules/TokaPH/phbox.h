// PHBOX.h: interface for the CPHBOX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHBOX_H__7E16FA16_8D9C_4F36_956F_56DAAFAFFF49__INCLUDED_)
#define AFX_PHBOX_H__7E16FA16_8D9C_4F36_956F_56DAAFAFFF49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProjectGraphic.h"

class CPHBOX:public Draw::CModel ,public BlopBaseObject  
{
public:
    CBasePhisic *ph;
	bVector3f    Scale;
	Draw::CDrawableElement Data;
	neT3 Tr;
	
public:
	CPHBOX(CBasePhisic *ph2=NULL);
	virtual ~CPHBOX();
	DWORD mySound;
	bool  mySoundPlaying;
	
	Draw::PDrawableDataArray GetData(Draw::CDrawableElement *Out);
	void    Move(float x,float y,float z);
	void    SetScale(float x,float y,float z);
	void    UnLink();
	void    SetTransform(neT3 &T);

	DWORD   OnMessage(const CBlopMessage &Message);

};

class CRocket:public CPHBOX//,public SmartObjectController
{
public:
	DWORD MyFireBallID;
	CRocket();
	~CRocket();
};

void CreateScene();

#endif // !defined(AFX_PHBOX_H__7E16FA16_8D9C_4F36_956F_56DAAFAFFF49__INCLUDED_)
