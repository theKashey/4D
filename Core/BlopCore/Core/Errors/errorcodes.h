//---------------------------------------------------------------------------

#ifndef ErrorCodesH
#define ErrorCodesH

//---------------------------------------------------------------------------


#define BERR_SOCKET_BEGIN              0
#define BERR_SOCKET_INVALID_CREATE     1
#define BERR_SOCKET_BIND_ERROR         2
#define BERR_SOCKET_ERROR_LISTEN       3
#define BERR_SOCKET_ERROR_READ         4
#define BERR_SOCKET_ERROR_WRITE        5
#define BERR_SOCKET_CONNECT_ERROR      6
#define BERR_SOCKET_END                10

#define BERR_ACC_BASE_BEGIN            11
#define BERR_ACCBASE_CANT_GET_PASSWD   12
#define BERR_ACCBASE_CANT_SET_PASSWD   13
#define BERR_ACCBASE_WRONG_PASSWD      14
#define BERR_ACCBASE_NEW_PASSWD_SET    15
#define BERR_ACCBASE_GOOD_PASSWD       16
#define BERR_ACC_BASE_END              20


#define BERR_CONGUARD_BEGIN            21
#define BERR_CONGUARD_SEND             22
#define BERR_CONGUARD_READ             23
#define BERR_CONGUARD_CMD              24
#define BERR_CONGUARD_ACCEPT           25
#define BERR_CONGUARD_UNFD             26
#define BERR_CONGUARD_END              27

#define BERR_CONKEEPER_BEGIN           31
#define BERR_CONKEEPER_NO_RESPONSE     32
#define BERR_CONKEEPER_SYNC_FAILED     33
#define BERR_CONKEEPER_END             40

bool errErrorToString(DWORD ErrorCode,LPSTR CharBuf,LPDWORD Capapsy);
LPCSTR errErrorToString(DWORD ErrorCode);

#endif




