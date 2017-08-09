#ifndef _AI_PLUGIN
#define _AI_PLUGIN

class CAIPlugIn	  : public BlopBaseObject
{
public:
	CAIPlugIn(void);
	~CAIPlugIn(void);

	bool  Start();
	void  Stop ();
	DWORD  Tick (float Time);
    DWORD OnMessage(const CBlopMessage &Message);
};

#endif
