cmake_minimum_required(VERSION 3.0.0)
project(G4GOUPIL
    LANGUAGES CXX
    VERSION 0.1.0
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")


# Geant4 includes
find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})
include_directories(${Geant4_INCLUDE_DIR})


# Geant4 goupil validation
add_library(G4goupil SHARED
    src/src/DetectorConstruction.cpp src/include/DetectorConstruction.hh
    src/src/PhysicsList.cpp src/include/PhysicsList.hh
    src/src/PrimaryGenerator.cpp src/include/PrimaryGenerator.hh
    src/src/EventAction.cpp src/include/EventAction.hh
    src/src/SteppingAction.cpp src/include/SteppingAction.hh
)
set_target_properties(G4goupil PROPERTIES VERSION ${G4GOUPIL_VERSION})
target_include_directories(G4goupil PUBLIC src/include)
target_link_libraries(G4goupil ${Geant4_LIBRARIES})

add_executable(runG4goupil src/runG4goupil.cpp)
set_target_properties(runG4goupil PROPERTIES
    INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
)
target_link_libraries(runG4goupil G4goupil)
install(TARGETS runG4goupil DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

install(TARGETS G4goupil DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY src/include DESTINATION ${CMAKE_INSTALL_PREFIX})
