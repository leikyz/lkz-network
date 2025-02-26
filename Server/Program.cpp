#include <iostream>
#include "Server.h"

int main()
{
    Server::Start();

    Server::Send(1, new CreateClientMessage)
}

