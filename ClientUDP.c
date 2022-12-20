

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
#define DEFAULTPORT 48000 //porta defaulta

//pulizia buffer
void pulbuf(char buf[],int size)
{
    int k;
	for(k=0;k<size;k++)
		buf[k]='\0';
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


	int CSocket;
	struct sockaddr_in MyAddrs;
	struct sockaddr_in ServAddrs;
	unsigned int recvLen;
	char msgInv[ECHOMAX];
	char msgRic[ECHOMAX];
	char frase[ECHOMAX]; 

	int lungMsgIniziale;
	int lungMsgServer;

	//nome server
	char nameServer[ECHOMAX];
	int portNum;
	printf("Inserire nome server: ");
	scanf("%s",nameServer);
	printf("\n\n");


	
	//numero di porta
	printf("Inserire numero porta: ");
	scanf("%d",&portNum);
	printf("\n\n");
	

	
	
	
	//stringa da inviare al server
	printf("Inserimento messaggio iniziale-> ");
	scanf("%s",msgInv);

	if((lungMsgIniziale = strlen(msgInv))> ECHOMAX){
		errorMsg("errore lunghezza\n");
	}
	
	//creazione della socket
	CSocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(CSocket<0){
		errorMsg("socket non creata correttamente\n");
	}

	//acquisizione indirizzo IP
	struct hostent *DNS;
	DNS = gethostbyname(nameServer);

	//creazione indirizzo server
	memset(&MyAddrs,0,sizeof(MyAddrs));
	MyAddrs.sin_family=PF_INET;
	MyAddrs.sin_port=htons(portNum);
	MyAddrs.sin_addr.s_addr=inet_addr(inet_ntoa(*(struct in_addr*)DNS->h_addr));
	

	//invio della stringa iniziale
	if(sendto(CSocket,msgInv,lungMsgIniziale,0,(struct sockaddr*)&MyAddrs,sizeof(MyAddrs))!=lungMsgIniziale){
		errorMsg("stringa non inviata correttamente\n");
		closesocket(CSocket);
		cleanWinsock();
	}
	pulbuf(msgInv,ECHOMAX);
	
	//ricezione stringa invio
	recvLen = sizeof(ServAddrs);
	lungMsgServer = recvfrom(CSocket,msgRic,ECHOMAX,0,(struct sockaddr*)&ServAddrs,&recvLen);
	if(MyAddrs.sin_addr.s_addr != ServAddrs.sin_addr.s_addr){
		errorMsg("errore nella ricezione\n");
	}
	msgRic[lungMsgServer]='\0';
	printf("Risposta server = %s\n\n\n",msgRic);
	
	

	int contavocali=0;
	char strV[ECHOMAX];
	char vocaliTrovate[ECHOMAX];

	do{
		//stringa input per vocali
		printf("Inserisci stringa da controllare: ");
		scanf("%s",frase);
		printf("\n\n");
		
		
		
		//conta vocali
		int i;
		for(i=0;i<strlen(frase);i++){
			if(toupper(frase[i])=='A' || toupper(frase[i])=='E' || toupper(frase[i])=='I' || toupper(frase[i])=='O' || toupper(frase[i])=='U'){
				vocaliTrovate[contavocali]=frase[i];
				contavocali++;
			}
		}
		
	}while(contavocali==0);

	//stampa maiuscole
	printf("Vocali trovate nella stringa: ");
	int i;
	for(i=0;i<contavocali;i++){
		if(i==contavocali-1)
			printf("%c\n\n",vocaliTrovate[i]);
		else
			printf("%c,",vocaliTrovate[i]);
	}

	//conversione da intero a stringa
	itoa(contavocali,strV,10);

	//invio numero vocali
	if(sendto(CSocket,strV,strlen(strV),0,(struct sockaddr*)&MyAddrs,sizeof(MyAddrs))!=strlen(strV)){
			errorMsg("errore invio");
			closesocket(CSocket);
			cleanWinsock();
		}else printf("invio del numero delle vocali al server in corso...\n\n\n\n\n");
	
    
	for(i=0;i<contavocali;i++)
	{
		pulbuf(msgInv,ECHOMAX);
		msgInv[0]=vocaliTrovate[i];
		msgInv[1]='\0';

		if(sendto(CSocket,msgInv,strlen(msgInv),0,(struct sockaddr*)&MyAddrs,sizeof(MyAddrs))!=strlen(msgInv)){
			errorMsg("errore invio stringa\n");
			closesocket(CSocket);
			cleanWinsock();
		}

	//ricezione vocali upper
		pulbuf(msgRic,ECHOMAX);
		recvLen = sizeof(ServAddrs);
		lungMsgServer = recvfrom(CSocket,msgRic,ECHOMAX,0,(struct sockaddr*)&ServAddrs,&recvLen);
		if(MyAddrs.sin_addr.s_addr != ServAddrs.sin_addr.s_addr){
			errorMsg("errore ricezione\n");
		}
		msgRic[lungMsgServer]='\0';
		printf("Vocale Maiuscola Aggiornata: %s\n",msgRic);
	}

	//chiusura sock
	closesocket(CSocket);
	cleanWinsock();
	return 0;
}
