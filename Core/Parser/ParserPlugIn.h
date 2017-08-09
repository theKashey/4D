#ifndef _PARSER_PLUGIN
#define _PARSER_PLUGIN

class CParserPlugIn : public BaseObject
{
public:
	CParserPlugIn(void);
	~CParserPlugIn(void);

	bool Start();
};

PLoadObjectList DLLTYPE GetParserStoredClasses();

#endif
