#include "DetectorConstruction.hh"
/* Geant4 volumes. */
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Navigator.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4VUserDetectorConstruction.hh"

DetectorConstruction::DetectorConstruction() {
    outerSize[0] = outerSize[1] = outerSize[2] = 2.0*CLHEP::km;
    innerSize[0] = innerSize[1] = innerSize[2] = 20.*CLHEP::m;
}

DetectorConstruction * DetectorConstruction::Singleton() {
    static DetectorConstruction * detector = nullptr;
    if(detector == nullptr) {
        detector = new DetectorConstruction;
    }
    return detector;
}

G4VPhysicalVolume * DetectorConstruction::Construct() {
    auto manager = G4NistManager::Instance();
    G4LogicalVolume * world = nullptr;
    {
        std::string name = "World";
        auto solid = new G4Box(name, 0.5*outerSize[0], 0.5*outerSize[1], 0.5*outerSize[2]);
        this->material = manager->FindOrBuildMaterial("G4_AIR");
        world = new G4LogicalVolume(solid, this->material, name);
    }
    this->worldVolume = world;
    
    G4LogicalVolume * inner = nullptr;
    {
        std::string name = "Inner";
        auto solid = new G4Box(name, 0.5*innerSize[0], 0.5*innerSize[1], 0.5*innerSize[2]);
        inner = new G4LogicalVolume(solid, this->material, name);
        new G4PVPlacement(
            nullptr,
            G4ThreeVector(0.0, 0.0, 0.0),
            inner,
            name,
            world,
            false,
            0
        );
    }
    
    return new G4PVPlacement(
        nullptr,
        G4ThreeVector(0.0, 0.0, 0.0),
        world,
        "World",
        nullptr,
        false,
        0
    );
}




