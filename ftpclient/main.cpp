#include "ftpclient.h"
CFTP *all_ftp;
int mm = 0;

struct THread { std::string path;std::string file; CFTP thread_ftp;};
DWORD WINAPI Fun(LPVOID lpParamter)
  {
	  THread* mythread=(THread*)lpParamter;//获取参数结构体
	  std::string pathname = mythread->path;

	  std::string filename  = mythread->file;


	  char char_pathname[MAX_PATH] = {0};
	  char char_filename[MAX_PATH] = {0};


	  strcpy_s(char_filename, filename.c_str());
	  strcpy_s(char_pathname, pathname.c_str());


	if(mythread->thread_ftp.InitFtp("192.168.134.62",21,20000,"USER root\r\n","PASS 9Ek2zvM0\r\n")){
		   //切换为PASV模式
           if(mythread->thread_ftp.SavePasvStr()){
			   if(mythread->thread_ftp.CreateDataSocket()){
				   mythread->thread_ftp.UpLoad(char_filename,char_pathname,NULL);
				}
		   }     
	}
	
	return 0;
  }


void main()
{
	int size =  3;
	mm = size;
	 all_ftp = new CFTP[size];

	    THread *mythread0=new THread();
        mythread0->path="E://ebook.zip";
		mythread0->file="ebook.zip";
		mythread0->thread_ftp=all_ftp[0];


		 HANDLE hThread0 = CreateThread(NULL, 0, Fun, mythread0, 0, NULL);
		 CloseHandle(hThread0);
		 while(1){
		 }



}