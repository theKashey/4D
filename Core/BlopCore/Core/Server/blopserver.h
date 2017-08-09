//---------------------------------------------------------------------------

#ifndef BlopServerH
#define BlopServerH
//---------------------------------------------------------------------------

class BlopServer: public BlopBaseObject
{
 CConnectionManager *ConnectManager;
 CAccauntBase       *AccauntBase;
 CChilli            *Chilli; 
public:
 BlopServer(BlopStartInformation &BSI);
~BlopServer();
};

#endif
