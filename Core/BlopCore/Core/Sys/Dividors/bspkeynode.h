#pragma once

//array lookup procedure
template <class KeyT,class IDT>
class CBSPDividorLookUp
{
private:
  int cID;
  typedef struct _CBSPElem
  {
   _CBSPElem *Left,*Right;
    KeyT Key;
    int  ID;
	IDT  SID;
  }CBSPElem;

 CBSPElem* NEW   ()               {return new CBSPElem;}
 void      Delete(CBSPElem *Elem) {delete Elem; }

 CBSPElem* ConstructBSPElement(CBSPElem* Base,char Dir,KeyT Key)
 {
  CBSPElem* Ret=NEW();
  Ret->Left=Ret->Right=NULL;
  Ret->Key=Key;
  Ret->ID =cID++;
  Ret->SID=0;
  if(Base)
  switch(Dir)
  {
   case BSP_ADD_LEFT : Base->Left =Ret;break;
   case BSP_ADD_RIGHT: Base->Right=Ret;break;
  }
  return Ret;
 }

 CBSPElem* GetNearestNode(CBSPElem* Begin,KeyT Key)
 {
   if(!Begin) return 0;
   if(Begin->Key == Key) return Begin;
   bool Less=Begin->Key < Key;
   CBSPElem* Ret;
   if(Less) Ret=GetNearestNode(Begin->Left ,Key);
   else     Ret=GetNearestNode(Begin->Right,Key);
   if(!Ret) Ret=Begin;
   return Ret;
 }

  CBSPElem *BSPBegin;
 public:
  CBSPDividorLookUp(){ BSPBegin=0;cID=1;}
~ CBSPDividorLookUp(){ RemoveAll(BSPBegin);}

  int GetIDFor(KeyT Key)
  {
	  CBSPElem* Base=GetNearestNode(BSPBegin,Key);
	  if(!Base || Base->Key!=Key) return 0;
	            return Base->ID;
  }

  IDT GetSIDFor(KeyT Key)
  {
	  CBSPElem* Base=GetNearestNode(BSPBegin,Key);
	  if(!Base || Base->Key!=Key) return 0;
	            return Base->SID;
  }

  void SetSIDFor(KeyT Key,IDT SID)
  {
	  CBSPElem* Base=GetNearestNode(BSPBegin,Key);
	  if(Base) Base->SID=SID;
  }

  void RemoveAll(CBSPElem *Start)
  {
   if(!Start)return;
   RemoveAll(Start->Left);
   RemoveAll(Start->Right);
   Delete(Start);
   Start=0;
  }

  int Push(KeyT Key)
  {
   IDT ID=GetIDFor(Key);
   if(ID) return ID;
   CBSPElem* Base=GetNearestNode(BSPBegin,Key);
   if(!Base)
   {
    BSPBegin=ConstructBSPElement(NULL,BSP_ADD_LEFT,Key);
    return BSPBegin->ID;
   }
   bool Less= Base->Key<Key;
   if(Less) Base=ConstructBSPElement(Base,BSP_ADD_LEFT ,Key);
   else     Base=ConstructBSPElement(Base,BSP_ADD_RIGHT,Key);
   if(Base) return Base->ID;
   return 0;
  }

};

template <class _KeyT,class IDT>
class CBSPDividorLookUpArray
{
	typedef const _KeyT KeyT;
private:
  IDT cID;
  typedef CBSPDividorLookUp<_KeyT,IDT> InA;
  typedef struct _CBSPElem
  {
	  InA LookUp;
	  SmartVector<_CBSPElem*> InArray;
  }CBSPElem;

 CBSPElem* NEW   ()               {return new CBSPElem;}
 void      Delete(CBSPElem *Elem) {delete Elem; }
 CBSPElem *BSPBegin;
 public:
  CBSPDividorLookUpArray(){ BSPBegin=0;cID=1;}
~ CBSPDividorLookUpArray(){ RemoveAll(BSPBegin);}

  void RemoveAll(CBSPElem *Start)
  {
   if(!Start)return;
   for(int i=0;i<Start->InArray.size();i++)
	   RemoveAll(Start->InArray[i]);
   Delete(Start);
   Start=0;
  }

  IDT inPushAs(IDT Sid,CBSPElem *Base,KeyT *Key,DWORD Size)
  {
	  int ID=Base->LookUp.Push(*Key);
	  if(Size<2)
	  {
		  IDT SID=Base->LookUp.GetSIDFor(*Key);
		  if(!SID)
		  {
			  if(!Sid) SID=cID++;else SID=Sid;
			  Base->LookUp.SetSIDFor(*Key,SID);
		  }
		  return SID;
	  }
	  //if(Base->InArray._allocated<ID) Base->InArray.SetSize(ID+1);
	  bool Less=Base->InArray.size()-1<ID;
	  if(!Less) Less=!Base->InArray[ID];
	  if(Less)//Base->InArray.size()-1<ID || !Base->InArray[ID])
	  { 
		  int DeltaC=ID;
		  int i;
		  int sID=Base->InArray.size()-1<ID?Base->InArray.size():ID;
		  for(i=0;i<sID      ;i++)if(Base->InArray[i])DeltaC--;
		  for(i=0;i<DeltaC+1;i++)Base->InArray.push_back(NEW()); 	  
		  if(Base->InArray._Size()==Base->InArray._Allocated())
			Base->InArray.push_back(NEW());
	  }
	  return inPushAs(Sid,Base->InArray[ID],Key+1,Size-1);
  }


  IDT inGetID(CBSPElem *Base,KeyT *Key,DWORD Size)
  {
	  int ID=Base->LookUp.Push(*Key);
	  if(Size<2)  return Base->LookUp.GetSIDFor(*Key);
	  if(Base->InArray.size()-1<ID)return 0;//Base->InArray.SetSize(ID+1);
	  if(!Base->InArray[ID])     return 0;//Base->InArray[ID]=NEW();
	  return inGetID(Base->InArray[ID],Key+1,Size-1);
  }

  IDT GetIDFor(KeyT *Key,DWORD Size)
  {
	if(!BSPBegin) return 0;
	return inGetID(BSPBegin,Key,Size);
  }

  IDT Push(KeyT *Key,DWORD Size)
  {
    if(!BSPBegin) BSPBegin=NEW();
	CBSPElem *Base=BSPBegin;
	return inPushAs(0,Base,Key,Size);
  }
  IDT PushAs(IDT SID,KeyT *Key,DWORD Size)
  {
    if(!BSPBegin) BSPBegin=NEW();
	CBSPElem *Base=BSPBegin;
	return inPushAs(SID,Base,Key,Size);
  }
};
