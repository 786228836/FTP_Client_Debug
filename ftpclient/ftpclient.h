#pragma once

#include <vector>
#include <atlstr.h>
#include <atlsocket.h>
using std::vector;

#define   RECV_BUF_LEN 1024 
#define  IP_LEN  20//发送数据的服务器IP数组长度


class CFTP{
 
	public:
	CFTP();
	~CFTP(void);

	protected:
	SOCKET m_ControlSocket;  //控制Socket
	SOCKET m_DataSocket;     //数据Socket

    CString data_ip;  //PASV返回ip地址
	int data_portal;  //PASV返回的端口号
	char szPathCmd[MAX_PATH + 7];
	bool IsRecvAlready;//命令是否执行完成
    char m_szBuf[RECV_BUF_LEN];//接收命令回复数组


	protected:
	bool  SendControl(const char * pszData);//给服务器发送命令
	bool  RecvControl(const char * pszResult);//接收命令回复


	public:
	bool InitFtp( char *sIP,int iPort,int nTimeOut, char *sUsr, char *sPwd);//初始化并连接Ftp服务器
	bool CreateControlSocket(char *sIP,u_short nServerPort , int nTimeOut);//创建命令套接字并连接服务器
	bool CreateDataSocket();//创建传输套接字 并连接服务器
	bool SendUSER(char * pszUserName);//发送用户名 成功返回真
	bool SendPASS(char * pszPassWord);//发送密码  成功返回真
	int progress; //进度

	bool CFTP::SavePasvStr();//切换为PASV模式,并且获取PASV返回的IP与端口号 

	bool SendAndRecv(const char * pszCMD,char * pszResult = NULL );//发送命令并判断是否成功执行


	bool SendData(char *pszData,int nLength);//发送的数据长度

	bool UpLoad(char *pszUpName,char * pszSave,char *pszPath);//上传文件

	bool ReadAndSendFile(char *pszUpName,char * pszSave,char *pszPath,bool flag);//上传字节


	int  GetLocalFileSize(char * pszFile);//获取本地文件大小

	void  CloseCtrlSocket();
	void CloseDataSocket();

	bool getsocket();//判断socket是否连接

};