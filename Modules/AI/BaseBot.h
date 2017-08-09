#ifndef _BOTBASE_AI
#define _BOTBASE_AI

#include "ProjectDividors.h"

namespace AI
{


#define BTI_BOT                  0x00   // item is a bot
#define BTI_WAYPOINT             0x01   // item is a waypoint
#define BTI_ITEM                 0x02   // item is item
#define BTI_LOCATION             0x03   // item is location
/*
#define BTI_CONTROLED_ITEM       0x04   // item is controlled
#define BTI_PLAYER               ((1)<<05)   // item is a player
#define BTI_PASSIVE_ITEM         ((1)<<06)   // item is passive
#define BTI_ACTIVE_ITEM          ((1)<<07)   // item is active
#define BTI_SLEEPER_ITEM         ((1)<<08)   // item is sleeper
#define BTI_UNION                ((1)<<09)   // item is union
#define BTI_GOAL                 ((1)<<10)   // item is goal
*/

#define MOVE_FAIL                 0      // движение не возможно
#define MOVE_OK                   1      // движение возможно
#define MOVE_WAIT                 1<<1   // абожди(до след тика)
#define MOVE_DONE                 1<<2   // хатово!

typedef CBSPDividorLookUpArray<char,int> _AICMDBSP;
void DLLTYPE AddCMD(LPCSTR Name,DWORD ID);

#define MAKE_BOT_CMD(a,b)                ((a<<16)|(b))
#define GET_BOT_CMD_CMD(a)               ((a & 0xFF))
#define GET_BOT_CMD_CLASS(a)             ((a )>>16)

#define BOT_CMDCLASS_MOVE           1
#define BOT_CMDCLASS_ATTACK         2
#define BOT_CMDCLASS_ACTION         3
#define BOT_CMDCLASS_LUA            4
#define BOT_CMDCLASS_SCRIPT_CTRL    5

#define BOT_CMD_LUA                 MAKE_BOT_CMD(BOT_CMDCLASS_LUA   ,1 ); // команда LUA

//BOT CMD CLASS MOVE


#define BOT_CMD_MOVE_TO             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,1 ) // бот идет до вейпоинта
#define BOT_CMD_MOVE_DIR            MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,2 ) // бот идет в напревлении
#define BOT_CMD_CHANGE_MOVE_TYPE    MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,3 ) // бот меняет тип движения(быстрый бег,бег,хотьба,медленная хотьба,присесть,лежать)
#define BOT_CMD_DO_JUMP             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,4 ) // бот прыгает
#define BOT_CMD_GET_TO              MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,5 ) // бот пробует забраться кудато
#define BOT_CMD_LOOK_TO             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,6 ) // бот смотрит в указаном направлении
#define BOT_CMD_LOOK_AT             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,7 ) // бот смотрит на обьект
#define BOT_CMD_ROTATE              MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,8 ) // бот поворачивается в указаном направлении

//BOT CMD CLASS ATTACK

#define BOT_CMD_FIRE                MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,1 ) // бот стреляет
#define BOT_CMD_FIRE_TO             MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,2 ) // бот стреляет по вейпоинту
#define BOT_CMD_FIRE_TARGET         MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,3 ) // бот стреляет по цели
#define BOT_CMD_AIM                 MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,4 ) // бот берет цель на прицел
#define BOT_CMD_CHANGE_WEAPON       MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,5 ) // бот меняет оружие
#define BOT_CMD_RELOAD_WEAPON       MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,6 ) // бот перезяряжает оружие
#define BOT_CMD_CHANGE_AIM_TYPE     MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,7 ) // бот тип наведения
#define BOT_CMD_DROP_WEAPON         MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,8 ) // бот бросает оружие

//BOT CMD ACTION

#define BOT_CMD_KICK                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,1 ) // толкнуть
#define BOT_CMD_PUSH                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,2 ) // нажать
#define BOT_CMD_GIVE                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,3 ) // дать
#define BOT_CMD_TAKE                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,4 ) // взять
#define BOT_CMD_USE                 MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,5 ) // использовать
#define BOT_CMD_USE_ON              MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,6 ) // использовать на
#define BOT_CMD_UNUSE               MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,7 ) // прекратить использование
#define BOT_CMD_DO_ANIMATION        MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,8 ) // проигрывание анимации
#define BOT_CMD_SAY                 MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,9 ) // сказать(текст,звук)
#define BOT_CMD_KILL                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,10) // убить
#define BOT_CMD_HEAL                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,11) // лечить
#define BOT_CMD_SUPPORT             MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,12) // помочь
#define BOT_CMD_DEVASTATE           MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,13) // уничтожить
#define BOT_CMD_COVERTHAT           MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,14) // осмотреться

//BOT CMD SCRIPT CONTROL

//BOT_CMDCLASS_SCRIPT_CTRL

#define BOT_CMD_ALLERT_TRIG         MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,1 ) // вызвать тригер
#define BOT_CMD_WAIT                MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,2 ) // подождать
#define BOT_CMD_MAKEMARKER          MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,3 ) // создать маркер
#define BOT_CMD_USEBOT              MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,4 ) // создать маркер
#define BOT_CMD_WAIT_TRIG			MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,5 ) // ждем завершения тригера
#define BOT_CMD_CALL_TRIG			MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,6 ) // блокируюший вызов тригера
#define BOT_CMD_PAUSE_TRIG          MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,7 ) // пауза тригера
#define BOT_CMD_RESUME_TRIG         MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,8 ) // продолжение тригера
#define BOT_CMD_DUP_TRIG            MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,9 ) // дуплицирование тригера
#define BOT_CMD_IN_ALLERT           MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,10) // тронуть тригер обьекта
#define BOT_CMD_EXEC_CMD            MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,11) // выполнить команду
#define BOT_CMD_EXEC_CONSOLE        MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,12) // выполнить команду консоли
#define BOT_CMD_MOD_VAR             MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,13) // изменить переменую
#define BOT_CMD_STOP_TRIG           MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,14) // стоп тригера

//типы целей для команды

#define CTI_UNKNOWN                 0x01   // цель не известна
#define BTT_TEAM                    0x01   // цель есть команда
#define BTT_BOTTYPE                 0x02   // цель есть тип бота
#define BTT_SINGLEBOT               0x03   // цель есть бот
#define BTT_COMMAND                 0x04   // цель есть группа ботов

struct CBotTeamType
{
	BYTE  Type;
	DWORD Target;
};

//типы командного взаимодейсвия
#define CTI_ENEMY                   0x01
#define CTI_DEADLY_ENEMY            0x02
#define CTI_NEUTRAL                 0x03
#define CTI_FRIEND                  0x04
#define CTI_GOOD_FRIEND             0x05
#define CTI_BAD_FRIEND              0x06
#define CTI_INVISIBLE               0x07

struct CBotTeam
{
  typedef SmartVector<CBotTeamType> TeamList;
  //список врагов
  TeamList   Enemy;
  //список вторичных врагов
  TeamList   SecondaryEnemy;
  //список союзников
  TeamList   Friends;
  //список нетралов
  TeamList   Neutals;

