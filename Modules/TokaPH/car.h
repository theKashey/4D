#pragma once



class CCar
{
public:
	CBaseController * controller; 
	CBasePhisic *carBody;
    bVector3f    suspensionLength;
	DWORD        id;
	float        accel;
	float        steer;
	float        slide;
	bVector3f    SteerDir[2];
	float        WHEEL_DIAMETER;


	float        MAX_SPEED,MAX_REVERSE_SPEED,
		         MAX_STEER,MAX_SLIDE;
public:
	CCar(void);
	~CCar(void);
	void MakeCar(bVector3f Pos,bVector3f Orientation);
	void MakeParts(bVector3f Pos,bVector3f Orientation);
	void CarController();
	void Process();
};

