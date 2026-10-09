#ifndef __TCPFUNC_H__
#define __TCPFUNC_H__

#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define PORTNUM 10001

typedef struct{
	WSADATA WSAData;//winsock初期化
	SOCKET sock;//ソケット

	sockaddr_in addr;//アドレスとか

	//サーバ用データ
	sockaddr_in client;
	SOCKET newSock;

	//共通
	void *data_recv;//受信用構造体指定
	void *data_send;//送信用構造体指定

	int datasize_recv;//受信用構造体のデータサイズ
	int datasize_send;//送信用構造体のデータサイズ

	bool v_flag;

	bool server;

}TCPDataT;

//初期設定
int initTCPServ(TCPDataT *tcpdata);//サーバー側初期化
int ConnectTCPServ(TCPDataT *tcpdata);//サーバー側接続待ち
int initTCPClient(TCPDataT *tcpdata, char *hostname);//クライアント側初期化
int ConnectTCPClient(TCPDataT *tcpdata);//クライアント側接続待ち
//通信
int TCPSend(TCPDataT *tcpdata);//データ送信
int TCPRecv(TCPDataT *tcpdata);//データ受信


//終了処理 
void termTCP(SOCKET *sock);

#endif //__TCPFUNC_H__
