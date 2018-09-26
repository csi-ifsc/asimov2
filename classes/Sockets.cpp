#include "Sockets.h"

#ifdef _WIN32

Sockets::Sockets()
{
	//ctor
}

Sockets::~Sockets()
{
	//dtor
	WSACleanup();
	closesocket(this->theSocket);
	closesocket(this->listenSocket);
}

void Sockets::CloseConection() {
	closesocket(this->theSocket);
	closesocket(this->listenSocket);
}

int Sockets::Connect(){

	closesocket(this->theSocket);
	closesocket(this->listenSocket);

	cout << "\nStream Client connecting to server: " << this->Ip << " on port: " << this->Port << ".\n" << endl;


	this->lpHostEntry = gethostbyname(this->Ip);

	if (this->lpHostEntry == NULL)
	{
#ifdef _MSC_VER
		PRINTERROR("gethostbyname()");
#endif
		cout << "Erro 1" << endl;
		return -1;
	}

	this->theSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->theSocket == INVALID_SOCKET)
	{
#ifdef _MSC_VER
		PRINTERROR("socket()");
#endif
		cout << "Erro 2" << endl;
		return -1;
	}



	this->saServer.sin_family = AF_INET;

	this->saServer.sin_addr = *((LPIN_ADDR)*this->lpHostEntry->h_addr_list);
	this->saServer.sin_port = htons(this->Port);

	int nRet;

	nRet = connect(this->theSocket, (LPSOCKADDR)&this->saServer, sizeof(struct sockaddr));
	if (nRet == SOCKET_ERROR)
	{
#ifdef _MSC_VER
		PRINTERROR("socket()");
#endif
		closesocket(this->theSocket);
		cout << "Erro 3" << endl;
		return -1;
	}
	return 0;
}

int Sockets::WaitConnection(){

	closesocket(this->theSocket);
	closesocket(this->listenSocket);

	this->listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
#ifdef _MSC_VER
		PRINTERROR("socket()");
#endif
		cout << "Erro" << endl;
		return -1;
	}

	this->saServer.sin_family = AF_INET;
	this->saServer.sin_addr.s_addr = INADDR_ANY;
	this->saServer.sin_port = htons(this->Port);



	this->nRet = bind(this->listenSocket, (LPSOCKADDR)&this->saServer, sizeof(struct sockaddr));
	if (this->nRet == SOCKET_ERROR)
	{
#ifdef _MSC_VER
		PRINTERROR("bind()");
#endif
		closesocket(this->listenSocket);
		cout << "Erro" << endl;
		return -1;
	}

	//int nLen;
	//nLen = sizeof(SOCKADDR);
	char szBuf[256];

	this->nRet = gethostname(szBuf, sizeof(szBuf));
	if (this->nRet == SOCKET_ERROR)
	{
#ifdef _MSC_VER
		PRINTERROR("gethostname()");
#endif
		closesocket(this->listenSocket);
		cout << "Erro" << endl;
		return -1;
	}

	cout << "\nServer named " << szBuf << " waiting on port " << this->Port << ".\n" << endl;

	this->nRet = listen(this->listenSocket, SOMAXCONN);
	if (this->nRet == SOCKET_ERROR)
	{
#ifdef _MSC_VER
		PRINTERROR("listen()");
#endif
		closesocket(this->listenSocket);
		cout << "Erro" << endl;
		return -1;
	}

	this->theSocket = accept(this->listenSocket, NULL, NULL);
	if (this->theSocket == INVALID_SOCKET)
	{
#ifdef _MSC_VER
		PRINTERROR("accept()");
#endif // _MSC_VER
		closesocket(this->listenSocket);
		cout << "Erro" << endl;
		return -1;
	}

	//memset(szBuf, 0, sizeof(szBuf));
	return 0;
}

void Sockets::TestVersion(){
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	int nRet;

	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		cout << "\nWrong Version." << endl;
		return;
	}
}

int Sockets::Send(char *Info, int tamanho){

	cout << "Data Send. (" << tamanho << ")" << endl;

	int nRet = send(this->theSocket, (char*)Info, tamanho, 0);
	if (nRet == SOCKET_ERROR)
	{
#ifdef _MSC_VER
		PRINTERROR("send()");
#endif // _MSC_VER
		closesocket(this->theSocket);
		cout << "Erro" << endl;
		return -1;
	}
	return 0;
}

