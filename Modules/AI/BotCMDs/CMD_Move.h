int CBotScript::DoMove  (const BotCMDPacked &In)
{
	bVector3f Dir;
//    DWORD Flags;
	DWORD const10=10;
#define com
	switch(In.CMD)
	{
 //*************************************************************
     case BOT_CMD_MOVE_TO           : 
		 MAKESTRUCT(CMD_MOVE_TO,c1)
         #undef com
         #define com c1
                PATH::FindMarker(Dir,com->GetWayPoint());
             if(In.Bot->MOVE_InDirrection(Dir,0,com->GetSpeed()))
				  return 1;
			 else return 0;
 //*************************************************************
     case BOT_CMD_MOVE_DIR          : 
		 MAKESTRUCT(CMD_MOVE_DIR,c2)
         #undef com
         #define com c2

             if(In.Bot->MOVE_InDirrection(com->GetDirrection(),com->GetForse(),com->IsStoped()?0:const10))
				  return 1;
			 else return 0;
 //*************************************************************
     case BOT_CMD_CHANGE_MOVE_TYPE  : 
		  MAKESTRUCT(CMD_CHANGE_MOVE_TYPE,c3)
          #undef com
          #define com c3
  			  if(In.Bot->MOVE_ChangeMode(com->GetMoveType(),const10))
				   return 1;
			  else return 0;
 //*************************************************************
     case BOT_CMD_DO_JUMP           : 
		  MAKESTRUCT(CMD_DO_JUMP,c4);
          #undef com
          #define com c4
		      if(In.Bot->MOVE_DoJump(com->GetDirrection(),const10))
			       return 1;
		      else return 0;
 //*************************************************************
     case BOT_CMD_GET_TO            :
		 //MAKESTRUCT(CMD_LOOK_TO,c5)
		 break;  
 //*************************************************************
     case BOT_CMD_LOOK_TO           :
		  MAKESTRUCT(CMD_LOOK_TO,c6)
          #undef com
          #define com c6
  			  if(In.Bot->ORI_LOOK(com->GetDirrection(),const10))
				   return 1;
			  else return 0;
 //*************************************************************
     case BOT_CMD_LOOK_AT           :  
		  MAKESTRUCT(CMD_LOOK_AT,c7)
          #undef com
          #define com c7
			if(In.Bot->GetMoventToMarker(Dir,com->GetWayPoint()))
			{
				In.Bot->ORI_LOOK(Dir,const10);
				return 1;
			}
			else return 0;
 //*************************************************************
     case BOT_CMD_ROTATE            : 
		 MAKESTRUCT(CMD_ROTATE,c8)
         #undef com
         #define com c8
		 if(In.Bot->ORI_ROTATE(com->GetDirrection(),const10,com->IsAnglesForced()))
			  return 1;
		 else return 0;
	}
	LastError.Format("undefined move command %d -%s",In.Message,TryTranslateCMD(In.CMD,true));
	return 0;
}