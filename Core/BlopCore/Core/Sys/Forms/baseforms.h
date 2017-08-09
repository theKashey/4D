
#pragma once

class CBaseForms
{
public:
	CBaseForms(void);
	~CBaseForms(void);
};

struct CBBOXf
{
	bVector3f Center;
	bVector3f Sized;
};

struct COOBB:public CBBOXf
{
	bVector3f Orientation;
};
