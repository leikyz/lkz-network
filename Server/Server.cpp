#include "Server.h"

void Server::Start()
{
    EventManager::BindEvents();

    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        printf("Error during the Initialization of Winsock \n");
        return;
    }

    // Create UDP Socket
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Erreur lors de la cr�ation du socket: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Create Server
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET; // IPV4
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT); // Convert port to network format

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("�chec du bind: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    printf("UDP Server listening on port %d...\n", PORT);

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    while (true)
    {
        int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            printf("Message receive error: %d\n", WSAGetLastError());
            break;
        }

        std::vector<uint8_t> receivedData(buffer, buffer + bytesReceived);

        // Vérifier si receivedData contient au moins un octet (ID du message)
        if (!receivedData.empty()) {
            int messageId = static_cast<int>(receivedData[0]);

            // Si le message est un "CreateClientMessage" (ID == 1), ajouter serverSocket
            if (messageId == 1) {
                uintptr_t socketValue = reinterpret_cast<uintptr_t>(socket); // Sérialisation
                uint8_t* socketPtr = reinterpret_cast<uint8_t*>(&socketValue);

                receivedData.insert(receivedData.end(), socketPtr, socketPtr + sizeof(socketValue));
            }
        }

        // Passer les données au gestionnaire d'événements
        EventManager::processMessage(receivedData);
    }



    closesocket(serverSocket);
    WSACleanup();
}

void Server::Send(int clientId, Message& message) {
   /* auto client = ClientManager::getClientById(clientId);
    if (!client) {
        std::cerr << "Client non trouvé pour ID " << clientId << std::endl;
        return;
    }

    std::vector<uint8_t> buffer;
    std::vector<uint8_t> data = message.serialize(buffer);

    int bytesSent = sendto(serverSocket, reinterpret_cast<const char*>(data.data()), data.size(), 0,
        reinterpret_cast<const sockaddr*>(&client->address), sizeof(client->address));

    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Erreur lors de l'envoi du message: " << WSAGetLastError() << std::endl;
    }
    else {
        std::cout << "Message envoyé au client ID " << clientId << std::endl;
    }*/
}



