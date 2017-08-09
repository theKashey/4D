#include "stdafx.h"
#include "parser.h"





/*---------------------------------------------------------------------------
    Ìועמהû:
---------------------------------------------------------------------------*/

CParser::CParser()
{
	file=0;
	buf_end=buffer=buffer_start=NULL;
	MySlot=NULL;
	scriptline=0;
	SpaceStop=true;
	ScriptLock=true;
	LockLine=0;
}
CParser::~CParser()
{
 StopParseFile();
}

void CParser::StartParseFile(LPCSTR name)
{
	file = bOpenFile(name,"r",true);
	DWORD Len=bFileSize(file);
	//MySlot=GetGlobalDataPool()->Alloc(Len);
	//buffer=(LPSTR)MySlot->GetAddr();
	buffer_start=new char[Len];
	buffer=buffer_start;
	buf_end=buffer+Len;
	bReadFile((LPVOID)buffer,Len,1,file);
	bCloseFile(file);
	file=0;
	scriptline=0;
}

void CParser::StartParseBuffer(LPCSTR Buffer, const INT size)
{
	//buff
	buffer    =Buffer;
	buf_end   =Buffer+size;
	scriptline=0;
}

void CParser::StartParseString(LPCSTR string)
{
	buffer =string;
	buf_end=string+Istrlen(string);
	scriptline=0;
}

void CParser::StopParseFile(void)
{
	if(file  ) bCloseFile(file);
	if(MySlot) delete MySlot;
	if(buffer_start)delete buffer_start;
	file  =0;
	MySlot=0;
}


bool CParser::GetTokenToLineEnd()
{
	//GString S;
	DWORD sp=SpaceStop;
	ScriptLock=true;
	SpaceStop=false;
	if(!GetToken(false))
	{
	 if(*buffer==';')buffer++;
	}
	SpaceStop=sp;
	//while(GetToken(false))
	//	S=S+" "+token;
	//strcpy(token,S.c_str());
	return true;
}

// if crossline is false, GetToken returns false when a line break is found
// GetToken(true) returns 0 only if end of file
// GetToken(false) returns 0 if line break or end of file
bool CParser::GetToken(bool crossline)
{
	if(!ScriptLock && !crossline && LockLine!=scriptline) return NULL;
	ScriptLock=crossline;
	LockLine=scriptline;
#define CHLINE {if(*buffer=='\n')scriptline++;}
	char    *token_p;
	if(!buffer) return 0;
	if(buffer>=buf_end) return 0;
	*token=0;	// init to zero
// skip space
skipspace:
    while(1)
	{
	 while(*buffer<=32 && buffer<buf_end)buffer++;
	 if(buffer>=buf_end)return 0;
	 bool N=(*buffer=='\n' || *buffer=='\r');
	 if(N){CHLINE;if(!crossline)return false;}
	 bool E=(*buffer==';');
	 if(E)               if(!crossline)return false;
	 if(!E && !N) break;
	 buffer++;
	}
	if (buffer >= buf_end) return 0;

	switch(*buffer)
	{
     case '/':  //comments
		      if(buffer[1]=='*')
			  {
				  while((buffer[0]!='*' || buffer[1]!='/') && buffer<buf_end)buffer++;
				  buffer+=2;
			      if(!crossline) return 0;
		          goto skipspace;
				  break;
			  }else
		      if(buffer[1]!='/')
				  break;
	 case '#': //comments
			  while(*buffer!='\n' && buffer<buf_end)buffer++;
			  CHLINE;
			  while(1)if(*buffer=='\r' || *buffer=='\n'){CHLINE;buffer++;}else break;
			  if(!crossline) return 0;
		      goto skipspace;
	
	}

	//
	// copy token
	//
	token_p = token;
	switch(*buffer)
	{
	case '"': {		// quoted token
	   	       buffer++;
		        while (*buffer != '"')
				{
			     *token_p++ = *buffer++;
			     if (buffer == buf_end) break;
			     if (token_p == &token[MAXTOKEN]) { ASSERT(FALSE); }
		      }
		      buffer++;
	          }break;
	case '{':  // regular token
	case '}':  *token_p++=*buffer++;
			    break;
	default:
		while ( ((*buffer > 32 && SpaceStop) || !SpaceStop) && *buffer != '\n' && *buffer != '\r' && *buffer != ';' && *buffer !='{' && *buffer !='}')
		{
			*token_p++ = *buffer++;
			if (buffer == buf_end) break;
			if (token_p == &token[MAXTOKEN]){ASSERT(FALSE);};
		}
	}
	*token_p = 0;
    while(1)if(*buffer=='\r' || *buffer=='\n'){CHLINE;buffer++;}else break;
	return 1;
}

GString sNULL;

GString  CParser::GetParameter()
{
	if(!CParser::GetToken(false)){return sNULL;}
	return CParser::token;
}

