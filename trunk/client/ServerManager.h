#include "Defines.h"
#include "TimerThread.h"
#include "OmniConnect.h"
#include "EventFactory.h"

class ConnectServer;

struct ServerInfo
{
    char mHost[HOSTLEN];
    int mPort;
    int mConnected;
    ConnectServer *mConnect;
    ConnectServer *(*mCreate)();
};

class ServerManager : public TimerObject
{
public:

    ServerManager();
    static ServerManager *GetInstance();
    void Add(const char *host, int port, ConnectServer *(*param)());
    static void ResetConnect( ConnectServer *connect );
    static void Connected( ConnectServer *connect );
    virtual void TimerCallback( int mEvent );
};


