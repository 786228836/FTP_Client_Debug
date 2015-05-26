#include "ftpclient.h"
 
CFTP::CFTP(){
 
	

	m_ControlSocket = INVALID_SOCKET;
	m_DataSocket = INVALID_SOCKET;

	 WSADATA WsaData;                       
     WSAStartup(MAKEWORD(2,0),&WsaData);
	 
	progress = 0;
}

CFTP::~CFTP(void)
{
	 if(m_ControlSocket != INVALID_SOCKET)
		{
			SendAndRecv("QUIT \r\n","");
			closesocket(m_ControlSocket);
		}
		m_ControlSocket = INVALID_SOCKET;

		if(m_DataSocket != INVALID_SOCKET)
		{
			closesocket(m_DataSocket);
		}
		m_DataSocket = INVALID_SOCKET;
	
}

bool CFTP::InitFtp(char *sIP, int iPort,int nTimeOut, char *sUsr, char *sPwd){


	if(!CreateControlSocket(sIP,iPort,nTimeOut)){
	  return false;
	}

	WSADATA WsaData;                       
    WSAStartup(MAKEWORD(2,0),&WsaData);


	if(!SendUSER(sUsr)){
	  return false;
	}

	if(!SendPASS(sPwd)){
	   return false;
	}
  
}

 
bool CFTP::SendControl(const char * pszData)
{
	IsRecvAlready = TRUE;
	if(IsRecvAlready)//如果上一个命令已经接受成功
	{
		if( send(m_ControlSocket,pszData,strlen(pszData),0) == SOCKET_ERROR )
			return FALSE;
	}
	else
	{//如果没有接受完 则等待100毫秒 然后继续发送
		Sleep(100);
		if(IsRecvAlready)//如果上一个命令已经接受成功
		{
			if( send(m_ControlSocket,pszData,strlen(pszData),0) == SOCKET_ERROR )
				return FALSE;
		}
	}
	IsRecvAlready = FALSE;
	return TRUE;
}


bool CFTP::RecvControl(const char * pszResult)
{
	
	memset(m_szBuf,0,RECV_BUF_LEN);

	if( recv(m_ControlSocket,m_szBuf,RECV_BUF_LEN,0) == SOCKET_ERROR)
	{ 
		IsRecvAlready = TRUE;
		return FALSE;
	}
	printf(m_szBuf);
	if(pszResult != NULL)
	{
		if(!strstr(m_szBuf,pszResult))
		{
			IsRecvAlready = TRUE;
			return FALSE;
		}
	}
	IsRecvAlready = TRUE;
	return TRUE;


}

bool  CFTP::SendAndRecv(const char * pszCMD,char * pszResult){

	if(!SendControl(pszCMD))
		return FALSE;
	if(!RecvControl(pszResult))
		return FALSE;
	return TRUE;
}

bool CFTP::CreateControlSocket(char *sIP,u_short nServerPort , int nTimeOut){

    sockaddr_in Server;
	Server.sin_family = AF_INET;
	Server.sin_port = htons(nServerPort);
	Server.sin_addr.S_un.S_addr = inet_addr(sIP);

	m_ControlSocket = socket(AF_INET,SOCK_STREAM,0);

	if(m_ControlSocket == INVALID_SOCKET)
	{
		return FALSE;
	}
	bool bislinger = true;
	//设置直接返回
	setsockopt(m_ControlSocket,SOL_SOCKET,SO_DONTLINGER,(char *)&bislinger,sizeof(bool));
	//发送延迟
	setsockopt(m_ControlSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nTimeOut,sizeof(int));
	//接收延迟
	setsockopt(m_ControlSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nTimeOut,sizeof(int));

	if(connect(m_ControlSocket,(sockaddr *)&Server,sizeof(Server)))
	{
		return FALSE;
	}
	if(!RecvControl("220"))
		return FALSE;
	return TRUE;

}


bool CFTP::SendUSER(char * pszUserName){
 
	if(!SendControl(pszUserName))
		return FALSE;
	if(!RecvControl("331"))
		return FALSE;
	return TRUE;

}

bool CFTP::SendPASS(char * pszPassWord)
{
	if(!SendControl(pszPassWord))
		return FALSE;
	if(!RecvControl("230"))
		return FALSE;
	return TRUE;
}


