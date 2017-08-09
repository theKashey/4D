#pragma once
template <class A>  //NULL
struct CInVertex0
{};
template <class A>  //1D
struct CInVertex1
{A Vertex[1];};
template <class A>  //2D
struct CInVertex2
{A Vertex[2];};
template <class A>  //3D
struct CInVertex3
{A Vertex[3];};
template <class A>  //4D
struct CInVertex4
{A Vertex[4];};


class CModelHolder
{

};

class CKeyFrameModel
{
	WORD NumVerticles;
	WORD NumFrames;
};

class CModel
{
public:
	CModel(void);
	~CModel(void);
};
