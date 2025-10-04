#include <iostream>
#include "LKZ/Core/Server.h"
#include "LKZ/Manager/MatchmakingManager.h"
#include <thread>
#include <LKZ/Core/Logger.h>

int main()
{
	Server::Initialize();
    Server::Start();
}
