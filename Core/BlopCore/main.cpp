#include "ProjectSpace.h"
#include "ProjectMemory.h"
/*#include "Core/NetUtil/Transfer.h"
#include "Core/Containers/AccauntBase.h"
#include "Core/Chilli/Chilli.h"
#include "Core/NetUtil/ConnectManager.h"
*/
#include "Core/Client/BlopClient.h"

#include "USEIMPORT.h"


/*int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	__InitMem();
	BlopStartInformation BST;
    BST.Log.ConOut=PrintLine;
    BST.Log.LogEnabled  = true;
    BST.Log.UseLogFile  = true;
    BST.Log.UseScreenOut= true;
	BST.Window.hInstance=(DWORD)hInst;
    CBlopClient *Client=new CBlopClient(BST);
	Client->Start();
	delete Client;
	__KillMem();
	return 0;
}*/


/*

int main () 
{
	__InitMem();
	
	BlopStartInformation BST;
    BST.Log.ConOut=NULL;//PrintLine;
    BST.Log.LogEnabled  = true;
    BST.Log.UseLogFile  = true;
    BST.Log.UseScreenOut= false;
	BST.Window.hInstance=(DWORD)GetModuleHandle(0);
    CBlopClient *Client=new CBlopClient(BST);
	Client->Start();
	delete Client;
	__KillMem();
	return 0;
}
*/


CBlopClient *Client;

extern HANDLE shMainThread;

bool InitBlop(BlopStartInformation &BST)
{
 __InitMem();
 DuplicateHandle(GetCurrentProcess(), 
	             GetCurrentThread(), 
				 GetCurrentProcess(),
		         &shMainThread, 
				 0, 
				 FALSE, 
				 DUPLICATE_SAME_ACCESS);
  BST.Window.hInstance=(DWORD)GetModuleHandle(0);
  Client=new CBlopClient(BST);
  return Client!=NULL;
}

bool StartBlop()
{
	{
	SmartVector<int> V1;
	V1.push_back(1);
	}
	{
	SmartVector<GString> V1;
	V1.push_back("lalala");
	}
	{
	SmartVector<GString*> V1;
	V1.push_back(new GString("lalala"));
	}


	return Client->Init();
}

bool BlopTick()
{
   return Client->TimeTick(); 
}

void BlopEnd()
{
   delete Client;
   __KillMem();
}