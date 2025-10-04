//#include "LKZ/Core/Server/LinuxServer.h"
//
//LinuxServer::LinuxServer(int port) : port(port), sockfd(-1), epollfd(-1) {}
//
//LinuxServer::~LinuxServer() {
//    close(sockfd);
//    close(epollfd);
//}
//
//void LinuxServer::Start() {
//    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//
//    sockaddr_in serverAddr{};
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_addr.s_addr = INADDR_ANY;
//    serverAddr.sin_port = htons(port);
//
//    bind(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr));
//
//    epollfd = epoll_create1(0);
//
//    std::cout << "Linux server started on port " << port << "\n";
//}
//
//void LinuxServer::Send(const sockaddr_in& clientAddr, const std::vector<uint8_t>& buffer) {
//    sendto(sockfd, buffer.data(), buffer.size(), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
//}
//
//void LinuxServer::Poll() {
//    // Epoll-based polling here
//}
