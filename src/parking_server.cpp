#include <SFML/Network.hpp>
#include <iostream>
#include <sstream>
#include "../include/ParkingLot.h"

int main() {

    std::string configPath = "../config/parking_config.txt";
    int hourlyRate = 100;
    int port = 8080;

    std::cout << "=== PARKING SERVER ===\n";


    ParkingLot lot(configPath, hourlyRate);
    std::cout << "Parking loaded. Free spots: " << lot.getFreeSpots() << "\n";


    sf::TcpListener listener;
    if (listener.listen(port) != sf::Socket::Done) {
        std::cerr << "Error: can't listen on port " << port << "\n";
        return 1;
    }
    std::cout << "Server started on port " << port << "\n";


    while (true) {
        sf::TcpSocket client;
        std::cout << "Waiting for client...\n";

        if (listener.accept(client) != sf::Socket::Done) {
            std::cerr << "Error: can't accept client\n";
            continue;
        }

        std::cout << "Client connected!\n";


        char buffer[1024];
        std::size_t received;


        while (true) {
            sf::Socket::Status status = client.receive(buffer, sizeof(buffer), received);

            if (status == sf::Socket::Disconnected) {
                std::cout << "Client disconnected\n";
                break;
            }

            if (status != sf::Socket::Done) {
                std::cerr << "Receive error\n";
                break;
            }

            std::string request(buffer, received);
            std::cout << "Request: " << request;


            std::string response;
            std::istringstream iss(request);
            std::string command;
            iss >> command;

            if (command == "ENTER") {
                std::string number, type;
                iss >> number >> type;

                float size = 0;
                if (type == "car") size = 1.0f;
                else if (type == "moto") size = 0.5f;
                else if (type == "truck") size = 2.5f;
                else {
                    response = "ERROR Unknown vehicle type: " + type;
                }

                if (response.empty()) {
                    int spotId;
                    if (lot.EnterVehicle(number, size, spotId)) {
                        response = "OK SPOT " + std::to_string(spotId);
                    } else {
                        if (lot.isParked(number))
                            response = "ERROR Vehicle " + number + " already parked";
                        else
                            response = "ERROR No free spot for " + type;
                    }
                }
            }
            else if (command == "EXIT") {
                std::string number;
                iss >> number;

                float cost = 0;
                float totalRevenue = 0;
                if (lot.ExitVehicle(number, cost, totalRevenue)) {
                    response = "OK COST " + std::to_string(cost) + " REVENUE " + std::to_string(totalRevenue);
                } else {
                    response = "ERROR Vehicle " + number + " not found";
                }
            }
            else if (command == "STATUS") {
                response = "STATUS free=" + std::to_string(lot.getFreeSpots()) +
                           " revenue=" + std::to_string(lot.getRevenue());
            }
            else if (command == "QUIT") {
                response = "OK BYE";
                client.send(response.c_str(), response.size());
                break;
            }
            else {
                response = "ERROR Unknown command: " + command;
            }


            response += "\n";
            client.send(response.c_str(), response.size());
        }
    }

    return 0;
}