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

#define MOVE_FAIL                 0      // �������� �� ��������
#define MOVE_OK                   1      // �������� ��������
#define MOVE_WAIT                 1<<1   // ������(�� ���� ����)
#define MOVE_DONE                 1<<2   // ������!

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

#define BOT_CMD_LUA                 MAKE_BOT_CMD(BOT_CMDCLASS_LUA   ,1 ); // ������� LUA

//BOT CMD CLASS MOVE


#define BOT_CMD_MOVE_TO             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,1 ) // ��� ���� �� ���������
#define BOT_CMD_MOVE_DIR            MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,2 ) // ��� ���� � �����������
#define BOT_CMD_CHANGE_MOVE_TYPE    MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,3 ) // ��� ������ ��� ��������(������� ���,���,������,��������� ������,��������,������)
#define BOT_CMD_DO_JUMP             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,4 ) // ��� �������
#define BOT_CMD_GET_TO              MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,5 ) // ��� ������� ��������� ������
#define BOT_CMD_LOOK_TO             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,6 ) // ��� ������� � �������� �����������
#define BOT_CMD_LOOK_AT             MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,7 ) // ��� ������� �� ������
#define BOT_CMD_ROTATE              MAKE_BOT_CMD(BOT_CMDCLASS_MOVE  ,8 ) // ��� �������������� � �������� �����������

//BOT CMD CLASS ATTACK

#define BOT_CMD_FIRE                MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,1 ) // ��� ��������
#define BOT_CMD_FIRE_TO             MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,2 ) // ��� �������� �� ���������
#define BOT_CMD_FIRE_TARGET         MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,3 ) // ��� �������� �� ����
#define BOT_CMD_AIM                 MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,4 ) // ��� ����� ���� �� ������
#define BOT_CMD_CHANGE_WEAPON       MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,5 ) // ��� ������ ������
#define BOT_CMD_RELOAD_WEAPON       MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,6 ) // ��� ������������ ������
#define BOT_CMD_CHANGE_AIM_TYPE     MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,7 ) // ��� ��� ���������
#define BOT_CMD_DROP_WEAPON         MAKE_BOT_CMD(BOT_CMDCLASS_ATTACK,8 ) // ��� ������� ������

//BOT CMD ACTION

#define BOT_CMD_KICK                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,1 ) // ��������
#define BOT_CMD_PUSH                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,2 ) // ������
#define BOT_CMD_GIVE                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,3 ) // ����
#define BOT_CMD_TAKE                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,4 ) // �����
#define BOT_CMD_USE                 MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,5 ) // ������������
#define BOT_CMD_USE_ON              MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,6 ) // ������������ ��
#define BOT_CMD_UNUSE               MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,7 ) // ���������� �������������
#define BOT_CMD_DO_ANIMATION        MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,8 ) // ������������ ��������
#define BOT_CMD_SAY                 MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,9 ) // �������(�����,����)
#define BOT_CMD_KILL                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,10) // �����
#define BOT_CMD_HEAL                MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,11) // ������
#define BOT_CMD_SUPPORT             MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,12) // ������
#define BOT_CMD_DEVASTATE           MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,13) // ����������
#define BOT_CMD_COVERTHAT           MAKE_BOT_CMD(BOT_CMDCLASS_ACTION,14) // �����������

//BOT CMD SCRIPT CONTROL

//BOT_CMDCLASS_SCRIPT_CTRL

#define BOT_CMD_ALLERT_TRIG         MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,1 ) // ������� ������
#define BOT_CMD_WAIT                MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,2 ) // ���������
#define BOT_CMD_MAKEMARKER          MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,3 ) // ������� ������
#define BOT_CMD_USEBOT              MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,4 ) // ������� ������
#define BOT_CMD_WAIT_TRIG			MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,5 ) // ���� ���������� �������
#define BOT_CMD_CALL_TRIG			MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,6 ) // ����������� ����� �������
#define BOT_CMD_PAUSE_TRIG          MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,7 ) // ����� �������
#define BOT_CMD_RESUME_TRIG         MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,8 ) // ����������� �������
#define BOT_CMD_DUP_TRIG            MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,9 ) // �������������� �������
#define BOT_CMD_IN_ALLERT           MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,10) // ������� ������ �������
#define BOT_CMD_EXEC_CMD            MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,11) // ��������� �������
#define BOT_CMD_EXEC_CONSOLE        MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,12) // ��������� ������� �������
#define BOT_CMD_MOD_VAR             MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,13) // �������� ���������
#define BOT_CMD_STOP_TRIG           MAKE_BOT_CMD(BOT_CMDCLASS_SCRIPT_CTRL,14) // ���� �������

//���� ����� ��� �������

#define CTI_UNKNOWN                 0x01   // ���� �� ��������
#define BTT_TEAM                    0x01   // ���� ���� �������
#define BTT_BOTTYPE                 0x02   // ���� ���� ��� ����
#define BTT_SINGLEBOT               0x03   // ���� ���� ���
#define BTT_COMMAND                 0x04   // ���� ���� ������ �����

