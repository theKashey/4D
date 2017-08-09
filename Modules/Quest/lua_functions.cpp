#include "stdafx.h"
#include "QuestScene.h"

bool ZM_before(const DWORD i,Draw::Skin::Surface *For)
{
 if(!CQuestGraph::Entry->GetCurrentScene())return false;
 return CQuestGraph::Entry->GetCurrentScene()->ModAlphaForObject(i,For);
}

LUAFUNC(loadZM)
{
 DWORD add=lua::_tonumber(L,2);
 if(CQuestGraph::zmesh)delete CQuestGraph::zmesh;
 CQuestGraph::zmesh=NULL;
 GString Name=lua::_tostring(L,1);
 if(add){Name=("Quest/Scenes/"+Name);Name=Name+"/"+"scene.fsm"; }
 CQuestGraph::zmesh=new Draw::Skin::SkinnedMesh(Name);
 CQuestGraph::zmesh->SetBeforeSurfaceRenderCallback(ZM_before);
 //zmesh->SetAfterSurfaceRenderCallback(ZM_before);
 if(CQuestGraph::zmesh && (int)CQuestGraph::zmesh->getNumSurfaces()>0){}
 else {if(CQuestGraph::zmesh)delete CQuestGraph::zmesh;CQuestGraph::zmesh=NULL;}

 Name=lua::_tostring(L,1);
// BackTexture=Textures::LoadTexture("Quest/Scenes/"+Name+"/"+"color.tga");
 return 0;
}

LUAFUNC(luaSetQuestSceneCamera)
{
	bVector3f V;
	V[0]=lua_tonumber(L,2);
	V[1]=lua_tonumber(L,3);
	V[2]=lua_tonumber(L,1);

  	World()->Camera()->SetPosition  (V);
    return 0;
}

LUAFUNC(luaGetQuestSceneCamera)
{
    bVector3f Pos=World()->Camera()->GetPosition();  
    lua_pushnumber(L,Pos[2]);
    lua_pushnumber(L,Pos[0]);
    lua_pushnumber(L,Pos[1]);
    return 3;
}


static bVector3f QuestLookOn;

LUAFUNC(luaSetQuestSceneLookAt)
{
        QuestLookOn.Set(lua_tonumber(L,2),
		                lua_tonumber(L,3),
						lua_tonumber(L,1));
	    World()->Camera()->SetLookAt(QuestLookOn);
        return 0;
}

LUAFUNC(luaGetQuestSceneLookAt)
{
    lua_pushnumber(L,QuestLookOn[2]);
    lua_pushnumber(L,QuestLookOn[0]);
    lua_pushnumber(L,QuestLookOn[1]);
    return 3;
}


LUAFUNC(dofsmtozconvert)
{
 return 0;
}

LUAFUNC(doSaveCamera)
{
 CQuestScene *S=CQuestGraph::Entry->GetCurrentScene();
 if(S)
 {
   CreateDirectory(bGetRootPath()+GString("Quest/Scenes/")+S->GetName()+"/scripts/",NULL);
   DWORD f=bOpenFile(GString("Quest/Scenes/")+S->GetName()+"/scripts/cameraset.lua","w+");
   bWriteString(f,"function SceneSetCam()\n");
   bVector3f P=World()->Camera()->GetPosition();
   bWriteVString(f,"  quest.utils.setcam(%f,%f,%f);\n",P[2],P[0],P[1]);
   bWriteVString(f,"  quest.utils.camtarget(%f,%f,%f);\n",QuestLookOn[2],QuestLookOn[0],QuestLookOn[1]);
   bWriteVString(f,"  win_fov=%f;\n",lua::getfloat("win_fov"));
   bWriteVString(f,"  win_znear=%f;\n",lua::getfloat("win_znear"));
   bWriteVString(f,"  win_zfar=%f;\n",lua::getfloat("win_zfar"));

   bWriteString(f,"end;\n\n");
   bCloseFile(f);
 }
 return 0;
}

#include <Textures/tga/libtarga.h>
//#pragma comment (lib,"../../Core/BlopCore/StaticModules/Textures.lib")

