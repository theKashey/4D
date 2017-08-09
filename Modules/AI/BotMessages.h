#ifndef _BOT_MESSAGES_1_H
#define _BOT_MESSAGES_1_H

#pragma once

//BOT CMD CLASS MOVE

namespace BOTCMD
{

#define BAD_WAYPOINT 0

struct CMD_BASE
{
public:
    virtual void PrepareData(){};
    virtual void FreeData(){};
    virtual DWORD GetLen(){return sizeof(*this);}
    virtual int   GetExLen(){return -1;}
    virtual LPVOID GetData(){return (LPVOID)this;}
};

//#define BOT_CMD_MOVE_TO             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,1 ) // бот идет до вейпоинта
struct CMD_MOVE_TO:public CMD_BASE
{
private:
	DWORD  WayPoint;
	DWORD  Speed;
public:
	static DWORD GetID       ()                   {return BOT_CMD_MOVE_TO;} 

	CMD_MOVE_TO              ()                   {SetWayPoint(BAD_WAYPOINT);}
	CMD_MOVE_TO              (const DWORD WP,const DWORD Speed=10)     {SetWayPoint(WP,Speed);}
	void        SetWayPoint  (const DWORD WP,const DWORD _Speed=10)     {WayPoint=WP;Speed=_Speed;}
	DWORD       GetWayPoint  ()                   {return WayPoint;}
	DWORD       GetSpeed     ()                   {return Speed;}
												  
};

//#define BOT_CMD_MOVE_DIR            MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,2 ) // бот идет в напревлении
struct CMD_MOVE_DIR:public CMD_BASE
{
private:
	bVector3f Dirrection;
    WORD      ForseDir;
    BYTE      StopVal;
public:
	static DWORD GetID       ()                   {return BOT_CMD_MOVE_DIR;} 

	CMD_MOVE_DIR             ()                   {Dirrection.Set(0,0,0);ForseDir=0;StopVal=0;}
	CMD_MOVE_DIR             (const bVector3f &WP){SetDirrection(WP);ForseDir=0;StopVal=0;}
	void        SetDirrection(const bVector3f &WP){Dirrection=WP;}
	const bVector3f& GetDirrection()                   {return Dirrection;}
    void  Forse              (const WORD type    ){ForseDir=type;}
    WORD  GetForse           ()                   {return ForseDir;}
    void  SetStop            ()                   {StopVal=1;}
    bool  IsStoped           ()                   {return StopVal==1;}
};

//#define BOT_CMD_CHANGE_MOVE_TYPE    MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,3 ) // бот меняет тип движения(быстрый бег,бег,хотьба,медленная хотьба,присесть,лежать)
struct CMD_CHANGE_MOVE_TYPE:public CMD_BASE
{
private:
	DWORD Type;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_CHANGE_MOVE_TYPE;} 

	CMD_CHANGE_MOVE_TYPE     ()                   {SetMoveType(BAD_WAYPOINT);}
	CMD_CHANGE_MOVE_TYPE     (const DWORD WP)     {SetMoveType(WP);}
	void        SetMoveType  (const DWORD WP)     {Type=WP;}
	DWORD       GetMoveType  ()                   {return Type;}
};

//#define BOT_CMD_DO_JUMP             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,4 ) // бот прыгает
struct CMD_DO_JUMP:public CMD_BASE
{
private:
	bVector3f Dirrection;
public:
	static DWORD GetID       ()                   {return BOT_CMD_DO_JUMP;} 

	CMD_DO_JUMP              ()                   {Dirrection.Set(0,1,0);}
	CMD_DO_JUMP              (const bVector3f &WP){SetDirrection(WP);}
	void        SetDirrection(const bVector3f &WP){Dirrection=WP;}
	bVector3f   GetDirrection()                   {return Dirrection;}
};

