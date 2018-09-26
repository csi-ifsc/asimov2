#ifndef SOCKETS_H
#define SOCKETS_H

#include "DefinicoesImportantes.h"

#ifdef _WIN32

#include <winsock.h>
#include <iostream>
#include <stdio.h>

using namespace std;

#define PRINTERROR(s)	\
		fprintf(stderr,"\n%: %d\n", s, WSAGetLastError())


class Sockets
{
public:
	Sockets();
	virtual ~Sockets();
	void TestVersion();
	int Connect();
	int WaitConnection();
	int Send(char *Info, int tamanho);
	char* Receive();
	void DefineIp(char *Ip);
	void DefinePort(short Port);
	void CloseConection();
	void Timeout(int sec);
protected:
private:
	char *Ip;
	short Port;

	LPHOSTENT lpHostEntry;
	SOCKET	theSocket;
	SOCKADDR_IN saServer;
	SOCKET	listenSocket;
	int nRet;
};
#endif
//-------------------------------------------------------------------------------------------------------
#ifndef _WIN32

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <stdio.h>
#include <ifaddrs.h>
#include <fcntl.h>

class Sockets
{
public:
	Sockets();
	virtual ~Sockets();
	int CreateSocket();
	//-----------------------
	int WaitConnection();
	int ListenConnection();
	int TestConnection();
	//-----------------------
	void DefinePort(short Port);
	int Send(char *Info, int tamanho);
	char* Receive();
	void CloseConection();
	void Timeout(int sec);
	void getIp();
	bool testIp(char *Ip);
protected:
private:
	short Port;
	int socket_desc;
	int new_socket;
	int c;
	int *new_sock;
	struct sockaddr_in server;
	struct sockaddr_in client;

	struct ifaddrs * ifAddrStruct;
	struct ifaddrs * ifa;
	void * tmpAddrPtr;


};

#endif

#endif // SOCKETS_H
