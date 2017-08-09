#pragma once

class CInput;

class CInputFeedBack
{
	GString Name;
	CInput  *Parent;
protected:
	void SetName(LPCSTR F,...);
public:
	CInputFeedBack(void);
	~CInputFeedBack(void);
	void SetParent(CInput *P);
	virtual bool AllowCrossfall();

	virtual void OnMouseMove(float x,float y,DWORD Key);
	virtual void OnMouseUp  (float x,float y,DWORD Key);
	virtual void OnMouseDown(float x,float y,DWORD Key);

	virtual void OnKeyChar  (DWORD Key);
	virtual void OnKeyDown  (DWORD Key);
	virtual void OnKeyUp    (DWORD Key);
	virtual void InpStart   ();
	virtual void Tick();

};

class CInput:public BaseObject
{
	typedef SmartVector<CInputFeedBack*> CInputFeedbackPack;
	CInputFeedbackPack  Inputs;
	bool Keys[0xFF];
public:
	CInput();
	~CInput();

	bool AddInput      (CInputFeedBack* obj);
	bool RemoveInput   (CInputFeedBack* obj);
	bool SetActiveInput(CInputFeedBack* obj);

	virtual void OnMouseMove(float x,float y,DWORD Key);
	virtual void OnMouseUp  (float x,float y,DWORD Key);
	virtual void OnMouseDown(float x,float y,DWORD Key);

	virtual void OnKeyChar  (DWORD Key);
	virtual void OnKeyDown  (DWORD Key);
	virtual void OnKeyUp    (DWORD Key);
	virtual bool IsKeyOn    (DWORD Key);

	bool Start();
	DWORD Tick(float Time);

};
