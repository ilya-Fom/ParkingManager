cmake_minimum_required(VERSION 4.1)
project(Parking_Manager)

set(CMAKE_CXX_STANDARD 20)

add_executable(Parking_Manager main.cpp
        include/ParkingLot.h
        include/ParkingSpot.h
        src/ParkingSpot.cpp
        src/ParkingLot.cpp)
