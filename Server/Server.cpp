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

        // Convertir les données reçues en vecteur de bytes
        std::vector<uint8_t> receivedData(buffer, buffer + bytesReceived);

        // Appel de la méthode de traitement des messages (désérialisation et appel du gestionnaire)
        EventManager::processMessage(receivedData);
    }
    closesocket(serverSocket);
    WSACleanup();
}

void Server::Send(int clientId, Message& message)
{
    // Trouver le client dans le ClientManager en utilisant son clientId
    std::shared_ptr<Client> client = ClientManager::getClientById(clientId);
    if (client)
    {
        // Récupérer le socket du client
        SOCKET clientSocket = client->socket;
        sockaddr_in clientAddr = {};  // Définir l'adresse du client (tu peux la récupérer si nécessaire)
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = htons(5555);  // Remplace avec le port approprié

        // Conversion de l'adresse IP du client en format binaire
        if (inet_pton(AF_INET, client->ipAddress.c_str(), &clientAddr.sin_addr) != 1) {
            std::cerr << "Erreur de conversion de l'adresse IP du client" << std::endl;
            return;
        }

        // Créer un buffer local pour la sérialisation du message
        std::vector<uint8_t> buffer;

        // Sérialisation du message dans le buffer
        std::vector<uint8_t> data = message.serialize(buffer);

        // Envoi du message via UDP
        int bytesSent = sendto(clientSocket, reinterpret_cast<const char*>(data.data()), data.size(), 0,
            reinterpret_cast<const sockaddr*>(&clientAddr), sizeof(clientAddr));
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Erreur lors de l'envoi du message: " << WSAGetLastError() << std::endl;
        }
        else {
            std::cout << "Message envoyé à client ID " << clientId << std::endl;
        }
    }
    else {
        std::cerr << "Client avec ID " << clientId << " non trouvé." << std::endl;
    }
}



