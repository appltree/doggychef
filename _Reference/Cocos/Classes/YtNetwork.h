/*
YtNetwork.h
YongTrim Lib
@initialize at 121221

@history
121221	yoonsr	initialize
*/

#ifndef __YTNETWORK_H__
#define __YTNETWORK_H__

#include "YtLib.h"

using namespace cocos2d;

USING_NS_CC_EXT;

class YtNetwork : public CCObject
{
private:
public:
};

typedef enum tag_YtNetworkRetMsg
{
    YtNetworkRetMsg_Success,
    YtNetworkRetMsg_Failure
    
}YtNetworkRetMsg;


class YtNetworkMananger : public CCObject
{
public:
	YtNetworkMananger();
	~YtNetworkMananger();
	std::string onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);
    std::vector<char>* onHttpRequestCompletedBinary(CCHttpClient *sender, CCHttpResponse *response);
	CCHttpRequest* httpRequest(const char* strUrl, const char *postData, SEL_HttpResponse pSelector);
    CCHttpRequest* httpRequestBinary(const char* strUrl, const char *postData, int length, SEL_HttpResponse pSelector);
    
    CCHttpRequest* httpRequestPutGet(const char* strUrl, SEL_HttpResponse pSelector, bool isGet);

	static YtNetworkMananger* sharedNetworkManager();
};

#endif // __YTNETWORK_H__
