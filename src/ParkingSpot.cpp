#include "../include/ParkingSpot.h"

#include <cmath>
#include <ctime>
#include <iostream>

ParkingSpot::ParkingSpot(int inputid, float inputsize)
    : id(inputid), size(inputsize), occupied(false) {}

bool ParkingSpot::park(const std::string& vehiclenumber) {
  if (occupied == false) {
    occupied = true;
    VehicleNumber = vehiclenumber;
    entryTime = std::time(nullptr);
    return true;
  }
  return false;
}

double ParkingSpot::leave() {
  if (occupied == true) {
    occupied = false;
    VehicleNumber = "";
    double seconds = std::difftime(std::time(nullptr), entryTime);
    if (seconds < 1.0) {
      seconds = 1.0;
    }
    double hours = seconds / 60.0;
    return std::ceil(hours);
  }
  throw std::logic_error("The spot " + std::to_string(id) +
                         " is already empty");
}

int ParkingSpot::getId() const { return id; }

float ParkingSpot::getSize() const { return size; }

bool ParkingSpot::isOccupied() const { return occupied; }

const std::string& ParkingSpot::getVehicleNumber() const {
  return VehicleNumber;
}