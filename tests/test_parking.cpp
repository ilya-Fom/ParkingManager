#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <chrono>
#include <fstream>
#include <thread>

#include "../include/ParkingLot.h"
#include "doctest.h"

void createTestConfig(const std::string& filename, const std::string& content) {
  std::ofstream file(filename);
  file << content;
  file.close();
}

void deleteTestConfig(const std::string& filename) {
  std::remove(filename.c_str());
}

TEST_CASE("ParkingSpot - create and getters") {
  ParkingSpot spot(5, 2.5f);

  CHECK(spot.getId() == 5);
  CHECK(spot.getSize() == 2.5f);
  CHECK(spot.isOccupied() == false);
  CHECK(spot.getVehicleNumber().empty());
}

TEST_CASE("ParkingSpot - park successfully") {
  ParkingSpot spot(1, 1.0f);

  bool result = spot.park("A123BC");

  CHECK(result == true);
  CHECK(spot.isOccupied() == true);
  CHECK(spot.getVehicleNumber() == "A123BC");
}

TEST_CASE("ParkingSpot - cannot park twice") {
  ParkingSpot spot(1, 1.0f);

  spot.park("A123BC");
  bool result = spot.park("B456CD");

  CHECK(result == false);
  CHECK(spot.getVehicleNumber() == "A123BC");
}

TEST_CASE("ParkingSpot - leave throws when empty") {
  ParkingSpot spot(1, 1.0f);

  CHECK_THROWS_AS(spot.leave(), std::logic_error);
}
