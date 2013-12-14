#include "Defines.h"
#include "TimerThread.h"
#include "OmniConnect.h"
#include "EventFactory.h"

class ConnectServer : public OmniConnect
{
protected:

public:
    ConnectServer();
    virtual ~ConnectServer();

    virtual void ReadPacket( OmniPacket *packet, char *buf, size_t len );
    virtual void CloseCallback( void *arg );
    virtual void ConnectCallback( void *arg );
    virtual void TimerCallback( int mEvent );

	virtual void TableList();
	virtual void TableCreate(const char *tname, const char *param);
	virtual void TableDelete(const char *tname);
	virtual void TableUse(const char *tname);
	virtual void RowInsert(const char *param);
	virtual void RowSelect(const char *key, const char *col);
	virtual void RowUpdate(const char *key, const char *col, const char *val);
	virtual void RowDelete(const char *key);
};

// client factory
class ServerFactory : public ConnectFactory
{
public:
    static ConnectServer* createInstance()
    {
        printf("create instance\n" );
        return new ConnectServer;
    }
};