char* Sockets::Receive(){

	char* mensagem = (char*)malloc(1000);
	memset(mensagem, '\0', 1000);
	char* mensagemCompleta = NULL;

	int total = 0;
	int tamanhototal = 0;

	int nr = recv(this->theSocket, (char*)mensagem, 1000, 0);
	if (nr == -1){
		closesocket(this->theSocket);
		free(mensagem);
		return NULL;
	}

	total += nr;
	mensagemCompleta = (char*)realloc(mensagemCompleta, total);
	//memset(mensagemCompleta,'\0', total);
	memcpy(mensagemCompleta + total - nr, mensagem, nr);

	char aux[20];
	memset(aux, '\0', sizeof(aux));
	memcpy(aux, mensagem, 10);
	sscanf(aux, "%d", &tamanhototal);
	while (total < tamanhototal){
		nr = recv(this->theSocket, (char*)mensagem, 1000, 0);
		if (nr == -1){
			closesocket(this->theSocket);
			free(mensagem);
			free(mensagemCompleta);
			return NULL;
		}
		total += nr;
		mensagemCompleta = (char*)realloc(mensagemCompleta, total);
		memcpy(mensagemCompleta + total - nr, mensagem, nr);
		memset(mensagem, '\0', 1000);
	}
	free(mensagem);


	//szBuf[nRet]=0;


	if (total == tamanhototal && tamanhototal >= 50){
		cout << "Data received. (" << tamanhototal << ")" << endl;
		return mensagemCompleta;
	}
	else{
		closesocket(this->theSocket);
		free(mensagemCompleta);
		return NULL;
	}


}

void Sockets::DefineIp(char *Ip){
	this->Ip = Ip;
}

void Sockets::DefinePort(short Port){
	this->Port = Port;
}

