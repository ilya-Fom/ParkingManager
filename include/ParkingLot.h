#ifndef PARKINGMANAGER_PARKINGLOT_H
#define PARKINGMANAGER_PARKINGLOT_H

#include "ParkingSpot.h"
#include <string>
#include <vector>
#include <mutex>

class ParkingLot {
private:
    mutable std::mutex mtx;
    std::vector<ParkingSpot> Spots;
    float revenue;
    float hourlyRate;
public:
    ParkingLot(const std::string& path, int price);
    bool LoadSpotsFromFile(const std::string& path);
    bool EnterVehicle(const std::string& vehicleNumber, float vehicleSize, int& SpotId);
    bool ExitVehicle(const std::string& vehicleNumber, float& cost, float& totalRevenue);
    int getFreeSpots() const;
    float getRevenue() const;
    bool isParked(const std::string& vehicleNumber) const;
};

#endif //PARKINGMANAGER_PARKINGLOT_H