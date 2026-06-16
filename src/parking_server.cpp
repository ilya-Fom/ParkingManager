#include <SFML/Network.hpp>
#include <format>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>
#include <vector>

#include "../include/ParkingLot.h"

void handleClient(std::shared_ptr<sf::TcpSocket> client, ParkingLot& lot) {
  char buffer[1024];
  std::size_t received;

  std::cout << "Client connected (thread: " << std::this_thread::get_id()
            << ")\n";

  while (true) {
    try {
      sf::Socket::Status status =
          client->receive(buffer, sizeof(buffer), received);

      if (status == sf::Socket::Disconnected) {
        std::cout << "Client disconnected (thread: "
                  << std::this_thread::get_id() << ")\n";
        break;
      }

      if (status != sf::Socket::Done) {
        std::cerr << "Receive error (thread: " << std::this_thread::get_id()
                  << ")\n";
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
        if (type == "car")
          size = 1.0f;
        else if (type == "moto")
          size = 0.5f;
        else if (type == "truck")
          size = 2.5f;
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
      } else if (command == "EXIT") {
        std::string number;
        iss >> number;

        float cost = 0;
        float totalRevenue = 0;
        if (lot.ExitVehicle(number, cost, totalRevenue)) {
          response =
              std::format("OK COST {:.0f} REVENUE {:.0f}", cost, totalRevenue);
        } else {
          response = "ERROR Vehicle " + number + " not found";
        }
      } else if (command == "STATUS") {
        response = std::format("STATUS free={} revenue={:.0f}",
                               lot.getFreeSpots(), lot.getRevenue());
      } else if (command == "QUIT") {
        response = "OK BYE";
        client->send(response.c_str(), response.size());
        break;
      } else {
        response = "ERROR Unknown command: " + command;
      }

      response += "\n";
      client->send(response.c_str(), response.size());
    } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
      std::string errorResponse = "ERROR Internal server error\n";
      client->send(errorResponse.c_str(), errorResponse.size());
    }
  }
}

int main() {
  try {
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
    std::cout << "Waiting for clients...\n";
    std::vector<std::thread> threads;

    while (true) {
      auto client = std::make_shared<sf::TcpSocket>();

      if (listener.accept(*client) != sf::Socket::Done) {
        std::cerr << "Error: can't accept client\n";
        continue;
      }
      threads.emplace_back(handleClient, client, std::ref(lot));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}