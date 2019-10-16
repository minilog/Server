#pragma once

#include <winsock.h>
#include <vector>
using namespace std;
class SocketUtil
{
public:
	static TCPSocketPtr CreateTCPSocket();
	inline static fd_set* FillSetFromVector(fd_set& outSet, const vector< TCPSocketPtr >* inSockets, int& ioNaxNfds);
	inline static void FillVectorFromSet(vector< TCPSocketPtr >* outSockets, const vector< TCPSocketPtr >* inSockets, const fd_set& inSet);
	static int			Select(const vector< TCPSocketPtr >* inReadSet,
		vector< TCPSocketPtr >* outReadSet,
		const vector< TCPSocketPtr >* inWriteSet,
		vector< TCPSocketPtr >* outWriteSet,
		const vector< TCPSocketPtr >* inExceptSet,
		vector< TCPSocketPtr >* outExceptSet);
};

TCPSocketPtr SocketUtil::CreateTCPSocket()
{
	SOCKET s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s != INVALID_SOCKET)
	{
		return TCPSocketPtr(new TCPSocket(s));
	}
	else
	{
		return nullptr;
	}
}
fd_set* SocketUtil::FillSetFromVector(fd_set& outSet, const vector< TCPSocketPtr >* inSockets, int& ioNaxNfds)
{
	if (inSockets)
	{
		FD_ZERO(&outSet);

		for (const TCPSocketPtr& socket : *inSockets)
		{
			FD_SET(socket->mSocket, &outSet);
#if !_WIN32
			ioNaxNfds = std::max(ioNaxNfds, socket->mSocket);
#endif
		}
		return &outSet;
	}
	else
	{
		return nullptr;
	}
}

void SocketUtil::FillVectorFromSet(vector< TCPSocketPtr >* outSockets, const vector< TCPSocketPtr >* inSockets, const fd_set& inSet)
{
	if (inSockets && outSockets)
	{
		outSockets->clear();
		for (const TCPSocketPtr& socket : *inSockets)
		{
			if (FD_ISSET(socket->mSocket, &inSet))
			{
				outSockets->push_back(socket);
			}
		}
	}
}


int SocketUtil::Select(const vector< TCPSocketPtr >* inReadSet,
	vector< TCPSocketPtr >* outReadSet,
	const vector< TCPSocketPtr >* inWriteSet,
	vector< TCPSocketPtr >* outWriteSet,
	const vector< TCPSocketPtr >* inExceptSet,
	vector< TCPSocketPtr >* outExceptSet)
{
	//build up some sets from our vectors
	fd_set read, write, except;

	int nfds = 0;

	fd_set *readPtr = FillSetFromVector(read, inReadSet, nfds);
	fd_set *writePtr = FillSetFromVector(read, inWriteSet, nfds);
	fd_set *exceptPtr = FillSetFromVector(read, inExceptSet, nfds);

	timeval *timeout = new timeval();
	timeout->tv_sec = 0; // Server sẽ lắng nghe trong 90s, nếu tham số timeout = NULL thì select sẽ chạy mãi.
	timeout->tv_usec = 0;


	int toRet = select(nfds + 1, readPtr, writePtr, exceptPtr, nullptr);

	if (toRet > 0)
	{
		FillVectorFromSet(outReadSet, inReadSet, read);
		FillVectorFromSet(outWriteSet, inWriteSet, write);
		FillVectorFromSet(outExceptSet, inExceptSet, except);
	}
	return toRet;
}