#include "tcpFunc.h"
#include <stdio.h>
//初期設定
/*------------------------------------------------------
initTCPServ
*/
//サーバー側初期化

int initTCPServ(TCPDataT *tcpdata)
{
	WORD VersionRequired;
	VersionRequired = MAKEWORD(2, 2);
	if(WSAStartup(VersionRequired, &tcpdata->WSAData) != 0)
	{
		printf("WSAStartup err\n");
		return -1;
	}

	tcpdata->sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tcpdata->sock == INVALID_SOCKET)
	{
		printf("socket err\n");
		return -1;
	}

	tcpdata->server = true;

	memset(&tcpdata->addr, 0, sizeof(tcpdata->addr));
	tcpdata->addr.sin_family = AF_INET;
	tcpdata->addr.sin_addr.s_addr = htonl(INADDR_ANY);
	tcpdata->addr.sin_port = htons(PORTNUM);
	bind(tcpdata->sock, (struct sockaddr *)&tcpdata->addr, sizeof(tcpdata->addr));

	return 0;
}
/*------------------------------------------------------
ConnectTCPServ
*/
//サーバー側接続待ち
int ConnectTCPServ(TCPDataT *tcpdata)
{
	printf("クライアントの接続を待っています\n");
	if(listen(tcpdata->sock, 5) != 0)
	{
		printf("listen err\n");
		return -1;
	}

	int len = sizeof(tcpdata->client);
	tcpdata->newSock = accept(tcpdata->sock, (struct sockaddr *)&tcpdata->client, &len);
	closesocket(tcpdata->sock);
	if(tcpdata->newSock == INVALID_SOCKET)
	{
		printf("new socket err\n");
		return -1;
	}
	printf("クライアントとの接続が完了しました\n");
	return 0;
}
/*------------------------------------------------------
initTCPClient
*/
//クライアント側初期化
int initTCPClient(TCPDataT *tcpdata, char *hostname)
{
	WORD VersionRequired;
	VersionRequired = MAKEWORD(2, 2);
	if(WSAStartup(VersionRequired, &tcpdata->WSAData) != 0)
	{
		printf("WSAStartup err\n");
		return -1;
	}

	tcpdata->sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tcpdata->sock == INVALID_SOCKET)
	{
		printf("socket err\n");
		return -1;
	}

	tcpdata->server = false;

	memset(&tcpdata->addr, 0, sizeof(tcpdata->addr));
	tcpdata->addr.sin_family = AF_INET;
	tcpdata->addr.sin_addr.s_addr = inet_addr(hostname);
	tcpdata->addr.sin_port = htons(PORTNUM);
	return 0;
}
/*------------------------------------------------------
ConnectTCPClient
*/
//クライアント側接続待ち
int ConnectTCPClient(TCPDataT *tcpdata)
{
	printf("サーバーに接続します\n");
	int ret;
	for(int i = 0;i < 10;i++)
	{
		ret = connect(tcpdata->sock, (struct sockaddr *)&tcpdata->addr, sizeof(tcpdata->addr));
		if(ret == 0)
			break;
		printf("接続できません。再接続します\n");
		Sleep(2000);//2秒待つ
	}
	if(ret != 0)
	{
		printf("connect err\n");
		closesocket(tcpdata->sock);
		WSACleanup();
		return -1;
	}
	printf("サーバーに接続できました\n");
	return 0;
}
/*------------------------------------------------------
TCPSend
*/
//送信する

int TCPSend(TCPDataT *tcpdata)
{
	int ret = 0;
	int total = 0;

	if(tcpdata->v_flag)printf("送信開始\n");

	if( tcpdata->server ){
		do{
			ret = sendto(tcpdata->newSock, (char *)tcpdata->data_send, tcpdata->datasize_send, 0, 
				(struct sockaddr *)&tcpdata->addr, sizeof(tcpdata->addr));

			if( ret == SOCKET_ERROR )
			{
				perror( "TCPSend err" );
				return -1;
			}
			if(tcpdata->v_flag) printf("sent size = %d\n", ret );
			total += ret;
		}
		while( total < tcpdata->datasize_send );

	}
	else{

		do{
			ret = sendto( tcpdata->sock, (char *)tcpdata->data_send, tcpdata->datasize_send, 0, 
				(struct sockaddr *)&tcpdata->addr, sizeof(tcpdata->addr) );

			if( ret == SOCKET_ERROR )
			{
				perror( "TCPSend err" );
				return -1;
			}
			if(tcpdata->v_flag) printf("sent size = %d\n", ret );
			total += ret;
			if( total < tcpdata->datasize_send && tcpdata->v_flag)
				printf("SendLESS\n");
		}
		while( total < tcpdata->datasize_send );

	}

	return 0;
}

/*------------------------------------------------------
TCPRecv
*/
//受信する
int TCPRecv(TCPDataT *tcpdata)
{
	int n = sizeof(tcpdata->addr);
	int ret = 0;
	int total = 0;

	static int loop_count = 0;

	if( tcpdata->v_flag) printf( "LOOP COUNT: %d\n", loop_count ); 

	if(tcpdata->v_flag) printf("受信を待っています\n");
	//
	if( tcpdata->server ){
		do{
			ret = recvfrom( tcpdata->newSock, (char*)tcpdata->data_recv,
				tcpdata->datasize_recv
				, 0, (struct sockaddr *)&tcpdata->addr, &n);

			if( ret == SOCKET_ERROR ){
				perror( "TCPRecv" );
				return -1;
			}
			if(tcpdata->v_flag) printf("recv size = %d\n", ret );
			total += ret;
			if( total < tcpdata->datasize_recv && tcpdata->v_flag)
				printf("RecvLESS\n");
		}
		while( total < tcpdata->datasize_recv );
	}
	else{
		do{
			ret = recvfrom(tcpdata->sock, &((char *)tcpdata->data_recv)[total], 
				tcpdata->datasize_recv-total, 0, (struct sockaddr *)&tcpdata->addr, &n);

			if( ret == SOCKET_ERROR ){
				perror( "TCPRecv" );
				return -1;
			}
			if(tcpdata->v_flag) printf("recv size = %d\n", ret );
			total += ret;
			if( total < tcpdata->datasize_recv && tcpdata->v_flag)
				printf("RecvLESS\n");
		}
		while( total < tcpdata->datasize_recv );
	}

	loop_count++;
	return 0;
}

void termTCP(SOCKET *sock)
{
	closesocket(*sock);
	WSACleanup();
	return;
}