struct CBotTeamType
{
	BYTE  Type;
	DWORD Target;
};

//���� ���������� �������������
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
  //������ ������
  TeamList   Enemy;
  //������ ��������� ������
  TeamList   SecondaryEnemy;
  //������ ���������
  TeamList   Friends;
  //������ ��������
  TeamList   Neutals;

  BYTE AddEnemy         (const CBotTeamType &TT);
  BYTE AddSecondaryEnemy(const CBotTeamType &TT);
  BYTE AddFriend        (const CBotTeamType &TT);
  BYTE AddNeutal        (const CBotTeamType &TT);

  BYTE WhoIs (const CBotTeamType &T);

  BYTE Remove(const CBotTeamType &TT); 
};


//��������� �������

#define MAKE_BOT_TRIG(a,b)           (a<<16|b)

#define BOT_TRIGCLASS_HEAR          0x01
#define BOT_TRIGCLASS_LOOK          0x02
#define BOT_TRIGCLASS_COMMAND       0x03
#define BOT_TRIGCLASS_PHISIC        0x04
#define BOT_TRIGCLASS_DEATH         0x05
#define BOT_TRIGCLASS_MOVE          0x06

//������� �����
//
#define BOT_TRIG_HEAR_SHOT          MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,1) // ������� �������
#define BOT_TRIG_HEAR_FRIEND        MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,2) // ������� �����
#define BOT_TRIG_HEAR_ENEMY         MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,3) // ������� �����
#define BOT_TRIG_HEAR_NEUTAL        MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,4) // ������� �������
#define BOT_TRIG_HEAR_SOUND         MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,5) // ������� ����
#define BOT_TRIG_HEAR_BOOM          MAKE_BOT_TRIG(BOT_TRIGCLASS_HEAR   ,6) // ������� �����

//������� ������

#define BOT_TRIG_SEE_FRIEND         MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,1) // ������ �����
#define BOT_TRIG_SEE_ENEMY          MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,2) // ������ �����
#define BOT_TRIG_SEE_NEUTAL         MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,3) // ������ �������
#define BOT_TRIG_SEE_DEAD           MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,4) // ������ ����
#define BOT_TRIG_SEE_COMBAT         MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,5) // ������ ���
#define BOT_TRIG_SEE_DANGER         MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,6) // ������ ����������
#define BOT_TRIG_ON_AIM             MAKE_BOT_TRIG(BOT_TRIGCLASS_LOOK   ,7) // ��� �� �������

//������� ������

#define BOT_TRIG_PH_WAVE            MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,1) // ������ �������
#define BOT_TRIG_PH_SHOT            MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,2) // ������ ����
#define BOT_TRIG_PH_FALL            MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,3) // ����
#define BOT_TRIG_PH_KICK            MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,4) // �������
#define BOT_TRIG_PH_DAMAGE          MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,5) // ������� �����������
#define BOT_TRIG_PH_LOOSEITEM       MAKE_BOT_TRIG(BOT_TRIGCLASS_COMMAND,6) // ������� �������


//������� ������

#define BOT_TRIG_DEATH_SELF         MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,1) // ���� ���
#define BOT_TRIG_DEATH_PLAYER       MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,2) // ���� �����
#define BOT_TRIG_DEATH_ENEMY        MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,3) // ���� ����
#define BOT_TRIG_DEATH_FRIEND       MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,4) // ���� ����
#define BOT_TRIG_DEATH_NEUTAL       MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,5) // ���� ������
#define BOT_TRIG_DEATH_COMMANDER    MAKE_BOT_TRIG(BOT_TRIGCLASS_DEATH  ,6) // ���� ��������

//������� ��������

#define BOT_TRIG_MOVE_CLEAR_PATH    MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,1) // ������ ������
#define BOT_TRIG_MOVE_NO_WAY        MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,2) // ���� ����
#define BOT_TRIG_MOVE_NEW_WAY       MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,3) // ����� ������
#define BOT_TRIG_MOVE_LOOKED_AT     MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,4) // ��� ��������� �� ���-��
#define BOT_TRIG_MOVE_CMP           MAKE_BOT_TRIG(BOT_TRIGCLASS_MOVE   ,5) // ��� ����� ������




struct CBotSquad
{
	//������ �����
	typedef SmartVector<DWORD> BotList;
	//��� ���������
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
	DWORD            MyTeam;    //��� �������
	DWORD            MySquad;   //��� �����
    DWORD            MyType;    //��� ���
	DWORD            MyStatus;  //��� ������(���..�����,����������)
	DWORD            MyMode;    //��� ����� (�������,������,�����,������,�������,�����������)
	DWORD            MyGoal;    //��� ����
};

