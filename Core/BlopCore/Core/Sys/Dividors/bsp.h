/***************************************************************************
                          BSP.h  -  description
                             -------------------
    begin                : Wed Jul 16 2003
    copyright            : (C) 2003 by kashey
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _BSP_
#define _BSP_

#define BSP_TYPE_CLASS_POINTER        1
#define BSP_TYPE_CLASS_REF            2
#define BSP_TYPE_SIMPLE_POINTER       3
#define BSP_TYPE_SIMPLE_REF           4


#define BSP_ADD_LEFT   0
#define BSP_ADD_RIGHT 1

template <class ItemTypeBase,class ItemType, class LessKeyType=int>
class CBSPDividor
{
  typedef struct _CBSPElem
  {
   _CBSPElem *Left,*Right;
   ItemType Elem;
  }CBSPElem;

 CBSPElem* NEW   ()               {return new CBSPElem;}
 void      Delete(CBSPElem *Elem) {delete Elem; }

 CBSPElem* ConstructBSPElement(CBSPElem* Base,char Dir,ItemType Item)
 {
  CBSPElem* Ret=NEW();
  Ret->Left=Ret->Right=NULL;
  Ret->Elem=Item;
  if(Base)
  switch(Dir)
  {
   case BSP_ADD_LEFT: Base->Left=Ret;break;
   case BSP_ADD_RIGHT: Base->Right=Ret;break;
  }
  return Ret;
 }

 CBSPElem* GetNearestNode(CBSPElem* Begin,LessKeyType Key)
 {
   if(!Begin) return 0;
   char LESS=ItemTypeBase::CmpKey(Begin->Elem, Key);
   if(LESS==0) return Begin;
  CBSPElem* Ret;
   if(LESS==-1)
    Ret=GetNearestNode(Begin->Left,Key);
  else
   Ret=GetNearestNode(Begin->Right,Key);
  if(!Ret) Ret=Begin;
  return Ret;
 }

 public:
  CBSPElem *BSPBegin;
  CBSPDividor(){ BSPBegin=0;}
~CBSPDividor(){ RemoveAll(BSPBegin);}

  bool LookUp(LessKeyType Key,ItemType Ret)
  {
   CBSPElem* Base=GetNearestNode(BSPBegin,Key);
   if(!Base || ItemTypeBase::CmpKey(Base->Elem,Key)!=0) return false;
   Ret=Base->Elem;
   return true;
  }

  void RemoveAll(CBSPElem *Start)
  {
   if(!Start)return;
   RemoveAll(Start->Left);
   RemoveAll(Start->Right);
   Delete(Start);
   Start=0;
  }

  bool Push(ItemType Item)
  {
   //try to find
   ItemType It;
   LessKeyType LKey=Item->GetKey();
   if(LookUp(LKey,It)) return false;
   CBSPElem* Base=GetNearestNode(BSPBegin,LKey);
   if(!Base)
   {
    BSPBegin=ConstructBSPElement(NULL,0,Item);
    return BSPBegin!=NULL;
   }
   char Less= ItemTypeBase::CmpKey(Base->Elem,LKey);
   if(Less<0)
    Base=ConstructBSPElement(Base,0,Item);
   else
    Base=ConstructBSPElement(Base,1,Item);
   return Base!=NULL;
  }

};


#endif

