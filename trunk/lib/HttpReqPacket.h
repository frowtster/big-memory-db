//
// http packet class.
// include http parser
// http://www.w3.org/Protocols/rfc2616/rfc2616.html
// 

#include "Packet.h"
#include "LinkedList.h"

#define HEADER_MAX_SIZE 512
//#define HEADER_MAX_SIZE 8192
#define HEADER_METHOD_SIZE 10
#define HEADER_LINE_SIZE 80
#define BODY_MAX_SIZE 512

#define ContentLength "Content-Length: "
#define ContentType "Content-Type: "
#define Date "Date: "
#define UserAgent "User-Agent: "

class HttpReqPacket : public Packet
{
	char mMethod[HEADER_METHOD_SIZE];
	char mRequestURI[HEADER_LINE_SIZE];
	char mContentLength[HEADER_LINE_SIZE];
	char mContentType[HEADER_LINE_SIZE];
	char mDate[HEADER_LINE_SIZE];
	char mUserAgent[HEADER_LINE_SIZE];

	OList mListHeader;

protected:
	char mBody[BODY_MAX_SIZE];

public:
	HttpReqPacket();
	int AddHeader( char *buf );
	int AddHeaderLine( const char *buf );
	int AddBody( char *buf );
	int Parse();

	void Clear();

	char *GetHeader();
	char *GetBody();
	char *GetMethod();
	char *GetRequestURI();
	char *GetContentLength();
	char *GetContentType();
	char *GetDate();
	char *GetUserAgent();

	void printHeader();
};

