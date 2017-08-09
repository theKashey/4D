//---------------------------------------------------------------------------

#ifndef refCMD_H
#define refCMD_H
//---------------------------------------------------------------------------

namespace BMSG
{

enum BLOP_MESSAGES
{
 CMD_TICK                            =  0 ,   //Perform tick
 CMD_START                           =  1 ,   //start object
 CMD_STOP                            =  2 ,   //stopobject
 CMD_TERMINATE                       =  5 ,   //terminate object
 CMD_SYNC                            =  6 ,   //sync with clone
 CMD_CALLYOURSEFL                    =  9 ,   //return object name
 CMD_LINK                            =  10,   //link with other object
 CMD_UPDATE                          =  11,   //notify object that something changed
 E_NOT_ACCEPTED                      =  12,
 CMD_USER_START                      =  0xF*4  //start of user defined commands
};

}
#endif