void Sockets::Timeout(int sec){

    int msec = sec*1000;
	struct timeval timeout;
	timeout.tv_sec = msec;
	timeout.tv_usec = 0;

	if (setsockopt(this->theSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == 0){
		cout << "Timeout receive set: " << sec << " sec" << endl;
	}
	else {
		cout << "Error on timeout! - Receive!" << endl;
	}

	if (setsockopt(this->theSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == 0){
		cout << "Timeout send set: " << sec << " sec" << endl;
	}
	else {
		cout << "Error on timeout - Send!" << endl;
	}

}

#endif
//-----------------------------------------------------------------------------
#ifndef _WIN32

Sockets::Sockets()
{
	//ctor
}

Sockets::~Sockets()
{
	close(this->new_socket);
	//dtor
}

int Sockets::CreateSocket(){
    	//Create socket
	this->socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socket_desc == -1)
	{
		printf("Could not create socket");
		return -1;
	}
}

void Sockets::CloseConection(){
	close(this->socket_desc);
	close(this->new_socket);
}

void Sockets::Timeout(int sec){

	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = 0;

	if (setsockopt(this->new_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == 0){
		//cout << "Timeout receive set: " << sec << " sec" << endl;
		printf("Timeout receive set: %d \n", sec);
	}
	else {
		//cout << "Error on timeout! - Receive!" << endl;
		printf("Error on timeout! - Receive!");
	}

	if (setsockopt(this->new_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == 0){
		//cout << "Timeout send set: " << sec << " sec" << endl;
		printf("Timeout send set: %d \n", sec);
	}
	else {
		//cout << "Error on timeout - Send!" << endl;
		printf("Error on timeout! - Send!");
	}

}

int Sockets::ListenConnection(){
	close(this->new_socket);

	//char *message;

	int enable = 1;
	if (setsockopt(this->socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
		puts("REUSEADDR ERROR");
		return -1;
	}
	//Prepare the sockaddr_in structure
	this->server.sin_family = AF_INET;
	this->server.sin_addr.s_addr = INADDR_ANY;
	this->server.sin_port = htons(this->Port);

	int status = fcntl(this->socket_desc, F_SETFL, fcntl(this->socket_desc, F_GETFL, 0) | O_NONBLOCK);

	if (status == -1){
		perror("calling fcntl");
		// handle the error.  By the way, I've never seen fcntl fail in this way
	}

	//Bind
	if (bind(this->socket_desc, (struct sockaddr *)&this->server, sizeof(this->server)) < 0)
	{
		puts("bind failed");
		return -1;
	}
	puts("bind done");

	//Listen
	listen(this->socket_desc, 3);

	puts("Waiting for incoming connections...");
	this->c = sizeof(struct sockaddr_in);
	return 0;

}

int Sockets::TestConnection(){
	this->new_socket = accept(this->socket_desc, (struct sockaddr *)&this->client, (socklen_t*)&this->c);

	if (this->new_socket<0)
	{
		perror("accept failed");
		return -1;
	}

	puts("Connection accepted");
	return 0;
}

int Sockets::WaitConnection(){
	close(this->new_socket);

	//char *message;

	//Create socket

	int enable = 1;
	if (setsockopt(this->socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
		puts("REUSEADDR ERROR");
		return -1;
	}
	//Prepare the sockaddr_in structure
	this->server.sin_family = AF_INET;
	this->server.sin_addr.s_addr = INADDR_ANY;
	this->server.sin_port = htons(this->Port);

	//Bind
	if (bind(this->socket_desc, (struct sockaddr *)&this->server, sizeof(this->server)) < 0)
	{
		puts("bind failed");
		return -1;
	}
	puts("bind done");

	//Listen
	listen(this->socket_desc, 3);

	puts("Waiting for incoming connections...");
	this->c = sizeof(struct sockaddr_in);

	//while(1){
	this->new_socket = accept(this->socket_desc, (struct sockaddr *)&this->client, (socklen_t*)&this->c);

	if (this->new_socket<0)
	{
		perror("accept failed");
		return -1;
	}

	puts("Connection accepted");
	return 0;
}

void Sockets::DefinePort(short Port){
	this->Port = Port;
}

int Sockets::Send(char *Info, int tamanho){

	int Erro = write(this->new_socket, Info, tamanho);

	//Reply to the client
	//write(new_socket , mensagemCompleta, tamanho);
	if (Erro == -1){
		this->CloseConection();
	}
	//cout << "Mensagem Enviada. (" << tamanho << ")" << endl;
	printf("Mensagem Enviada. (%d) \n", tamanho);

	//break;
	return Erro;
}

char* Sockets::Receive(){

	char* mensagem = (char*)malloc(1000);
	memset(mensagem, '\0', 1000);
	char* mensagemCompleta = NULL;

	int total = 0;
	int tamanhototal = 0;

	int nr = read(this->new_socket, mensagem, 1000);

	if (nr == -1){
		this->CloseConection();
		free(mensagem);
		return NULL;
	}

	total += nr;

	mensagemCompleta = (char*)realloc(mensagemCompleta, total);
	//memset(mensagemCompleta,'\0', total);

	memcpy(mensagemCompleta + total - nr, mensagem, nr);

	char aux[20];
	memset(aux, '\0', sizeof(aux));
	memcpy(aux, mensagem, 10);
	sscanf(aux, "%d", &tamanhototal);

	while (total < tamanhototal){
		nr = read(this->new_socket, mensagem, 1000);
		if (nr == -1){
			this->CloseConection();
			free(mensagem);
			free(mensagemCompleta);
			return NULL;
		}
		total += nr;
		mensagemCompleta = (char*)realloc(mensagemCompleta, total);
		memcpy(mensagemCompleta + total - nr, mensagem, nr);
		memset(mensagem, '\0', 1000);
	}

	free(mensagem);
	if (total == tamanhototal && tamanhototal >= 50){
		//cout << "Mensagem Recebida. (" << tamanhototal << ")" << endl;
		printf("Mensagem Recebida. (%d) \n", tamanhototal);
		return mensagemCompleta;
	}
	else{
		this->CloseConection();
		free(mensagemCompleta);
		return NULL;
	}

}

void Sockets::getIp(){
	getifaddrs(&this->ifAddrStruct);

	for (this->ifa = this->ifAddrStruct; this->ifa != NULL; this->ifa = this->ifa->ifa_next) {
		if (!this->ifa->ifa_addr) {
			continue;
		}
		if (this->ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			this->tmpAddrPtr = &((struct sockaddr_in *)this->ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, this->tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			printf("%s IP Address %s\n", this->ifa->ifa_name, addressBuffer);
		}
		else if (this->ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr = &((struct sockaddr_in6 *)this->ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, this->tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			printf("%s IP Address %s\n", this->ifa->ifa_name, addressBuffer);
		}
	}
	if (this->ifAddrStruct != NULL) freeifaddrs(this->ifAddrStruct);
}

bool Sockets::testIp(char *Ip){
	bool Teste = false;
	getifaddrs(&this->ifAddrStruct);

	for (this->ifa = this->ifAddrStruct; this->ifa != NULL; this->ifa = this->ifa->ifa_next) {
		if (!this->ifa->ifa_addr) {
			continue;
		}
		if (this->ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			this->tmpAddrPtr = &((struct sockaddr_in *)this->ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, this->tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			//printf("%s IP Address %s\n", this->ifa->ifa_name, addressBuffer);

			if(strcmp(Ip,addressBuffer) == 0){
				Teste = true;
			}
		}
		else if (this->ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr = &((struct sockaddr_in6 *)this->ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, this->tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			//printf("%s IP Address %s\n", this->ifa->ifa_name, addressBuffer);
		}
	}
	if (this->ifAddrStruct != NULL) freeifaddrs(this->ifAddrStruct);
	return Teste;
}

#endif
