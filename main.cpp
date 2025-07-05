#include "main.h"



void Ping::Send_ping(ULONG target_ip) const
{
  char send_data[] = "Ping ping ping ...";
  char reply_buffer[sizeof(ICMP_ECHO_REPLY) + sizeof(send_data) + 8];

  const DWORD reply_size = IcmpSendEcho(
    icmp_handle,
    target_ip,
    send_data,
    sizeof(send_data),
    nullptr,
    reply_buffer,
    sizeof(reply_buffer),
    1000
  );

  if (reply_size > 0) {
    const auto echo_reply = reinterpret_cast <PICMP_ECHO_REPLY>(reply_buffer);

    std::cout << "Response from "
      << inet_ntoa(*reinterpret_cast <struct in_addr *>(&echo_reply->Address))
      << ": time = " << echo_reply->RoundTripTime << " ms"
    << std::endl;
  }
  else {
    std::cout << "Failed to send ping" << std::endl;
  };
};


std::string Ping::Get_mac_adress(ULONG target_ip) const
{
  ULONG mac_addr[2];
  ULONG mac_addr_len = 6;

  if (const DWORD result = SendARP(target_ip, 0, mac_addr, &mac_addr_len);
      result == NO_ERROR && mac_addr_len == 6
  ) {
    const unsigned char * mac_bytes = reinterpret_cast <unsigned char *>(mac_addr);
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');
    for (int i = 0; i < mac_addr_len; ++i) {
      if (i > 0) {
        oss << ":";
      };

      oss << std::setw(2) << static_cast <int>(mac_bytes[i]);
    };

    return oss.str();
  };

  return "";
};


void Ping::Resolve_and_ping(const std::string & target) const
{
  struct addrinfo Hints = {};
  struct addrinfo * Result = nullptr;

  Hints.ai_family = AF_INET;
  Hints.ai_socktype = SOCK_RAW;
  Hints.ai_protocol = IPPROTO_ICMP;

  if (const int result = getaddrinfo(target.c_str(), nullptr, &Hints, &Result);
      result != 0 || !Result
  ) {
    std::cerr << "getaddrinfo() failed: " << gai_strerror(result)
    << "(" << target << ")" << std::endl;

    return;
  };

  auto ipv4 = reinterpret_cast <sockaddr_in *>(Result->ai_addr);
  ULONG target_ip = ipv4->sin_addr.s_addr;

  char ip_str[INET_ADDRSTRLEN] = {};
  inet_ntop(AF_INET, &target_ip, ip_str, INET_ADDRSTRLEN);

  std::cout << "Ping " << target << "(" << ip_str << ")" << std::endl;

  for (int i = 0; i < 3; ++i) {
    Send_ping(target_ip);

    if (std::string mac = Get_mac_adress(target_ip);
        !mac.empty()
    ) {
      std::cout << "MAC adress: " << mac << std::endl;
    };

    Sleep(100);
  };

  freeaddrinfo(Result);
};


Ping::Ping() : icmp_handle(nullptr)
{
  WSADATA wsa_data;
  WSAStartup(MAKEWORD(2, 2), &wsa_data);
  icmp_handle = IcmpCreateFile();
};

Ping::~Ping()
{
  if (icmp_handle) {
    IcmpCloseHandle(icmp_handle);
  };

  WSACleanup();
};


void Ping::Do_ping(const std::string & target) const
{
  Resolve_and_ping(target);
};


int main(int argc, char *argv[])
{
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <IP Address>" << std::endl;

    return 1;
  };

  Ping ping;
  ping.Do_ping(argv[1]);

  return 0;
};