LUAFUNC(doSaveZScene)
{
 CQuestScene *S=CQuestGraph::Entry->GetCurrentScene();
 if(S)
 {
    DWORD f=bOpenFile(GString("Quest/Scenes/")+S->GetName()+"/Z.zga","wb+");
	// FORMAT
	// 4b w,h
	// 4b z32
	// 4b zfar,znear
	// 4b zdx,zdy
	// data...
    DWORD W;
    W=lua::getdword("win_width");
	bWriteFile(&W    ,4,1,f);
    W=lua::getdword("win_height");
	bWriteFile(&W    ,4,1,f);
	DWORD z32=32;
	bReadFile(&z32  ,4,1,f);
    float Z=lua::getdword("win_znear");
	bWriteFile(&Z,4,1,f);
    Z=lua::getdword("win_zfar");
	bWriteFile(&Z ,4,1,f);
	bWriteFile(&z32  ,4,1,f);
	bWriteFile(&z32  ,4,1,f);

     {
    const DWORD W=lua::getdword("win_width");
    const DWORD H=lua::getdword("win_height");
    float *Buf=(float*)malloc((W+1)*(H+1)*sizeof(float));
    RENDER->ForseClear(RENDER_CLEAR_DEPTH);
    CQuestGraph::Entry->Draw(NULL);
    RENDER->ReadPixelf(0,0,W,H,grDEPTH_COMPONENT,Buf);
	bWriteFile(Buf,W*H*4,1,f);

	tga_write_rawZ(GString("Data/Quest/Scenes/")+S->GetName()+"/Z.tga",W,H,Buf,TGA_TRUECOLOR_24,true);
    free(Buf);
     }
	bCloseFile(f);
//	QType=QD_Z;
  }
	return 0;
}

LUAFUNC(zbalance)
{
 if(lua::_tonumber(L,1)!=1)
   Print2Console(SYS_QUEST,"Start balansing scene..");
 //lua::dostring("win_zfar=100000");
 //lua::dostring("win_updateviewport=1");
 //World()->Camera()->Init();
 RENDER->ForseClear(RENDER_CLEAR_DEPTH);
 CQuestGraph::Entry->Draw(NULL);
 const DWORD W=lua::getdword("win_width");
 const DWORD H=lua::getdword("win_height");
 float *Buf=(float*)malloc((W+1)*(H+1)*sizeof(float));
 RENDER->ReadPixelf(0,0,W,H,grDEPTH_COMPONENT,Buf);
 const DWORD WH=W*H;
 float mZ=1000;
 float MZ=0;
 const float *B=Buf;
 for(DWORD i=0;i<WH;++i,++B)
     {
       if(*B!=0 && *B!=1)
           {
             mZ=MIN(mZ,*B);
             MZ=MAX(MZ,*B);
           }
     }
 free(Buf);
 bVector3f posm(W/2,H/2,mZ);
 bVector3f posM(W/2,H/2,MZ);
 bVector3f Rm;
 bVector3f RM;
 bVector3f Cen;
 World()->Camera()->ScreenToClient(posm,Rm);
 World()->Camera()->ScreenToClient(posM,RM);
 posm[2]=0;
 World()->Camera()->ScreenToClient(posm,Cen);
 float Zm=lua::getdword("win_znear")+(Cen-Rm).Length();
 float ZM=lua::getdword("win_znear")+(Cen-RM).Length();
 float ZmDelta=fabs(lua::getdword("win_znear")-Zm);
 float ZMDelta=fabs(lua::getdword("win_zfar")-ZM);
 if(lua::_tonumber(L,1)!=1)
 {
 if(ZmDelta>1 || ZMDelta>1)Print2Console(SYS_QUEST,"need to recalibrate(%f-%f)",ZmDelta,ZMDelta); 
 Print2Console(SYS_QUEST,"znear at %f(%f), zfar at %f(%f) and D=%f",Zm,mZ,ZM,MZ,ZM-Zm);
 Print2Console(SYS_QUEST,"as lua: win_znear=%f;win_zfar=%f;win_updateviewport=1;",Zm,ZM);
 }
 else
     Print2Console(SYS_QUEST,"(%f-%f)",ZmDelta,ZMDelta); 
 if(lua::_tonumber(L,1)!=0)
     lua::doVstring("win_znear=%f;win_zfar=%f;win_updateviewport=1;",Zm,ZM);
 return 0;
}