  BYTE AddEnemy         (const CBotTeamType &TT);
  BYTE AddSecondaryEnemy(const CBotTeamType &TT);
  BYTE AddFriend        (const CBotTeamType &TT);
  BYTE AddNeutal        (const CBotTeamType &TT);

  BYTE WhoIs (const CBotTeamType &T);

  BYTE Remove(const CBotTeamType &TT); 
};


//возможные тригеры

#define MAKE_BOT_TRIG(a,b)           (a<<16|b)

#define BOT_TRIGCLASS_HEAR          0x01
#define BOT_TRIGCLASS_LOOK          0x02
#define BOT_TRIGCLASS_COMMAND       0x03
#define BOT_TRIGCLASS_PHISIC        0x04
#define BOT_TRIGCLASS_DEATH         0x05
#define BOT_TRIGCLASS_MOVE          0x06

//тригеры слуха
//
#define BOT_TRIG_HEAR_SHOT          MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,1) // услышал выстрел
#define BOT_TRIG_HEAR_FRIEND        MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,2) // услышал друга
#define BOT_TRIG_HEAR_ENEMY         MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,3) // услышал врага
#define BOT_TRIG_HEAR_NEUTAL        MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,4) // услышал нетрала
#define BOT_TRIG_HEAR_SOUND         MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,5) // услышал звук
#define BOT_TRIG_HEAR_BOOM          MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,6) // услышал взрыв

//тригеры зрения

#define BOT_TRIG_SEE_FRIEND         MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,1) // увидел друга
#define BOT_TRIG_SEE_ENEMY          MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,2) // увидел врага
#define BOT_TRIG_SEE_NEUTAL         MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,3) // увидел нетрала
#define BOT_TRIG_SEE_DEAD           MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,4) // увидел труп
#define BOT_TRIG_SEE_COMBAT         MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,5) // увидел бой
#define BOT_TRIG_SEE_DANGER         MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,6) // увидел опастность
#define BOT_TRIG_ON_AIM             MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,7) // бот на прицеле

//тригеры физики

#define BOT_TRIG_PH_WAVE            MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,1) // задело взрывом
#define BOT_TRIG_PH_SHOT            MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,2) // попала пуля
#define BOT_TRIG_PH_FALL            MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,3) // упал
#define BOT_TRIG_PH_KICK            MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,4) // ударили
#define BOT_TRIG_PH_DAMAGE          MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,5) // получил повреждения
#define BOT_TRIG_PH_LOOSEITEM       MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,6) // потерял предмет


//тригеры смерти

#define BOT_TRIG_DEATH_SELF         MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,1) // умер сам
#define BOT_TRIG_DEATH_PLAYER       MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,2) // умер игрок
#define BOT_TRIG_DEATH_ENEMY        MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,3) // умер враг
#define BOT_TRIG_DEATH_FRIEND       MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,4) // умер друг
#define BOT_TRIG_DEATH_NEUTAL       MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,5) // умер нетрал
#define BOT_TRIG_DEATH_COMMANDER    MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,6) // умер командир

//тригеры движения

#define BOT_TRIG_MOVE_CLEAR_PATH    MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,1) // уступи дорогу
#define BOT_TRIG_MOVE_NO_WAY        MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,2) // нету пути
#define BOT_TRIG_MOVE_NEW_WAY       MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,3) // новая дорога
#define BOT_TRIG_MOVE_LOOKED_AT     MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,4) // бот посмотрел на что-то
#define BOT_TRIG_MOVE_CMP           MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,5) // бот дошол кудато




struct CBotSquad
{
	//список ботов
	typedef SmartVector<DWORD> BotList;
	//имя командира
	DWORD                      Commander;
};

#define BOT_STAT_ALIVE                   1
#define BOT_STAT_DEAD                    2
#define BOT_STAT_ATTACKING               3
#define BOT_STAT_HIDING                  4 
#define BOT_STAT_SEARCHING               5
#define BOT_STAT_DEFENDING               6
#define BOT_STAT_KILLING                 7
#define BOT_STAT_TAKING_DAMAGE           8 


struct CBotDefTeam
{
	DWORD            MyID;
	DWORD            MyTeam;    //моя команда
	DWORD            MySquad;   //мой взвод
    DWORD            MyType;    //мой тип
	DWORD            MyStatus;  //мой статус(жив..мертв,активность)
	DWORD            MyMode;    //мой режим (патруль,охрана,атака,стоять,снайпер,определяемо)
	DWORD            MyGoal;    //моя цель
};

struct CBotDefLife
{
	bool             Valid;     //активно\используемо
	float            Current;   //текушее здоровье
	DWORD            Start;     //стартовое
	DWORD            Max;       //максимальное
	float            Bleeding;  //потеря крови(едениц\минута);
};

struct CBotDefAttack
{
	float            PLevel;           // приоритет этого оружия
	bool             SingleHanded;
	float            accuracy;         // точность
	float            tactics;          // тактика
	DWORD            AimMode;          // режим наведения
 	DWORD            CurrentWeapon;    // текушее оружие
	DWORD            CurrentAmo;       // колво патронов
	float            CurrentUsage;     // использование партронов
	float            ReloadTime;       // предположительное время перезарядки
	float            EmptyTime;        // предположительное время конца патронов
	float            Sniper;           // снайпер
	float            Camper;           // кемпер
	DWORD            MoveFactor;	   // модификатор движения
	float            MoveFactorParam;  // параметер модификатора
};

struct CBotDefMind
{
	float            Aggression;       // агрессивность
    float            Awarness;         // осмотрительность
	float            Reaction;         // реакция
	float            Stupidness;       // тупизм

