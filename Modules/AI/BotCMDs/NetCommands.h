#ifndef _NET_COMMANDS_H
#define _NET_COMMANDS_H

#define AINC_FLOAT   1
#define AINC_DWORD   2 
#define AINC_VECTOR  3
#define AINC_STRING  4

class CAINetCommand
{
public:
    DWORD     Command;
    WORD      Size;
    char      Data[1];
    void      SetCommand   (DWORD CMD){Command=CMD;Size=0;}
    void      PushDWORD    (DWORD DData){ memcpy(&Data[Size],&DData,sizeof(DData));Size+=4;}
    void      PushFLOAT    (float DData){ memcpy(&Data[Size],&DData,sizeof(DData));Size+=4;}
    void      PushBYTE     (BYTE  DData){ memcpy(&Data[Size],&DData,sizeof(DData));Size+=1;}
    void      PushVector   (const float* DData){ memcpy(&Data[Size],DData,sizeof(float)*3);Size+=12;}
    void      PushString   (LPCSTR DData){ strcpy_s(&Data[Size],0xFF*2,DData);Size+=(WORD)(strlen(DData)+1);}

};

class CAINetCommandReader
{
    CAINetCommand *CMD;
    DWORD          Pos;

public:
   CAINetCommandReader(CAINetCommand *cmd)
   {
       CMD=cmd;
       Pos=0;
   }

	DWORD     GetCommand   (){return CMD->Command;};
	DWORD     GetDataLen   (){return CMD->Size;};
	char*     GetData      (){return CMD->Data;};

    float     PopFloat ()
    {
        float ret=0;
        if(Pos>CMD->Size)return 0;
        else
        {
            ret= *((float*)&CMD->Data[Pos]);
            Pos+=4;
            return ret;
        }
    }
	DWORD     PopDword ()
    {
        if(Pos>CMD->Size)return 0;
        else
        {
         DWORD ret= *((DWORD*)&CMD->Data[Pos]);
         Pos+=4;
         return ret;
        }
    }
    BYTE     PopBYTE ()
    {
        if(Pos>CMD->Size)return 0;
        else
        {
         BYTE ret= *((BYTE*)&CMD->Data[Pos]);
         Pos+=1;
         return ret;
        }
    }
	bVector3f PopVector()
    {
        bVector3f ret;
        if(Pos>CMD->Size)return ret;
        else
        {
          ret=((const float*)&CMD->Data[Pos]);
          Pos+=12;
          return ret;
        }
    }
	LPCSTR    PopString()
    {
        if(Pos>CMD->Size)return 0;
        else
        {

          LPCSTR ret=  ((LPCSTR)&CMD->Data[Pos]);
          Pos+=(WORD)(strlen(ret)+1);
          return ret;
        }
    }

	int       TrackToBegin (){Pos=0;return Pos;};
	int       Track2Next   (DWORD Type){return 0;};
	int       TrackTo      (int To){Pos=To;return Pos;};
};

//commands

#endif