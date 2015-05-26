#pragma once

#include <vector>
#include <atlstr.h>
#include <atlsocket.h>
using std::vector;

#define   RECV_BUF_LEN 1024 
#define  IP_LEN  20//�������ݵķ�����IP���鳤��


class CFTP{
 
	public:
	CFTP();
	~CFTP(void);

	protected:
	SOCKET m_ControlSocket;  //����Socket
	SOCKET m_DataSocket;     //����Socket

    CString data_ip;  //PASV����ip��ַ
	int data_portal;  //PASV���صĶ˿ں�
	char szPathCmd[MAX_PATH + 7];
	bool IsRecvAlready;//�����Ƿ�ִ�����
    char m_szBuf[RECV_BUF_LEN];//��������ظ�����


	protected:
	bool  SendControl(const char * pszData);//����������������
	bool  RecvControl(const char * pszResult);//��������ظ�


	public:
	bool InitFtp( char *sIP,int iPort,int nTimeOut, char *sUsr, char *sPwd);//��ʼ��������Ftp������
	bool CreateControlSocket(char *sIP,u_short nServerPort , int nTimeOut);//���������׽��ֲ����ӷ�����
	bool CreateDataSocket();//���������׽��� �����ӷ�����
	bool SendUSER(char * pszUserName);//�����û��� �ɹ�������
	bool SendPASS(char * pszPassWord);//��������  �ɹ�������
	int progress; //����

	bool CFTP::SavePasvStr();//�л�ΪPASVģʽ,���һ�ȡPASV���ص�IP��˿ں� 

	bool SendAndRecv(const char * pszCMD,char * pszResult = NULL );//��������ж��Ƿ�ɹ�ִ��


	bool SendData(char *pszData,int nLength);//���͵����ݳ���

	bool UpLoad(char *pszUpName,char * pszSave,char *pszPath);//�ϴ��ļ�

	bool ReadAndSendFile(char *pszUpName,char * pszSave,char *pszPath,bool flag);//�ϴ��ֽ�


	int  GetLocalFileSize(char * pszFile);//��ȡ�����ļ���С

	void  CloseCtrlSocket();
	void CloseDataSocket();

	bool getsocket();//�ж�socket�Ƿ�����

};