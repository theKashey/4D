#ifndef _WIN_CONTROLS_H
#define _WIN_CONTROLS_H

class CWindowCaption:public CTextArea
{
  bool Locked;
public:
  CWindowCaption():CTextArea()
  {
	  Locked=false;
  }
  virtual int OnMouseMove   (const float X,const float Y,DWORD &KeyState);
  virtual int OnMouseUp     (const float X,const float Y,DWORD &KeyState);
  virtual int OnMouseDown   (const float X,const float Y,DWORD &KeyState);
  void OnFocusLost(){Locked=false;}
  virtual void Render()
  {
	  Text=((CWindow*)GetParent())->GetCaption();
      //if(((CWindow*)GetParent())->GetActiveElement())
	  //Text+=((CWindow*)GetParent())->GetActiveElement()->GetName();
	  //if(GetParent()->IsFocused())Text+="WF ";
	  //if(IsFocused())Text+="SF ";
	  CTextArea::Render();
  }

};

class CFormCloser:public CButton
{
 virtual int OnMouseClick   (const float X,const float Y,DWORD &KeyState)
 {
	 ((CWindow*)GetParent())->SetVisible(false);
	 return 1;
 }
 
};

#endif