#pragma once

#define MAXTOKEN			1024

class DLLTYPE CParser {
public:
	CParser();
   ~CParser();
	void StartParseFile  (LPCSTR name);
	void StartParseBuffer(LPCSTR buffer, const INT size);
	void StartParseString(LPCSTR string);
	void StopParseFile   (void);
	bool GetToken        (bool crossline);
	bool GetTokenToLineEnd();

	char token[MAXTOKEN];				// buffer for tokens
	int  scriptline;
	LPCSTR EndOffset(){return buffer;}

	GString  GetParameter();

protected:
	bool       ScriptLock;
	int        LockLine;
	DWORD      SpaceStop;
	DWORD      file;
	SlotPoint *MySlot;
   	LPCSTR     buffer;
	LPCSTR     buf_end;
	LPSTR      buffer_start;
};

/*---------------------------------------------------------------------------
    Определители:
---------------------------------------------------------------------------*/
#define IS_OPENBRACKET(a)		(((a)[0]=='{') && ((a)[1]==0))
#define IS_CLOSEBRACKET(a)		(((a)[0]=='}') && ((a)[1]==0))


