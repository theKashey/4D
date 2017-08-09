#pragma once
#ifndef _C_CAMERA_H
#define _C_CAMERA_H

class DLLTYPE CCamera: public BaseObject
{
	bVector3f Position;
	bVector3f LookAt  ;
	DWORD     syncObject;
	DWORD     Type;
	float     Width,Height;
	int       Viewport[4];
	float      Projection[16], MPV[16], ModelView[16];
    double    dProjection[16],dMPV[16],dModelView[16];
	float     Fov,Zfar,Znear;

	void      SetViewPort();
	void      StartUpInit();
	bool      ChangeIsOn;
public:
	 CCamera(void);
	~CCamera(void);

	const float* GetModelView ();
	const float* GetProjection();
	const float* GetMPV       ();

	void TransformGraphic();
	void TransformSound  ();
	void TransformAll();

	void Sync();
	void Translate(const bVector3f &TR);
	void Rotate   (const bVector3f &RT);
	void Scale    (const bVector3f &SC);
	void StoreMatrix();
	void Init();
	void Select3D();
	void Select2D();

    const bVector3f& GetPosition();

	void SetSyncObject(DWORD Obj){syncObject=Obj;};

	bVector3f GetMoveShift(BYTE Forward,BYTE SLeft,BYTE AddY=0);
	void SetLookAtAdd  (float X,float Y,float Z);
	void SetPosAdd     (bVector3f &Pos);
	void SetPosition   (bVector3f &Pos);
	void SetLookAngles (bVector3f &Pos);
    void SetLookAt     (bVector3f &Pos);
	void ClearPos();
	bool EnableChanges (bool OnOff);

	bVector3f GetLookVector();
	bVector3f GetFrustumVector();

	DWORD OnMessage(const CBlopMessage &Message);

	DWORD GetWidth(){return Width;}
	DWORD GetHeight(){return Height;}
	float WHRatio(){return Width/Height;}

//	bVector3f GetPosition();
	const bVector3f&  GetLookAngles();

    void ScreenToClient(const bVector3f &In,bVector3f &Out);
    void ClientToScreen(const bVector3f &In,bVector3f &Out);
};

	#endif