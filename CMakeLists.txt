cmake_minimum_required(VERSION 3.0.0)
project(G4GOUPIL
    LANGUAGES CXX
    VERSION 0.1.0
)

# Geant4 includes
find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})
include_directories(${Geant4_INCLUDE_DIR})


#link_directories(${CMAKE_INSTALL_PREFIX}/lib)
#include_directories(${CMAKE_INSTALL_PREFIX}/include)

# Geant4 goupil validation
add_library(G4goupil SHARED
    src/DetectorConstruction.cpp include/DetectorConstruction.hh
    src/PhysicsList.cpp include/PhysicsList.hh
    src/PrimaryGenerator.cpp include/PrimaryGenerator.hh
    src/EventAction.cpp include/EventAction.hh
    src/SteppingAction.cpp include/SteppingAction.hh
)
set_target_properties(G4goupil PROPERTIES VERSION ${G4GOUPIL_VERSION})
target_include_directories(G4goupil PRIVATE include)
target_link_libraries(G4goupil ${Geant4_LIBRARIES})

add_executable(runG4goupil runG4goupil.cpp)
target_link_libraries(runG4goupil G4goupil)
target_include_directories(runG4goupil PRIVATE include)
install(TARGETS runG4goupil DESTINATION .)

install(TARGETS G4goupil DESTINATION lib)
install(DIRECTORY include DESTINATION .)
