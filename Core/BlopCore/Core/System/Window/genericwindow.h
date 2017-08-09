#pragma once

#include "../../Input/InputFeedBack.h"

class  CGenericWindow : public BaseObject
{
protected:
	BlopWindowStartUp Options;
public:
	CInput   *Input;
	CGenericWindow(BlopWindowStartUp &StartUpInfo,BlopBaseObject *_Driver);
	~CGenericWindow(void);

};
