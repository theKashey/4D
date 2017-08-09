#ifndef _BLOP_CULL_H
#define _BLOP_CULL_H


namespace CULL
{

enum cullform
{
	cullform_box,
	cullform_sphere,
	cullform_ico
};

class CCullElement
{
protected:
  //position tasks
	typedef SmartVector<CCullElement*>CParentCullPack;
	CParentCullPack    ParentCull;

  	bVector3f Position;
	float     Rad;
	bVector3f Box;
  //occlusion query
	int       FirstChecked;
    BYTE      OccStatus;
	BYTE      FrameOCCFreq;
	BYTE      FrameOCCFreqHalf;
	
	cullform  MyForm;
	DWORD     ClassColor;
	CCullElement()
	{
		OccStatus=0;
        SetUseOcclusion(1);
        SetUseHalf     (1);
		SetClassColor  (0);
	}
public:

	void SetFrameOcc    (BYTE Occ){FrameOCCFreq=Occ;FrameOCCFreqHalf=Occ/2;}
	void SetFrameOccHalf(BYTE Occ){FrameOCCFreqHalf=Occ;}

	void SetPosition    (const float *P   ){Position=P;}
	void SetRad         (const float _Rad ){Rad=_Rad;}
	void SetBox         (const float *_Box){Box=_Box;}
	void SetUseOcclusion(const BYTE Use){OccStatus&=0xFE;OccStatus|=Use;}
	void SetUseHalf     (const BYTE Use){OccStatus&=(~0x80);OccStatus|=Use<<7;}

	void SetOCCFreq     (const BYTE Freq){FrameOCCFreq=Freq;}
	void SetHalfOCCFreq (const BYTE Freq){FrameOCCFreqHalf=Freq;}

	void SetForm        (cullform Form){MyForm=Form;	}
	void SetClassColor  (const DWORD CL){ClassColor=CL;}

	virtual bool IsVisibled(){return true;}

	void AddParent(CCullElement *Par){ParentCull.push_back(Par);}
};

typedef CCullElement *PCullElement;
PCullElement DLLTYPE CreateCullElement();

typedef PCullElement(*_fCullFarbic)();

void DLLTYPE RegisterCullFabric(_fCullFarbic);

}
#endif