#ifndef PARKINGMANAGER_PARKINGSPOT_H
#define PARKINGMANAGER_PARKINGSPOT_H

#include <string>
#include <stdexcept>

class ParkingSpot {
private:
    int id;
    float size;
    bool occupied;
    std::string VehicleNumber;
    std::time_t entryTime;
public:
    ParkingSpot(int entery_id, float entery_size);
    bool park(const std::string& vehiclenumber);
    double leave();
    int getId() const;
    float getSize() const;
    bool isOccupied() const;
    const std::string& getVehicleNumber() const;
};

#endif //PARKINGMANAGER_PARKINGSPOT_H