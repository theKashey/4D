#pragma once

class CRagDude
{
	CBasePhisic *rigidBodies[0xFF];
public:
	CRagDude(void);
	~CRagDude(void);
	CBasePhisic* Make(bVector3f &_position);
	void ChangeCollClass(DWORD C);
	void MakeActive();

};
