

void di_matrixprep(float finalMatrix[16]);
void di_ProcessVector(neV4 &pos,neV3 &_out);
void di_ProcessPack(float realX,float realY,float deltainx,float deltainy,float fzdRezX,float fzdRezY);

struct ZGridData
{
	bVector3f V;
	float     T[2];
};