	float            HearFactor;       // фактор слуха
	float            HearDistance;     // радиус слуха

	float            LookAngle[3];     // углы обзора
	float            LookFactor;       // фактор зрения
	float            LookDistance;     // дистанция зрения
};

struct CBotDefMovent
{
	bool             MoventPossible;
	bool             SilentMoventPossible;
	bool             RunningPossible;
	bool             CrouchingPosible;
	bool             RotationPossible;
	bool             JumpingPossible;
	bool             SwimingPossible;
};

struct CBotDefStat
{
	DWORD            IsAMarker;
	bVector3f        Position;
	bVector3f        Orientation;
	bVector3f        Speed;
	bVector3f        SizeBox;
};

struct CBotCMD
{
	DWORD  CMD;
	DWORD  Wait;
	WORD   Len;
	BYTE   Data[1];
};

class CBaseBot;

struct CBotScript
{
	bool    Enabled;
	GString ScriptName;
	DWORD   TimerStartedAt;
    GString CurrentToken;
    GString LastError;
	CBaseBot *ValidBot;
	static _AICMDBSP AICMDBSP;
    static _AICMDBSP TRIGCMDBSP;
    static _AICMDBSP TRIGPACKBSP;
	typedef SmartVector<CBotCMD*   > CommandList;
	typedef SmartVector<CBotScript*> CScriptList;
	static  CScriptList   ScriptList;
	struct  BotCMDPacked
	{
		DWORD    CMD;
		BYTE     Class;
		BYTE     Message;
	    CBotCMD* InData;
		CBaseBot *Bot;
		BotCMDPacked(CBotCMD* bIN,CBaseBot *InBot)
		{
			Bot      =InBot;
			InData   =bIN;
			CMD      =bIN->CMD;
			Class    =GET_BOT_CMD_CLASS(CMD);
            Message  =GET_BOT_CMD_CMD(CMD);
		}
	};
    //список команд
	CommandList           Commands;
	DWORD                 CurrentCommand;
	void AddCommand(CBotCMD *CMD);
	void Execute   (CBaseBot *Bot);

