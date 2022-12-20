
#if defined WIN32
#include <winsock.h>

#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ECHOMAX 255
#define DEFAULTPORT 21234 //porta default

//pulizia buf
void pulbuf(char buf[],int size){
	int i;
	for(i=0;i<size;i++)
		buf[i]='\0';
}

//stampa errore
void errorMsg(char *msg){
	printf("%s\n",msg);
}


void cleanWinsock(){
	#if defined WIN32
		WSACleanup();
	#endif
}

int main(void) {

	#if defined WIN32
		system("cls");
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2,2),&wsaData);
		if(iResult!=0){
			errorMsg("Errore Startup");
			return EXIT_FAILURE;

		}
	#else
		system("clear");
	#endif
	

	int SSocket;
	struct sockaddr_in servAddr;
	struct sockaddr_in clientAddr;
	unsigned int clientAddrL;
	char buf[ECHOMAX];
	char msgInv[ECHOMAX];
	int lenMsg;

	//creazione socket
	SSocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(SSocket<0){
		errorMsg("errore socket");
		closesocket(SSocket);
		return EXIT_FAILURE;
	}else printf("Socket creata correttamente... \n\n\n");

	//creazione indirizzo server
	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_port=htons(DEFAULTPORT);
	servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");//server presente su localhost
	
	//collegamento con socket
	int resultBind = bind(SSocket,(struct sockaddr *) &servAddr, sizeof(servAddr));
	if(resultBind<0){
		errorMsg("bind() error");
	}
	//ciclo per server attivo
	struct hostent *clientName;
	int contavocali=0;

	
	while(1)
	{
	
		pulbuf(buf,ECHOMAX);
		//ricezione frase iniziale
		clientAddrL=sizeof(clientAddr);
		lenMsg=recvfrom(SSocket,buf,ECHOMAX,0,(struct sockaddr*) &clientAddr,&clientAddrL);
		
		
		//invio ok
		strcpy(msgInv,"OK");

		//risoluzione nome
		clientName=gethostbyaddr(&clientAddr.sin_addr,sizeof(clientAddr.sin_addr),PF_INET);
		printf("Messaggio ricevuto dal client %s -> %s\n\n\n",clientName->h_name,buf);
		if(sendto(SSocket,msgInv,strlen(msgInv),0,(struct sockaddr*)&clientAddr,sizeof(clientAddr))!=strlen(msgInv)){
			errorMsg("invio ok non riuscito");
		}
		
		
		
		
		//numero vocali
		pulbuf(buf,ECHOMAX);
		clientAddrL=sizeof(clientAddr);
		lenMsg=recvfrom(SSocket,buf,ECHOMAX,0,(struct sockaddr*) &clientAddr,&clientAddrL);
		
		
		//conversione ad intero
		contavocali=atoi(buf);
		
        int i;
        int j;
		for(i=0;i<contavocali;i++)
		
		{
			
			pulbuf(buf,ECHOMAX);
			pulbuf(msgInv,ECHOMAX);
			clientAddrL=sizeof(clientAddr);


			//ricezione
			lenMsg=recvfrom(SSocket,buf,ECHOMAX,0,(struct sockaddr*) &clientAddr,&clientAddrL);
			buf[lenMsg]='\0';

			//trasformazione vocali
			
			for(j=0;j<lenMsg;j++){
				buf[j]=toupper(buf[j]);
			}

			//rinvio vocali
		
			strcpy(msgInv,buf);
			if(sendto(SSocket,msgInv,strlen(msgInv),0,(struct sockaddr*)&clientAddr,sizeof(clientAddr))!=strlen(msgInv)){
						errorMsg("Errore invio stringa");
					}else printf("Reinvio vocale aggiornata: %s \n",buf);
					

		}
	}
	closesocket(SSocket);
	cleanWinsock();
	

	return 0;
}