//#define BOT_CMD_GET_TO              MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,5 ) // бот пробует забраться кудато
struct CMD_GET_TO:public CMD_BASE
{
private:
	DWORD WayPoint;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_GET_TO;} 

	CMD_GET_TO               ()                   {SetWayPoint(BAD_WAYPOINT);}
	CMD_GET_TO               (const DWORD WP)     {SetWayPoint(WP);}
	void        SetWayPoint  (const DWORD WP)     {WayPoint=WP;}
	DWORD       GetWayPoint  ()                   {return WayPoint;}
};

//#define BOT_CMD_LOOK_TO             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,6 ) // бот смотрит в указаном направлении
struct CMD_LOOK_TO:public CMD_BASE
{
private:
	bVector3f Dirrection;
    float     Speed;
public:
	static DWORD GetID       ()                   {return BOT_CMD_LOOK_TO;} 

	CMD_LOOK_TO              ()                   {Dirrection.Set(0,0,0);Speed=1;}
	CMD_LOOK_TO              (const bVector3f &WP){SetDirrection(WP);Speed=1;}
	void        SetDirrection(const bVector3f &WP){Dirrection=WP;}
	const bVector3f&    GetDirrection()                   {return Dirrection;}
    void  SetSpeed           (const float sp     ){Speed=sp;}
    float GetSpeed           ()                   {return Speed;}
};

//#define BOT_CMD_LOOK_AT             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,7 ) // бот смотрит на обьект
struct CMD_LOOK_AT:public CMD_BASE
{
private:
	DWORD WayPoint;
    float Speed;
    char  Name[256];
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_LOOK_AT;} 

	CMD_LOOK_AT              ()                   {SetWayPoint(BAD_WAYPOINT);Name[0]=0;Speed=1;}
	CMD_LOOK_AT              (const DWORD WP)     {SetWayPoint(WP);Name[0]=0;Speed=1;}
	void        SetWayPoint  (const DWORD WP)     {WayPoint=WP;}
    void        SetWayTarget (LPCSTR WP)          {strcpy_s(Name,sizeof(Name),WP);}
	DWORD       GetWayPoint  ()                   {return WayPoint;}
    LPCSTR      GetWayTarget ()                   {return Name;}
    void  SetSpeed           (const float sp     ){Speed=sp;}
    float GetSpeed           ()                   {return Speed;}
   int GetExLen(){int Size=sizeof(*this);Size-=sizeof(Name);Size+=strlen(Name)+1;return Size;}
};

//#define BOT_CMD_ROTATE              MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,8 ) // бот поворачивается в указаном направлении
struct CMD_ROTATE:public CMD_BASE
{
private:
	bVector3f Dirrection;
	float     ForceAngles;
public:
	static DWORD GetID       ()                   {return BOT_CMD_ROTATE;} 
	
	CMD_ROTATE               ()                   {Dirrection.Set(0,0,0);ForceAngles=1;}
	CMD_ROTATE               (const bVector3f &WP,float _ForceAngles){SetDirrection(WP,_ForceAngles);}
	void        SetDirrection(const bVector3f &WP,float _ForceAngles){Dirrection=WP;ForceAngles=_ForceAngles;}
	bVector3f   GetDirrection()                   {return Dirrection;}
	float       IsAnglesForced()                  {return ForceAngles;}
};

//**************************************************************************
//**************************************************************************
//BOT CMD CLASS ATTACK
//**************************************************************************

//#define BOT_CMD_FIRE                MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,1 ) // бот стреляет
struct CMD_FIRE:public CMD_BASE
{
private:
	DWORD Action;
    int Rounds;
public:											  
	static DWORD GetID      ()                   {return BOT_CMD_FIRE;} 

	CMD_FIRE                ()                   {SetType(0);SetNumRaunds(-1);}
	CMD_FIRE                (DWORD _Type        ){SetType(_Type);}
	void        SetType     (DWORD      _Type=0 ){Action=_Type;}
    DWORD       GetType     ()                   {return Action;}
    void        SetNumRaunds(const DWORD WP)     {Rounds=WP;}
	int         GetNumRaunds()                   {return Rounds;}
};


