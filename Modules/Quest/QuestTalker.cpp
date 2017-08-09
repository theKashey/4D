#include "stdafx.h"
#include "QuestTalker.h"
/*
using namespace SetTalker;

CQuestTalker QuestTalker;

CQuestTalker::CQuestTalker()
{
}

void CQuestTalker::Insert(creature *Src,LPCSTR Format,...)
{
	CTalkScene Scene;
	va_list arg;
    va_start(arg,Format);
    vsprintf(MsgBuf,Format,arg);
    va_end(arg);
	DecodeString(MsgBuf,Scene);

	out->Insert("%s says:%s\n",Src->GetName(),Scene.GetText());
	if(Scene.Sound)
	{
	  Sound::SetSoundPos(Scene.Sound,Src->GetOrigin());
	  Sound::PlayOnce(Scene.Sound);
	}
}
*/

