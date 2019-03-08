#include <Windows.h>
#include <iostream>
#include <winhttp.h>
//#include "../X1FastCall.h"

#pragma comment( lib, "Winhttp.lib" )

class CHTTPSession
{
private:
	HINTERNET hSession, hConnect, hRequest;
	LPCWSTR wcUserAgent;
	LPCWSTR wcHTTPVerb;

	int iHTTPMethod;
	std::wstring wsDomain, wsDocument;
	
	int iSESSION_PROTOCOL, iSESSION_PORT;
private:
	bool SetHTTPSecuirty(int iProtocolID);
	bool CreateSession();
	bool SetServer(LPCWSTR wcServerDomain, INTERNET_PORT wdPort);
	bool SetRequest(LPCWSTR wcData);
	bool SendRequest();
	bool SetResponseHandle();
	bool HTTPPhraseData(std::string sTarget, std::string sParameters);

	void CloseHandles();
public:

	std::string sPOSTData, sPOSTFile;

	std::wstring GetResponseHeader();
	int GetHTTPStatusCode();
	bool ReciveData(std::string& sData);
	bool SetHTTPMethod(int iVERBID);

	bool Send(LPCWSTR wcDomain, LPCWSTR wcDirectory);
	bool Recv(int& iStatusCode, std::string& sHeader, std::string& sData);
	int Transmit(int iHTTP_Method, int iHTTP_Protocol, std::string sTarget, std::string& sRecvData, std::string sParameters, int iEncyption);

	CHTTPSession::CHTTPSession()
	{
		wcUserAgent = L"MyUserAgent/1.0";
	}

	CHTTPSession::CHTTPSession(LPCWSTR lwcUserAgent)
	{
		wcUserAgent = lwcUserAgent;
	}

	CHTTPSession::~CHTTPSession()
	{
		CloseHandles();
	}
	void AddPOSTFile(std::string sData)
	{
		sPOSTFile = sData;
	}
};

enum CHTTP_PROTOCOLID
{
	HTTP_CLEAR = 10,
	HTTP_SSL
};

enum CHTTP_METHODID
{
	HTTP_GET = 10,
	HTTP_POST
};