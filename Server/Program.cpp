#include <iostream>
#include "Server.h"

int main()
{
    // ANSI escape code for blue text
    std::cout << "\033[34m";

    // ASCII Art copied from the C# string array
    std::cout << R"(
                                                       _    _  __ ____           
                                                      | |  | |/ /|_  /           
                                                      | |__| ' <  / /            
                                           _  _ ___ __|____|_|\_\/___|  ___ _  __
                                          | \| | __|_   _\ \    / / _ \| _ \ |/ /
                                          | .` | _|  | |  \ \/\/ / (_) |   / ' < 
                                          |_|\_|___| |_|   \_/\_/ \___/|_|_\_|\_\
                                         
                                         
                                         
    )" << std::endl;

    // Reset color to default
    std::cout << "\033[0m";

    // Start the server
    Server::Start();
}