LUAFUNC(luaCreateScene)
{
  GString SName=lua::_tostring(L,1); 
  GString RootPath=bGetRootPath()+GString("Quest/Scenes/__DEF/");
  GString ScenePath=bGetRootPath()+GString("Quest/Scenes/")+SName;
  CreateDirectory(ScenePath,NULL);
  CreateDirectory(ScenePath+"/scripts/",NULL);

  CopyFile((RootPath+"color.tga"),(ScenePath+"/color.tga"),true);
  CopyFile((RootPath+"Z.zga"    ),(ScenePath+"/Z.zga"    ),true);
  RootPath +="scripts/";
  ScenePath+="/scripts/";

  CopyFile((RootPath+"activate.lua"  ),(ScenePath+"activate.lua"  ),true);
  CopyFile((RootPath+"deactivate.lua"),(ScenePath+"deactivate.lua"),true);
  CopyFile((RootPath+"main.lua"      ),(ScenePath+"main.lua"      ),true);
  CopyFile((RootPath+"objectdef.lua" ),(ScenePath+"objectdef.lua"      ),true);

  DWORD f1=bOpenFile(ScenePath+SName+".qpr","r");
  if(f1) bCloseFile(f1);
  else
  {
   DWORD f=bOpenFile(ScenePath+SName+".qpr","wb+");
   bWriteString(f,"[QDE Project]\n");
   bWriteString(f,"Version=\"1.00\"\n");
   bWriteString(f,"Name=\""+SName+"\"\n");
   bWriteString(f,"[End]\n");
   DWORD f2=bOpenFile(RootPath+"projtail.qpr","rb",true);
   bWriteString(f,(LPCSTR)bGetFileData(f2));
   bCloseFile(f2);
   bCloseFile(f);
  }
  return 0;
}


extern void ParseQuestCatalog();

LUAFUNC(lua_qReloadScenes)
{
 ParseQuestCatalog();
 return 0;
}

LUAFUNC(lua_DefineObject)
{
 GString ObjName    =lua::_tostring(L,1);
 if(!CQuestGraph::Entry->GetCurrentScene())
 {
  Error2Console(SYS_QUEST,"No scene selected");
  return 0;
 }
 CQuestGraph::Entry->GetCurrentScene()->DefineObject(ObjName,lua::_tonumber(L,2)); 
 return 0;
}

LUAFUNC(lua_MakeNewObject)
{
 GString ObjName    =lua::_tostring(L,1);
 GString SurfaceName=lua::_tostring(L,2);
 //lua::doVstring("quest.objectalias.%s=\"%s\";",(LPCSTR)SurfaceName,(LPCSTR)ObjName);
 lua::doVstring("quest.objects._NEW(\"%s\",%d)",(LPCSTR)ObjName,(DWORD)lua::_tonumber(L,3));
 return 0;
}



LUAFUNC(lua_obj_GETLOCATION)
{
  if(!CQuestGraph::Entry->GetCurrentScene())return 0;
  CQuestObject *O=CQuestGraph::Entry->GetCurrentScene()->GetObjectByName(lua::_tostring(L,1));
  if(!O){ Error2Console(SYS_QUEST,"No object named <%s>",lua::_tostring(L,1));return 0;}
  lua::pushnumber(L,O->GetLocation());
  return 1;
}

LUAFUNC(lua_obj_fromZ)
{
  lua::dostring("for i,v in pairs(models.assemblers.scene.S) do quest.objects._DEFINE(v,v,64);end;");
  return 0;
}

LUAFUNC(lua_obj_SWITCH)
{
 if(!CQuestGraph::Entry->GetCurrentScene())return 0;
 CQuestObject *O=CQuestGraph::Entry->GetCurrentScene()->GetObjectByName(lua::_tostring(L,1));
 if(!O){ Error2Console(SYS_QUEST,"No object named <%s>",lua::_tostring(L,1));return 0;}
 switch((DWORD)lua::_tonumber(L,2))
  {
   case 1:O->SetVisibled(!O->Visibled());break;  //Visibled
   case 2:O->SetStatic  (!O->IsStatic());break;   //Static
   case 3:O->SetActive  (!O->Active  ());break; //Enabled
   case 4:;break;  //Active
  }
 return 0;
}

LUAFUNC(lua_MakeNewActor)
{
 GString ObjName    =lua::_tostring(L,1);
 if(!CQuestGraph::Entry->GetCurrentScene())
 {
  Error2Console(SYS_QUEST,"No scene selected");
  return 0;
 }
 CQuestGraph::Entry->GetCurrentScene()->DefineActor(ObjName,lua::_tonumber(L,2)); 
 return 0;
}

