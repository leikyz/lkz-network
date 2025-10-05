//#ifndef SERVER_H
//#define SERVER_H
//
//#include <vector>
//#include <winsock2.h> 
//#include "LKZ/Manager/ClientManager.h"
//#include "LKZ/Protocol/Message/Message.h"
//#include "LKZ/Manager/LobbyManager.h" 
//#include "LkZ/Core/Threading/ThreadManager.h"
//#include <LKZ/Core/Log/Logger.h>
//#pragma comment(lib, "Ws2_32.lib")
//
//#define PORT 5555
//#define BUFFER_SIZE 1024
//
//class Server
//{
//private:
//    static SOCKET serverSocket;
//	static ThreadManager threadManager;
//public:
//    static void Start();
//
//    static void Initialize();
//
//    static void Send(const sockaddr_in clientAddr, const std::vector<uint8_t>& buffer);
//
//    static void SendToMultiple(const std::vector<Client*>& clients, const std::vector<uint8_t>& buffer, const sockaddr_in* excludedClientAddr = nullptr);
//
//    static void SendToAllInLobby(Lobby*, const std::vector<uint8_t>& buffer);
//
//    static void SendToAllInLobbyExcept(Lobby*, const sockaddr_in& excludedClientAddr, const std::vector<uint8_t>& buffer);
//
//    static void SendToAllClients(const std::vector<uint8_t>& buffer);
//
//    static void SendToAllClientsExcept(const sockaddr_in& excludedClientAddr, const std::vector<uint8_t>& buffer);
//};
//
//#endif // SERVER_H
