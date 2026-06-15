cmake_minimum_required(VERSION 4.1)
project(ParkingManager)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

#SFML
message(STATUS "Downloading SFML automatically...")

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)

set(BUILD_SHARED_LIBS TRUE)
set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_GRAPHICS OFF)
set(SFML_BUILD_WINDOW OFF)
set(SFML_BUILD_NETWORK ON)

FetchContent_MakeAvailable(SFML)

#parking
add_library(parking_lib STATIC
        src/ParkingSpot.cpp
        src/ParkingLot.cpp
)

target_include_directories(parking_lib PRIVATE include)

# server
add_executable(parking_server src/parking_server.cpp)
target_link_libraries(parking_server parking_lib sfml-network)
target_include_directories(parking_server PRIVATE include)

# client
add_executable(parking_client src/parking_client.cpp)
target_link_libraries(parking_client sfml-network)

#tests
message(STATUS "Downloading Doctest automatically...")

FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

add_executable(parking_tests
        tests/test_parking.cpp
        src/ParkingSpot.cpp
        src/ParkingLot.cpp
)

target_include_directories(parking_tests PRIVATE include)
target_include_directories(parking_tests PRIVATE ${doctest_SOURCE_DIR}/doctest)
target_link_libraries(parking_tests doctest)

enable_testing()
add_test(NAME parking_tests COMMAND parking_tests)

