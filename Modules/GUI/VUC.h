#ifndef _VUC_H
#define _VUC_H


namespace GUI
{
	void StoreGUI(LPCSTR FileName);
	void GrabMouse();
    void RenderGUI();
    void CreateGUI();

    float GetGUIParamenterf(LPCSTR Name,const float Def=0);
    void  EnableGUIIterations(const int Type);

};


#endif