LUAFUNC(lua_act_Drive)
{
    GString DriveName=lua::_tostring(L,1);
    if(!CQuestGraph::Entry->GetCurrentScene())
    {
     Error2Console(SYS_QUEST,"No scene selected");
     return 0;
    }
	CQuestGraph::Entry->GetCurrentScene()->DriveActor(DriveName);
	return 0;
}

void QluaObjects()
{
    lua::dostring("quest.objects={}"); 
	lua::dostring("quest.actors={}"); 
	lua::luafunction("_NEW",lua_DefineObject,"quest.objects");
	lua::luafunction("_DEFINE",lua_MakeNewObject,"quest.objects");
	lua::luafunction("_GETLOCATION",lua_obj_GETLOCATION,"quest.objects");
	lua::luafunction("_FROMZ",lua_obj_fromZ,"quest.objects");
	lua::luafunction("_SWITCHLOC",lua_obj_SWITCH,"quest.objects");

	lua::luafunction("_ACTOR",lua_MakeNewActor,"quest.actors");
	lua::luafunction("_DRIVE",lua_act_Drive,"quest.actors");
}

static DWORD DumpFile=0;

void writeAlias(const GString &N,LPCSTR A)
{
 lua::dostring("tmps="+N+A);
 LPCSTR C=lua::getstring("tmps");
 if(C) bWriteString(DumpFile,"   "+N+A+" = \""+C+"\";\n");
}

LUAFUNC(objDUMP)
{
 GString Name=lua::_tostring(L,1);
 bWriteString(DumpFile,"-- "+Name+" definitions...\n\n");
 GString SName="quest.objects."+Name+".";
 writeAlias(SName,"mousepointer");
 writeAlias(SName,"_ONmouseon");
 writeAlias(SName,"_ONmouseout");
 writeAlias(SName,"_ONClick");
 writeAlias(SName,"_ONIdle");
 writeAlias(SName,"Hint");
 return 0;
}

LUAFUNC(save_Active)
{
  CQuestScene *S=CQuestGraph::Entry->GetCurrentScene();
  if(S)
  {
    DWORD f=bOpenFile(GString("Quest/Scenes/")+S->GetName()+"/Scripts/objectdef.lua","wb+");
	DumpFile=f;
	bWriteString(f,"-- start auto generated object def code\n\n");
	bWriteString(f,"function DefineObjects()\n");
	bWriteString(f,"-- here one will create objects\n");
	const CQuestScene::CQObjects &O=S->AssessObjects();
	GString NameWill;
	for( CQuestScene::CQObjects::Citerator it=O.begin();it!=O.end();++it)
	{
	 NameWill.Format("   quest.objects._DEFINE(\"%s\",\"%s\",%d);\n",it->GetName(),it->GetName(),it->GetLocation());
	 bWriteString(f,NameWill);
	}
	bWriteString(f,"-- here one will set object super props\n\n");
	bWriteString(f,"");

	lua::luafunction("objDUMP",objDUMP);
	lua::dostring("for i,v in pairs(quest.objects) do objDUMP(i);end;objDUMP=nil;");

	bWriteString(f,"\nend;\n");
	bCloseFile(f);
   }
 return 0;
}

void QluaINIT()
{
    lua::luafunction("loadZmodel",loadZM,"quest.utils");
    lua::luafunction("setcam",luaSetQuestSceneCamera,"quest.utils");
    lua::luafunction("camtarget",luaSetQuestSceneLookAt,"quest.utils");
    lua::luafunction("getcam",luaGetQuestSceneCamera,"quest.utils");
    lua::luafunction("getcamtarget",luaGetQuestSceneLookAt,"quest.utils");
    lua::luafunction("savecamera",doSaveCamera,"quest.utils");
    lua::luafunction("saveZ",doSaveZScene,"quest.utils");
    lua::luafunction("CreateSceneDir",luaCreateScene,"quest.utils");
    lua::luafunction("RefreshDir",lua_qReloadScenes,"quest.utils");
    
    lua::luafunction("dofsmtozconvert",dofsmtozconvert,"quest.utils");
    lua::luafunction("zbalance",zbalance,"quest.utils");
	lua::luafunction("_SaveActive",save_Active,"quest");
    lua::dofile("Scripts/Utils/convert/fsmtoquestz.lua");

	QluaObjects();

}