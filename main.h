#ifndef MAIN_H
#define MAIN_H



#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

class Ping
{
  private:
    HANDLE icmp_handle;


    void Send_ping(ULONG target_ip) const;

    std::string Get_mac_adress(ULONG target_ip) const;

    void Resolve_and_ping(const std::string & target) const;


  public:
    Ping();

    ~Ping();


    void Do_ping(const std::string & target) const;
};


int main(int argc, char *argv[]);



#endif //MAIN_H