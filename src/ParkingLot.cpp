#include "../include/ParkingLot.h"

#include <fstream>
#include <iostream>

ParkingLot::ParkingLot(const std::string& path, const int price)
    : hourlyRate(price), revenue(0.0f) {
  if (LoadSpotsFromFile(path) == false) {
    throw std::runtime_error("File isn`t available");
  }
}

bool ParkingLot::LoadSpotsFromFile(const std::string& path) {
  std::ifstream file(path);
  float element;
  int num = 1;
  if (file.is_open()) {
    while (file >> element) {
      ParkingSpot new_spot(num, element);
      num++;
      Spots.push_back(new_spot);
    }
    file.close();
    if (std::size(Spots) > 0) {
      return true;
    }
  }
  return false;
}

bool ParkingLot::EnterVehicle(const std::string& vehicleNumber,
                              float vehicleSize, int& SpotId) {
  std::lock_guard<std::mutex> lock(mtx);
  if (isParked(vehicleNumber) == false) {
    for (ParkingSpot& spot : Spots) {
      if (spot.isOccupied() == false && spot.getSize() >= vehicleSize) {
        spot.park(vehicleNumber);
        SpotId = spot.getId();
        return true;
      }
    }
  }
  return false;
}

bool ParkingLot::ExitVehicle(const std::string& vehicleNumber, float& cost,
                             float& totalRevenue, double& hours) {
  std::lock_guard<std::mutex> lock(mtx);
  for (ParkingSpot& spot : Spots) {
    if (spot.isOccupied() == true && spot.getVehicleNumber() == vehicleNumber) {
      try {
        hours = spot.leave();
        cost = static_cast<float>(hours * hourlyRate);
        revenue += cost;
        totalRevenue = revenue;
        return true;
      } catch (std::logic_error& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return false;
      }
    }
  }
  return false;
}

int ParkingLot::getFreeSpots() const {
  std::lock_guard<std::mutex> lock(mtx);
  int freeSpots = 0;
  for (const ParkingSpot& spot : Spots) {
    if (spot.isOccupied() == false) {
      freeSpots++;
    }
  }
  return freeSpots;
}

float ParkingLot::getRevenue() const {
  std::lock_guard<std::mutex> lock(mtx);
  return revenue;
}

bool ParkingLot::isParked(const std::string& vehicleNumber) const {
  // std::lock_guard<std::mutex> lock(mtx);
  for (const ParkingSpot& spot : Spots) {
    if (spot.isOccupied() == true && spot.getVehicleNumber() == vehicleNumber) {
      return true;
    }
  }
  return false;
}