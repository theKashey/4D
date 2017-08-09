//---------------------------------------------------------------------------


#pragma hdrstop

#include "ProjectSpace.h"
#include "Transfer.h"
#include "AccauntBase.h"
#include "Chilli.h"
#include "ConnectManager.h"
#include "BlopServer.h"

//---------------------------------------------------------------------------

BlopServer::BlopServer(BlopStartInformation &BSI)
{
 ConsoleOut       =new ConsoleUtil(BSI.Log,"SRV");
 ConsoleOut->Printf("Console started");

 BlopUtilsPack BUP;
 BUP.ConsoleOut=ConsoleOut;

 ConsoleOut->Printf("Loading modules");
 ConsoleOut->Printf(" -loading ConnectManager");
 ConnectManager   =new CTCPConnectionManager(BUP);
 ConsoleOut->Printf(" -loading AccauntManager");
 AccauntBase      =new CAccauntBase;
 ConsoleOut->Printf(" -loading Chilli");
 Chilli           =new CChilli(BUP);

 ConsoleOut->Printf("linking");

 ConnectManager->SetAccauntBase(AccauntBase);

 ConsoleOut->Printf("Stating modules");
 ConsoleOut->Printf(" -starting ConnectManager");
 if(!ConnectManager->Run())
 {ConsoleOut->Printf("Connecting manager fail..");return;}
}

BlopServer::~BlopServer()
{
 ConsoleOut->Printf("Engine finilization");
 delete ConnectManager;
 delete ConsoleOut;
}