bool CFTP::SavePasvStr(){

   char m_szPasvString[RECV_BUF_LEN];//被动模式下接受 PASV返回 IP和端口号的字符串

	 //FTP切换为 PASV模式
	 if(!SendAndRecv("PASV \r\n","227"))
	 {
		 return FALSE;
	 }

	  //接受PASV返回的数据端口IP与端口号
	 memset(m_szPasvString,0,RECV_BUF_LEN);
	 memcpy(m_szPasvString,m_szBuf,strlen(m_szBuf));

	 if(m_szPasvString!= NULL){

		char *temp = strchr ( m_szPasvString, '(' );
		const char * split = ",";
		char * temp_split =  strtok(temp,split);

		vector<char *> vec;

		while(temp_split!=NULL){
			vec.push_back(temp_split);
		    temp_split = strtok(NULL,split); 
		} 

		for(int i=0;i<6;i++){
			if(i<4){
				data_ip +=vec[i];
				data_ip +=".";
			}else if(i==4){
				data_portal = atoi(vec[i]) *256;

			}else{
				data_portal += atoi(vec[i]); 
			}
	
		}

		data_ip.Delete(0,1);
		data_ip.Delete(data_ip.GetLength()-1,1);

		return TRUE;
    }else{

	 return FALSE;

	}
}


bool CFTP::CreateDataSocket(){

         const size_t strsize=(data_ip.GetLength()+1)*2; // 宽字符的长度;
          char * pstr= new char[strsize]; //分配空间;
          size_t sz=0;
          wcstombs_s(&sz,pstr,strsize,data_ip,_TRUNCATE);

		 //创建数据端口
		  m_DataSocket = socket(AF_INET,SOCK_STREAM,0);

		  if(m_DataSocket == INVALID_SOCKET){
		     return FALSE;
		  }
		    
		sockaddr_in RecvAddr ;
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port = htons(data_portal);
		RecvAddr.sin_addr.S_un.S_addr = inet_addr(pstr);

		if(!connect(m_DataSocket,(sockaddr *)&RecvAddr,sizeof(sockaddr_in)) == 0 ){
		    return FALSE;
		}

		  return TRUE;

}


bool CFTP::UpLoad(char *pszUpName,char * pszSave,char *pszPath){

	
	 sprintf(szPathCmd,"SIZE %s\r\n",pszUpName);

	 if(!SendAndRecv(szPathCmd,NULL))
		 return FALSE;


	bool falg = (strstr(m_szBuf,"550") || strstr(m_szBuf,"213 0"));

	if(!ReadAndSendFile(pszUpName,pszSave,pszPath,falg))
		return FALSE;

      return TRUE;
     }
	 
 




bool  CFTP::ReadAndSendFile(char *pszUpName,char * pszSave,char *pszPath,bool flag){

	FILE *pFile =  fopen(pszSave,"rb+");
	if(pFile == NULL)
		return FALSE;

	_fseeki64(pFile,0,SEEK_END);
	unsigned long long iLength = _ftelli64(pFile);
	_fseeki64(pFile, 0, SEEK_SET);


	unsigned long long internet_size = 0L;
	

	if(flag){ 
		 sprintf(szPathCmd,"STOR %s\r\n",pszUpName);

	}else{
	
	   //获取本地文件长度
		 char * pszPos =strstr(m_szBuf,"213 ");
		 pszPos += strlen("213 ");
		 internet_size = _atoi64(pszPos);

		
		 if(internet_size != iLength){
		    sprintf(szPathCmd,"APPE %s\r\n",pszUpName); 
		 }else{
			 return TRUE;
		 }
	}

	if(SendAndRecv(szPathCmd,"150") || SendAndRecv(szPathCmd,"213")){

	
	long all_step = 0L;

	unsigned long long upload_length =iLength - internet_size;
	
    if(upload_length%4096){
	  all_step ++;
	}
	all_step += (upload_length/4096);

	long step = internet_size/4096;
    all_step += step;
    
	if(!flag){
	_fseeki64(pFile, internet_size, SEEK_SET);
	}

	int nRead = 0;
	char ReadBuf[4096] = {0};
	
	while ((nRead = fread(ReadBuf,1,4096,pFile)) > 0){
        step++;
		if(step == all_step ){
		progress = 100;
		}else{
		progress  = (((float)step/all_step)*100);   
		}

		
		char *pszData = ReadBuf;
		int nSend = 0;

		
		while (nRead > 0){
			
			pszData += nSend;
			nSend = send(m_DataSocket,pszData,nRead,0);
			if(nSend == -1){
			return FALSE;
			}
			nRead -= nSend;
		 }

		if(nSend == -1){
			  return FALSE;
			}

	  }					   
	
	      
	}else{
	   
		return FALSE;
	
	}


	if(pFile!=NULL){
		fclose(pFile);
	}

	pFile=NULL;


return TRUE;


}