    static bool IsCommandComplette(const DWORD ID);
    void AI_ERROR(const CBaseBot *Bot);

    int DoMove  (const BotCMDPacked &In);
    int DoAttack(const BotCMDPacked &In);
    int DoAction(const BotCMDPacked &In);
    int DoLua   (const BotCMDPacked &In);
    int DoScript(const BotCMDPacked &In);

	bool inAddCMD(DWORD ID,DWORD Len,LPCVOID Data);

	template<class A>
	bool AddCMD(A &CMD)
	{
		CMD.PrepareData();
        DWORD RS=sizeof(A);
        if(RS<CMD.GetLen())RS=CMD.GetLen();
        int ES=CMD.GetExLen();
        if(ES!=-1)RS=ES;
		bool ret=inAddCMD(A::GetID(),RS,CMD.GetData());
        CMD.FreeData();
		return ret;
	}

	LPCSTR TryTranslateCMD(DWORD CMD,bool DoError=false);
    void   Reset();

    DWORD   Line(LPCSTR Line);
    void    AISHADERTOKENERROR(LPCSTR err,...);  
	void    MakeTrigger(LPCSTR Name);
	void    SetName    (LPCSTR Name);

	bool Activate(CBaseBot* B=NULL);
    bool Deactivate();
    static CBotScript* GetTriggerByName(LPCSTR Name);
    void  SetMyBot(CBaseBot* B){ ValidBot=B;}
	static void RunActive();
    static void StopAll  ();
	CBotScript()
	{
		Enabled=false;
		ValidBot=NULL;
		TimerStartedAt=0;
	}
};

typedef SmartVector<CBotScript*> _ScriptPack;

struct CBotTrigers
{   
    typedef SmartVector<_ScriptPack> _CmdPack;
    _CmdPack Pack;
};

void DLLTYPE InitTrigerStructure(CBotTrigers::_CmdPack &Pack);

class CIdeaManager
{
};

struct CBotParamBSP
{
	BYTE Type;
	WORD Offset;
	WORD Len;
};


#include "BotCMDs\NetCommands.h"

class CBotExecutor
{
public:
	virtual DWORD ExecCMD(CAINetCommand *CMD){return -1;}
};

class CBaseBot : public  BlopBaseObject
{
protected:
	typedef SmartVector<CBotDefAttack> AttackPack;
    CBotDefTeam      TeamDef;   //глобальные настройки
	CBotDefLife      Heal;      //жизнь
	CBotDefLife      Armor;     //броня
	AttackPack       Attack;    //пареметры боя для каждого оружия
    CBotDefMind      Mind;      //Параметры восприятия;
	CBotDefMovent    Movent;    //Параметры движения
	CBotDefStat      MyStat;    //Параметры пространства
public:
	CIdeaManager *Ideas;
	_ScriptPack     MyScripts;
    BYTE          __L1ParamsEnd;