struct CBotDefLife
{
	bool             Valid;     //�������\�����������
	float            Current;   //������� ��������
	DWORD            Start;     //���������
	DWORD            Max;       //������������
	float            Bleeding;  //������ �����(������\������);
};

struct CBotDefAttack
{
	float            PLevel;           // ��������� ����� ������
	bool             SingleHanded;
	float            accuracy;         // ��������
	float            tactics;          // �������
	DWORD            AimMode;          // ����� ���������
 	DWORD            CurrentWeapon;    // ������� ������
	DWORD            CurrentAmo;       // ����� ��������
	float            CurrentUsage;     // ������������� ���������
	float            ReloadTime;       // ����������������� ����� �����������
	float            EmptyTime;        // ����������������� ����� ����� ��������
	float            Sniper;           // �������
	float            Camper;           // ������
	DWORD            MoveFactor;	   // ����������� ��������
	float            MoveFactorParam;  // ��������� ������������
};

struct CBotDefMind
{
	float            Aggression;       // �������������
    float            Awarness;         // ����������������
	float            Reaction;         // �������
	float            Stupidness;       // ������

	float            HearFactor;       // ������ �����
	float            HearDistance;     // ������ �����

	float            LookAngle[3];     // ���� ������
	float            LookFactor;       // ������ ������
	float            LookDistance;     // ��������� ������
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
    //������ ������
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
    CBotDefTeam      TeamDef;   //���������� ���������
	CBotDefLife      Heal;      //�����
	CBotDefLife      Armor;     //�����
	AttackPack       Attack;    //��������� ��� ��� ������� ������
    CBotDefMind      Mind;      //��������� ����������;
	CBotDefMovent    Movent;    //��������� ��������
	CBotDefStat      MyStat;    //��������� ������������
public:
	CIdeaManager *Ideas;
	_ScriptPack     MyScripts;
    BYTE          __L1ParamsEnd;

	virtual void         CreateScriptParameters();

	//�������� ��� �����.
	//������� 1
	DWORD        GetBotID   ();	 //�������� ID
	DWORD        GetTeam    ();	 //�������� �������
	DWORD        GetSquad   ();	 //�������� ����� 
	DWORD        GetType    ();	 //�������� ���
	DWORD        GetStatus  ();	 //�������� ������

	DWORD        GetMyWeapon(int i); //�������� ������ � ����� i

	float        GetHeal    ();	 // �������� �����
	float        GetArmor   ();	 // �������� �����

	const bVector3f &GetPosition         (); // �������� �������
	const bVector3f &GetVectorOrientation(); // ������ �����������
	const bVector3f &GetAngleOrientation (); // ���� �����������

	const bVector3f &GetCurrentSpeed     (); // ������� ��������

	const bVector3f &GetSizeBox          (); // �������


	bool IsMoventPossible   ();  // �������� �� ��������
	bool IsSilentMovePosible();  // �������� �� �������� ��������
	bool IsRotationPossible ();  // �������� �� �������


    float Tick              ();  // ���������� ���          

    void  WholeReset        ();  // ������ �����

	void  MakeMyMarker      (DWORD mID=-1);// ��������� ������� �� ����
  
    DWORD OnMessage(const CBlopMessage &Message); // ��������� ���������


	//**************************************************************
	//**************************************************************
	// �������� ��� �������������

	//�������
	DWORD GetMoventToMarker         (bVector3f &Dir,const DWORD Marker); // �������� ������ ����������� �������� �� �������
	DWORD GetDirrectionToMarker     (bVector3f &Dir,const DWORD Marker); // �������� ������ ����������� �� ������
	DWORD GetShotDirrectionToMarker (bVector3f &Dir,const DWORD Marker); // �������� ������ ����������� �������� �� ������
	DWORD GetSilentMoventToMarker   (bVector3f &Dir,const DWORD Marker); // �������� ������ ����������� ��������� �������� �� �������

	//**************************************************************
	//**************************************************************
	// �������� ��� ����� 

	virtual bool MOVE_InDirrection          (const bVector3f &Dir,const WORD Force,const DWORD &Speed);// ��� ��������� � �������� �������
	virtual bool MOVE_ChangeMode            (const DWORD &Mode   ,const DWORD &Speed);					// ����� ������ ������
	virtual bool MOVE_DoJump                (const bVector3f &Dir,const DWORD &Speed);					// ������ � �����������
    virtual bool MOVE_ToPoint               (const bVector3f &To,const DWORD &Speed);

	virtual bool ORI_LOOK                   (const bVector3f &Dir,const float &Speed);                 // ���������� ��
	virtual bool ORI_ROTATE                 (const bVector3f &Dir,const float &Speed,const float Force);// ����������� ��
	virtual bool Say                        (LPCSTR S);

    //combat
    virtual void Fire                       (const DWORD Type,const int Rounds=-1);                    // ��������� � ������ Type ������� ����� ��������

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
