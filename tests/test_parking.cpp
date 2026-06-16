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

TEST_CASE("ParkingLot - cannot enter same vehicle twice") {
  createTestConfig("test_config.txt", "1.0 1.0");

  ParkingLot lot("test_config.txt", 100);
  int spotId;

  lot.EnterVehicle("A123BC", 1.0f, spotId);
  bool result = lot.EnterVehicle("A123BC", 1.0f, spotId);

  CHECK(result == false);
  CHECK(lot.getFreeSpots() == 1);

  deleteTestConfig("test_config.txt");
}

TEST_CASE("ParkingSpot - leave throws when empty") {
  ParkingSpot spot(1, 1.0f);

  CHECK_THROWS_AS(spot.leave(), std::logic_error);
}

TEST_CASE("ParkingLot - multiple enters and exits") {
  createTestConfig("test_config.txt", "1.0 1.0 1.0");

  ParkingLot lot("test_config.txt", 100);
  int spotId;

  bool result = lot.EnterVehicle("A111AA", 1.0f, spotId);
  lot.EnterVehicle("B222BB", 1.0f, spotId);
  lot.EnterVehicle("C333CC", 1.0f, spotId);

  CHECK(lot.getFreeSpots() == 0);
  CHECK(result == true);
  CHECK(spotId == 3);

  float cost, revenue;
  lot.ExitVehicle("A111AA", cost, revenue);
  CHECK(lot.getFreeSpots() == 1);
  CHECK(revenue == 100);

  deleteTestConfig("test_config.txt");
}

TEST_CASE("ParkingLot - vehicle size overflow") {
  createTestConfig("test_config.txt", "1.0 1.0");

  ParkingLot lot("test_config.txt", 100);
  int spotId;

  bool result = lot.EnterVehicle("TRUCK1", 2.5f, spotId);

  CHECK(result == false);
  CHECK(lot.getFreeSpots() == 2);

  deleteTestConfig("test_config.txt");
}

TEST_CASE("ParkingLot - truck needs larger spot") {
  createTestConfig("test_config.txt", "1.0 2.5 1.0");

  ParkingLot lot("test_config.txt", 100);
  int spotId;

  bool result = lot.EnterVehicle("TRUCK1", 2.5f, spotId);

  CHECK(result == true);
  CHECK(spotId == 2);

  deleteTestConfig("test_config.txt");
}

TEST_CASE("ParkingSpot - leave with zero seconds") {
  ParkingSpot spot(1, 1.0f);
  spot.park("A123BC");

  double hours = spot.leave();

  CHECK(hours >= 1.0);
  CHECK(spot.isOccupied() == false);
}

TEST_CASE("ParkingLot - load config successfully") {
  createTestConfig("test_config.txt", "1.0 1.0 2.5 0.5");

  ParkingLot lot("test_config.txt", 100);

  CHECK(lot.getFreeSpots() == 4);
  CHECK(lot.getRevenue() == 0.0f);

  deleteTestConfig("test_config.txt");
}

TEST_CASE("ParkingLot - throw on bad config") {
  CHECK_THROWS_AS(ParkingLot("nonexistent.txt", 100), std::runtime_error);
}

TEST_CASE("ParkingLot - cannot exit non-existent vehicle") {
  createTestConfig("test_config.txt", "1.0");

  ParkingLot lot("test_config.txt", 100);

  float cost, revenue;
  bool result = lot.ExitVehicle("NOTEXIST", cost, revenue);

  CHECK(result == false);
  CHECK(lot.getRevenue() == 0.0f);

  deleteTestConfig("test_config.txt");
}

TEST_CASE("ParkingLot - empty config file") {
  createTestConfig("empty.txt", "");
  CHECK_THROWS_AS(ParkingLot("empty.txt", 100), std::runtime_error);
  deleteTestConfig("empty.txt");
}

TEST_CASE("ParkingLot - mixed enter exit") {
  createTestConfig("test_config.txt", "1.0 1.0 1.0");
  ParkingLot lot("test_config.txt", 100);
  int id1, id2, id3;

  lot.EnterVehicle("A1", 1.0f, id1);
  lot.EnterVehicle("A2", 1.0f, id2);
  lot.EnterVehicle("A3", 1.0f, id3);
  CHECK(lot.getFreeSpots() == 0);

  float cost, revenue;
  lot.ExitVehicle("A2", cost, revenue);
  CHECK(lot.getFreeSpots() == 1);

  lot.EnterVehicle("A4", 1.0f, id1);
  CHECK(id1 == 2);
  deleteTestConfig("test_config.txt");
}

TEST_CASE("ParkingLot - thread safety") {
  createTestConfig("test_config.txt", "1.0 1.0 1.0");
  ParkingLot lot("test_config.txt", 100);

  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};

  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&, i]() {
        int spotId;
        std::string num = "CAR" + std::to_string(i);
        if (lot.EnterVehicle(num, 1.0f, spotId)) {
            successCount++;
        }
    });
  }

  for (auto& t : threads) t.join();
  CHECK(successCount == 3);
  CHECK(lot.getFreeSpots() == 0);
  deleteTestConfig("test_config.txt");
}