	virtual void         CreateScriptParameters();

	//операции над ботом.
	//уровень 1
	DWORD        GetBotID   ();	 //получить ID
	DWORD        GetTeam    ();	 //получить команду
	DWORD        GetSquad   ();	 //получить взвод 
	DWORD        GetType    ();	 //получить тип
	DWORD        GetStatus  ();	 //получить статус

	DWORD        GetMyWeapon(int i); //получить оружие в слоте i

	float        GetHeal    ();	 // получить жизнь
	float        GetArmor   ();	 // получить броню

	const bVector3f &GetPosition         (); // получить позицию
	const bVector3f &GetVectorOrientation(); // вертор направления
	const bVector3f &GetAngleOrientation (); // углы направления

	const bVector3f &GetCurrentSpeed     (); // текушая скорость

	const bVector3f &GetSizeBox          (); // размеры


	bool IsMoventPossible   ();  // возможно ли движение
	bool IsSilentMovePosible();  // возможно ли скрытное движение
	bool IsRotationPossible ();  // возможен ли поворот


    float Tick              ();  // логический тик          

    void  WholeReset        ();  // полный сброс

	void  MakeMyMarker      (DWORD mID=-1);// установка маркера на себя
  
    DWORD OnMessage(const CBlopMessage &Message); // обработка сообщений


	//**************************************************************
	//**************************************************************
	// операции над пространством

	//маркеры
	DWORD GetMoventToMarker         (bVector3f &Dir,const DWORD Marker); // получить вектор направляния движения до маркера
	DWORD GetDirrectionToMarker     (bVector3f &Dir,const DWORD Marker); // получить вектор направляния на маркер
	DWORD GetShotDirrectionToMarker (bVector3f &Dir,const DWORD Marker); // получить вектор направляния стрельбы на маркер
	DWORD GetSilentMoventToMarker   (bVector3f &Dir,const DWORD Marker); // получить вектор направляния скрытного движения до маркера

