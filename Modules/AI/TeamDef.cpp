#include "stdafx.h"
#include "BaseBot.h"

using namespace AI;


bool IsTypeInTeam(DWORD T1,DWORD T2)
{
	return false;
}

bool IsBotInTeam(DWORD T1,DWORD T2)
{
	return false;
}

bool IsCommandInTeam(DWORD T1,DWORD T2)
{
    return false;
}

bool IsTypeInCommand(DWORD T1,DWORD T2)
{
    return false;
}

DWORD GetBotType(DWORD T1)
{
	return 0;
}

bool IsBotInCommand(DWORD T1,DWORD T2)
{
	return false;
}

DWORD GetCommandTeam(DWORD T1)
{
	return 0;
}

BYTE CBotTeam::AddEnemy         (const CBotTeamType &TT)
{
	Remove(TT);
	Enemy.push_back(TT);
	return Enemy.size();
}
BYTE CBotTeam::AddSecondaryEnemy(const CBotTeamType &TT)
{
	Remove(TT);
	SecondaryEnemy.push_back(TT);
	return SecondaryEnemy.size();
}
BYTE CBotTeam::AddFriend        (const CBotTeamType &TT)
{
	Remove(TT);
	Friends.push_back(TT);
	return Friends.size();
}
BYTE CBotTeam::AddNeutal        (const CBotTeamType &TT)
{
	Remove(TT);	
	Neutals.push_back(TT);
	return Neutals.size();
}

inline bool IsOne(const CBotTeamType &T1,const CBotTeamType &T2)
{
	switch(T1.Type)
	{
      case BTT_TEAM        :              // цель есть команда
		  	switch(T2.Type)
			{
              case BTT_TEAM        :              // цель есть команда
				  return T1.Target==T2.Target;
              case BTT_BOTTYPE     :              // цель есть тип бота
				  return IsTypeInTeam(T1.Target,T2.Target);
              case BTT_SINGLEBOT   :              // цель есть бот
				  return IsBotInTeam (T1.Target,T2.Target);
              case BTT_COMMAND     :              // цель есть группа ботов
				  return IsCommandInTeam(T1.Target,T2.Target);
			};
      case BTT_BOTTYPE     :              // цель есть тип бота
		  	switch(T2.Type)
			{
              case BTT_TEAM        :              // цель есть команда
				  return IsTypeInTeam(T2.Target,T1.Target);
              case BTT_BOTTYPE     :              // цель есть тип бота
				  return T1.Target==T2.Target;
              case BTT_SINGLEBOT   :              // цель есть бот
				  return GetBotType(T2.Target)==T1.Target;
              case BTT_COMMAND     :              // цель есть группа ботов
				  return IsTypeInCommand(T2.Target,T1.Target);
			};

      case BTT_SINGLEBOT   :              // цель есть бот
		  	switch(T2.Type)
			{
              case BTT_TEAM        :              // цель есть команда
				  return IsBotInTeam(T2.Target,T1.Target);
              case BTT_BOTTYPE     :              // цель есть тип бота
				  return GetBotType(T1.Target)==T2.Target;
              case BTT_SINGLEBOT   :              // цель есть бот
				  return T1.Target==T2.Target;
              case BTT_COMMAND     :              // цель есть группа ботов
				  return IsBotInCommand(T2.Target,T1.Target);
			};

      case BTT_COMMAND     :              // цель есть группа ботов
		  	switch(T2.Type)
			{
              case BTT_TEAM        :              // цель есть команда
				  return GetCommandTeam(T1.Target)==T2.Target;
              case BTT_BOTTYPE     :              // цель есть тип бота
				  return IsTypeInCommand(T1.Target,T2.Target);
              case BTT_SINGLEBOT   :              // цель есть бот
				  return IsBotInCommand(T1.Target,T2.Target);
              case BTT_COMMAND     :              // цель есть группа ботов
				  return T1.Target==T2.Target;
			};
	};
	return false;
}

BYTE CBotTeam::WhoIs(const CBotTeamType &TT)
{
	TeamList::iterator it;
	TeamList::iterator end;
	end=Enemy.end();it=Enemy.begin();
	for(;it!=end;++it) if(IsOne(*it,TT)) return CTI_DEADLY_ENEMY;
	end=SecondaryEnemy.end();it=SecondaryEnemy.begin();
	for(;it!=end;++it) if(IsOne(*it,TT)) return CTI_ENEMY;
	end=Friends.end();it=Friends.begin();
	for(;it!=end;++it) if(IsOne(*it,TT)) return CTI_FRIEND;
	end=Neutals.end();it=Neutals.begin();
	for(;it!=end;++it) if(IsOne(*it,TT)) return CTI_NEUTRAL;
	return CTI_UNKNOWN;
}

BYTE CBotTeam::Remove(const CBotTeamType &TT)
{
	TeamList::iterator it;
	TeamList::iterator end;
	end=Enemy.end();it=Enemy.begin();
	for(;it!=end;++it) if(IsOne(*it,TT)) return CTI_DEADLY_ENEMY;
	end=SecondaryEnemy.end();it=SecondaryEnemy.begin();
	for(;it!=end;++it) if(IsOne(*it,TT)) return CTI_ENEMY;
	end=Friends.end();it=Friends.begin();
	for(;it!=end;++it) if(IsOne(*it,TT)) return CTI_FRIEND;
	end=Neutals.end();it=Neutals.begin();
	for(;it!=end;++it) if(IsOne(*it,TT)) return CTI_NEUTRAL;
	return CTI_UNKNOWN;
}