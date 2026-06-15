#include <SFML/Network.hpp>
#include <iostream>

int main() {
    std::string serverIp;
    int port = 8080;

    std::cout << "=== PARKING CLIENT ===\n";
    std::cout << "Enter server IP: ";
    std::getline(std::cin, serverIp);

    sf::TcpSocket socket;
    if (socket.connect(serverIp, port) != sf::Socket::Done) {
        std::cerr << "Can't connect to server\n";
        return 1;
    }
    std::cout << "Connected to server!\n\n";

    std::cout << "Commands:\n";
    std::cout << "  ENTER <plate> <car|moto|truck>\n";
    std::cout << "  EXIT <plate>\n";
    std::cout << "  STATUS\n";
    std::cout << "  QUIT\n\n";

    std::string input;
    char buffer[1024];
    std::size_t received;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input.empty()) continue;
        
        input += '\n';
        socket.send(input.c_str(), input.size());

        if (input.find("QUIT") != std::string::npos) {
            break;
        }

        if (socket.receive(buffer, sizeof(buffer), received) == sf::Socket::Done) {
            std::string response(buffer, received);
            std::cout << response;
        } else {
            std::cerr << "Error receiving response\n";
            break;
        }
    }

    std::cout << "Disconnected\n";
    return 0;
}