	//**************************************************************
	//**************************************************************
	// операции над ботом 

	virtual bool MOVE_InDirrection          (const bVector3f &Dir,const WORD Force,const DWORD &Speed);// бот двигается в указаную сторону
	virtual bool MOVE_ChangeMode            (const DWORD &Mode   ,const DWORD &Speed);					// смена режима хотьбы
	virtual bool MOVE_DoJump                (const bVector3f &Dir,const DWORD &Speed);					// прыжок в направлении
    virtual bool MOVE_ToPoint               (const bVector3f &To,const DWORD &Speed);

	virtual bool ORI_LOOK                   (const bVector3f &Dir,const float &Speed);                 // посмотреть на
	virtual bool ORI_ROTATE                 (const bVector3f &Dir,const float &Speed,const float Force);// повернуться на
	virtual bool Say                        (LPCSTR S);

    //combat
    virtual void Fire                       (const DWORD Type,const int Rounds=-1);                    // выпускает в режиме Type заданое колво патронов

	virtual void Exec                       (LPCSTR String);
};

typedef CBotExecutor  * PBotExecutor;

DWORD          RegisterBot(CBaseBot *Bot,LPCSTR ID=NULL);
bool           SetBotPosition(DWORD ID,const bVector3f &Pos);
CBaseBot*      GetBotByName(LPCSTR BotName);
DWORD          RegisterCreatureT(CBotExecutor *Exc,LPCSTR Name);
DWORD          DLLTYPE RegisterCreature (CBotExecutor *Exc,LPCSTR Name);

typedef DWORD(*regEXTFunc)(CBotExecutor *Exc,LPCSTR Name);
void    DLLTYPE  RegisterCallBack_regExt(regEXTFunc Func);
PBotExecutor   DLLTYPE GetCreatureByNameT(LPCSTR Name);

DWORD         tRegisterWayPoint(DWORD ID,LPCSTR Name,const float* Origin,LPCSTR Target);
typedef DWORD(*regWAYPFunc)(DWORD ID,LPCSTR Name,const float* Origin,LPCSTR Target);
void DLLTYPE RegisterCallBack_regWayp(regWAYPFunc Func);
DWORD DLLTYPE  RegisterWayPoint(DWORD ID,LPCSTR Name,const float* Origin,LPCSTR Target);

typedef bool(*regbSTR)(LPCSTR n);
void DLLTYPE RegisterLoadAIScript(regbSTR f);
void DLLTYPE RegisterRunScript   (regbSTR f);
typedef DWORD(*regdSTRL)(LPCSTR b,DWORD l);
void DLLTYPE RegisterParseScript(  regdSTRL);

typedef DWORD(*regSTRV)(LPCSTR s,const bVector3f & V);
void DLLTYPE RegisterUpdateMarker( regSTRV);

bool  DLLTYPE     LoadAIScriptFile(LPCSTR Name);
bool  DLLTYPE     RunScript(LPCSTR Name);
DWORD DLLTYPE     ParseAIScript(LPCSTR Buf,DWORD Len);

bool  DLLTYPE     LoadAIScriptFileT(LPCSTR Name);
bool  DLLTYPE     RunScriptT(LPCSTR Name);
DWORD DLLTYPE     ParseAIScriptT(LPCSTR Buf,DWORD Len);

DWORD DLLTYPE     UpdateMarker(LPCSTR Name,const bVector3f &Pos);
DWORD DLLTYPE     fUpdateMarker(LPCSTR Name,const bVector3f &Pos);


void LoadBaseCMDS();

void DLLTYPE CreateAI();
void DLLTYPE RetouchScripts(LPCSTR Drive=NULL);
void DLLTYPE RetouchScript (LPCSTR Drive=NULL);

extern DWORD SYS_AI;

};
#endif // !defined(AFX_BASEBOT_H__A2F546EE_BA2F_4041_B1C0_917A297647F2__INCLUDED_)