//#define BOT_CMD_FIRE_TO             MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,2 ) // бот стреляет по вейпоинту
struct CMD_FIRE_TO:public CMD_BASE
{
private:
	DWORD Rounds;
	DWORD WayPoint;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_FIRE;} 

	CMD_FIRE_TO              ()                   {SetNumRaunds(BAD_WAYPOINT,BAD_WAYPOINT);}
	CMD_FIRE_TO              (const DWORD WP,const DWORD WP2)     {SetNumRaunds(WP,WP2);}
	void        SetNumRaunds (const DWORD WP,const DWORD WP2)     {Rounds=WP;WayPoint=WP2;}
	DWORD       GetNumRaunds ()                   {return Rounds;}
	DWORD       GetWayPoint  ()                   {return WayPoint;}
};

//#define BOT_CMD_FIRE_TARGET         MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,3 ) // бот стреляет по цели
struct FIRE_TARGET:public CMD_FIRE
{
private:
	DWORD Rounds;
	DWORD WayPoint;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_FIRE_TARGET;} 
};

//#define BOT_CMD_AIM                 MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,4 ) // бот берет цель на прицел
struct CMD_AIM:public CMD_BASE
{
private:
	DWORD WayPoint;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_AIM;} 

	CMD_AIM                  ()                   {SetWayPoint(BAD_WAYPOINT);}
	CMD_AIM                  (const DWORD WP)     {SetWayPoint(WP);}
	void        SetWayPoint  (const DWORD WP)     {WayPoint=WP;}
	DWORD       GetWayPoint  ()                   {return WayPoint;}
};

//#define BOT_CMD_CHANGE_WEAPON       MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,5 ) // бот меняет оружие
struct CMD_CHANGE_WEAPON:public CMD_BASE
{
private:
	DWORD WepID;
	DWORD Slot;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_CHANGE_WEAPON;} 

	CMD_CHANGE_WEAPON        ()                   {SetWeapon(BAD_WAYPOINT,BAD_WAYPOINT);}
	CMD_CHANGE_WEAPON        (const DWORD WP,const DWORD  Slot=0)     {SetWeapon(WP,Slot);}
	void        SetWeapon    (const DWORD WP,const DWORD _Slot=0)     {WepID=WP;Slot=_Slot;}
	DWORD       GetWeapon    ()                   {return WepID;}
	DWORD       GetSlot      ()					  {return Slot;}
};

//#define BOT_CMD_RELOAD_WEAPON       MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,6 ) // бот перезяряжает оружие
struct CMD_RELOAD_WEAPON:public CMD_BASE
{
private:
	DWORD Slot;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_RELOAD_WEAPON;} 

	CMD_RELOAD_WEAPON        ()                   {SetSlot(BAD_WAYPOINT);}
	CMD_RELOAD_WEAPON        (const DWORD  Slot   ){SetSlot(Slot);}
	void        SetSlot      (const DWORD _Slot=0){Slot=_Slot;}
	DWORD       GetSlot      ()					  {return Slot;}
};

//#define BOT_CMD_CHANGE_AIM_TYPE     MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,7 ) // бот тип наведения
struct CMD_CHANGE_AIM_TYPE:public CMD_BASE
{
private:
	DWORD Type;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_CHANGE_AIM_TYPE;} 

	CMD_CHANGE_AIM_TYPE      ()                   {SetType(BAD_WAYPOINT);}
	CMD_CHANGE_AIM_TYPE      (const DWORD  Type   ){SetType(Type);}
	void        SetType      (const DWORD _Type=0){Type=_Type;}
	DWORD       GetType      ()					  {return Type;}
};

