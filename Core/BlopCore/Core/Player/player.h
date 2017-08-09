//---------------------------------------------------------------------------

#ifndef PlayerH
#define PlayerH
//---------------------------------------------------------------------------


//global player
class /*DLLTYPE*/ CPlayer
{
 public:
  BlopTransfer        Transfer;       //tranfer level
//  BlopPlugIn          WorldDriver;    //world level
  DWORD               AID;            //accaunt id
  DWORD               PID;            //player id
  DWORD               ID;             //object id
  DWORD               PLEVEL;         // PLEVEL

  HANDLE           GetTransferEvent(){ return Transfer.R_EVENT;}
  virtual bool     EnterWorld  (DWORD SessionID);  // enters player to world by initializing this level

  void             ChargeRead  ();                 // Charges read
  void             BeginSession();                 // Starts Session
  void             EndSession  ();                 // Ends   Session
};

#endif
