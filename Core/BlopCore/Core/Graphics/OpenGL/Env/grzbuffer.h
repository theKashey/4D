#pragma once

#define ZLESS    1
#define ZLEQUAL  2
#define ZEQUAL   3
#define ZGREATER 4
#define ZGEQUAL  5

class DLLTYPE CgrZBuffer
{
	bool   Enabled;
public:
	CgrZBuffer(void);
	~CgrZBuffer(void);

	void Disable();
	void Enable ();
	void EnableMode (DWORD Mode=ZLEQUAL);
	void Clear  ();
};