//#define BOT_CMD_DROP_WEAPON         MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,8 ) // бот бросает оружие
struct CMD_DROP_WEAPON:public CMD_BASE
{
private:
	DWORD WepID;
	DWORD Slot;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_DROP_WEAPON;} 

	CMD_DROP_WEAPON          ()                   {SetWeapon(BAD_WAYPOINT,BAD_WAYPOINT);}
	CMD_DROP_WEAPON          (const DWORD WP,const DWORD  Slot=0)     {SetWeapon(WP,Slot);}
	void        SetWeapon    (const DWORD WP,const DWORD _Slot=0)     {WepID=WP;Slot=_Slot;}
	DWORD       GetWeapon    ()                   {return WepID;}
	DWORD       GetSlot      ()					  {return Slot;}
};

//**************************************************************************
//**************************************************************************
//BOT CMD ACTION
//**************************************************************************

//#define BOT_CMD_KICK                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,1 ) // толкнуть
struct CMD_KICK:public CMD_BASE
{
private:
	DWORD Target;
	DWORD Type;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_KICK;} 

	CMD_KICK                 ()                   {SetTarget(BAD_WAYPOINT,BAD_WAYPOINT);}
	CMD_KICK                 (const DWORD WP,const DWORD  Slot=0)     {SetTarget(WP,Slot);}
	void        SetTarget    (const DWORD WP,const DWORD _Slot=0)     {Target=WP;Type=_Slot;}
	DWORD       GetTarget    ()                   {return Target;}
	DWORD       GetType      ()					  {return Type;}
};

//#define BOT_CMD_PUSH                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,2 ) // нажать
struct CMD_PUSH:public CMD_BASE
{
private:
	DWORD Target;
	DWORD Type;
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_PUSH;} 

	CMD_PUSH                 ()                   {SetTarget(BAD_WAYPOINT,BAD_WAYPOINT);}
	CMD_PUSH                 (const DWORD WP,const DWORD  Slot=0)     {SetTarget(WP,Slot);}
	void        SetTarget    (const DWORD WP,const DWORD _Slot=0)     {Target=WP;Type=_Slot;}
	DWORD       GetTarget    ()                   {return Target;}
	DWORD       GetType      ()					  {return Type;}
};

//#define BOT_CMD_GIVE                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,3 ) // дать
//#define BOT_CMD_TAKE                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,4 ) // взять
//#define BOT_CMD_USE                 MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,5 ) // использовать
//#define BOT_CMD_USE_ON              MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,6 ) // использовать на
//#define BOT_CMD_UNUSE               MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,7 ) // прекратить использование
//#define BOT_CMD_DO_ANIMATION        MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,8 ) // проигрывание анимации


//#define BOT_CMD_SAY                 MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,9 ) // сказать(текст,звук)
struct CMD_SAY:public CMD_BASE
{
private:
	char Text[0xFF];
public:											  
	static DWORD GetID       ()                   {return BOT_CMD_SAY;} 

	CMD_SAY                  ()                   {SetText("");}
	CMD_SAY                  (LPCSTR Text)        {SetText(Text);}
	void SetText(LPCSTR t){strcpy_s(Text,sizeof(Text),t);Text[MIN((size_t)0xFF,strlen(t))]=0;}
	LPCSTR GetText(){return Text;}
	int GetExLen(){int Size=sizeof(*this);Size-=sizeof(Text);Size+=strlen(Text)+1;return Size;}
};

//#define BOT_CMD_KILL                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,10) // убить
//#define BOT_CMD_HEAL                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,11) // лечить
//#define BOT_CMD_SUPPORT             MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,12) // помочь
//#define BOT_CMD_DEVASTATE           MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,13) // уничтожить
//#define BOT_CMD_COVERTHAT           MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,14) // осмотреться



//#define BOT_CMD_ALLERT_TRIG         MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,1 ) // вызвать тригер

struct CMD_ALLERT_TRIG:public CMD_BASE
{
private:
	DWORD Trig;
public:											  
	static DWORD GetID      ()                   {return BOT_CMD_ALLERT_TRIG;} 

	CMD_ALLERT_TRIG         ()                   {SetTrigger(BAD_WAYPOINT);}
	CMD_ALLERT_TRIG         (const DWORD  Type  ){SetTrigger(Type);}
	void        SetTrigger  (const DWORD _Type=0){Trig=_Type;}
	DWORD       GetTrigger  ()					 {return Trig;}
};



//#define BOT_CMD_WAIT                MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,2 ) // подождать

struct CMD_WAIT:public CMD_BASE
{
private:
	DWORD Time;
    int   ret;
public:											  
	static DWORD GetID      ()                   {return BOT_CMD_WAIT;} 

	CMD_WAIT                ()                   {SetTime(BAD_WAYPOINT);SetReturn(-1);}
	CMD_WAIT                (const DWORD  Type  ){SetTime(Type);SetReturn(-1);}
	void        SetTime     (const DWORD _Type=0){Time=_Type;}
	DWORD       GetTime     ()					 {return Time;}
    void        SetReturn   (int _ret           ){ret=_ret;}
    int         GetReturn   ()                   {return ret;}
};

//#define BOT_CMD_WAITTRIG			MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,5 ) // ждем завершения тригера

struct CMD_WAIT_TRIG:public CMD_BASE
{
private:
	DWORD Trig;
public:											  
	static DWORD GetID      ()                   {return BOT_CMD_WAIT_TRIG;} 

	CMD_WAIT_TRIG           ()                   {SetTrigger(BAD_WAYPOINT);}
	CMD_WAIT_TRIG           (const DWORD  Type  ){SetTrigger(Type);}
	void        SetTrigger  (const DWORD _Type=0){Trig=_Type;}
	DWORD       GetTrigger  ()					 {return Trig;}
};

//#define BOT_CMD_CALLTRIG			MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,6 ) // блокируюший вызов тригера

struct CMD_CALL_TRIG:public CMD_BASE
{
private:
	DWORD Trig;
public:											  
	static DWORD GetID      ()                   {return BOT_CMD_CALL_TRIG;} 

	CMD_CALL_TRIG           ()                   {SetTrigger(BAD_WAYPOINT);}
	CMD_CALL_TRIG           (const DWORD  Type  ){SetTrigger(Type);}
	void        SetTrigger  (const DWORD _Type=0){Trig=_Type;}
	DWORD       GetTrigger  ()					 {return Trig;}
};



struct CMD_USEBOT:public CMD_BASE
{
private:
    bool Wait;
	char Name[0xFF];
public:											  
	static DWORD GetID      ()                   {return BOT_CMD_USEBOT;} 

	CMD_USEBOT              ()                   {SetName("");SetWait(0);}
	CMD_USEBOT              (LPCSTR _Name       ){SetName(_Name);}
	void        SetName     (LPCSTR      _Name=0){strcpy_s(Name,sizeof(Name),_Name);}
    void        SetWait     (bool _Wait =0      ){Wait=_Wait;}
	LPCSTR      GetName     ()					 {return Name;}
    bool        GetWait     ()                   {return Wait;}
    int GetExLen(){int Size=sizeof(*this);Size-=sizeof(Name);Size+=strlen(Name)+1;return Size;}
};

struct CMD_EXEC_CONSOLE:public CMD_BASE
{
private:
	char Text[0xFF*4];
public:											  
	static DWORD GetID      ()                   {return BOT_CMD_EXEC_CONSOLE;} 
	CMD_EXEC_CONSOLE              ()             {SetText("");}
	CMD_EXEC_CONSOLE              (LPCSTR _Name ){SetText(_Name);}
	void        SetText     (LPCSTR      _Text=0){strcpy_s(Text,sizeof(Text),_Text);}
	LPCSTR      GetText     ()					 {return Text;}
    int GetExLen(){int Size=sizeof(*this);Size-=sizeof(Text);Size+=strlen(Text)+1;return Size;}
};


};
#endif
