#include "DetectorConstruction.hh"
/* Geant4 volumes. */
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Navigator.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4VUserDetectorConstruction.hh"

DetectorConstruction::DetectorConstruction() {
    this->ComputeSize();
}

DetectorConstruction * DetectorConstruction::Singleton() {
    static DetectorConstruction * detector = nullptr;
    if(detector == nullptr) {
        detector = new DetectorConstruction;
    }
    return detector;
}

void DetectorConstruction::ComputeSize() {
    detectorSize[0] = detectorSize[1] = 20.0*CLHEP::m;
    detectorSize[2] = 10.0*CLHEP::m;
    
    airSize[0] = airSize[1] = 2.0*CLHEP::km;
    airSize[2] = 1*CLHEP::km;
    groundSize[2] = 1*CLHEP::m;
    for(int i = 0; i < 2; i++) {
        worldSize[i] = groundSize[i] = airSize[i];
    }
    worldSize[2] = groundSize[2] + airSize[2];
    detectorOffset = 0.5 * (-airSize[2] + detectorSize[2] + groundSize[2]) + 5.0 * CLHEP::cm;
}

G4VPhysicalVolume * DetectorConstruction::Construct() {
    auto manager = G4NistManager::Instance();
    G4LogicalVolume * world = nullptr;
    {
        std::string name = "World";
        auto solid = new G4Box(name, 0.5*worldSize[0], 0.5*worldSize[1], 0.5*worldSize[2]);
        this->material = manager->FindOrBuildMaterial("G4_AIR");
        world = new G4LogicalVolume(solid, this->material, name);
    }
    this->worldVolume = world;
    
    G4LogicalVolume * airVolume = nullptr;
    {
        std::string name = "Air";
        auto material = manager->FindOrBuildMaterial("G4_AIR");
        G4ThreeVector pos(0.0, 0.0, 0.5*groundSize[2]);
        airVolume = this->PlaceInVolume(name, airSize, material, nullptr, pos, world);
    }
    
    {
        std::string name = "Ground";
        auto material = manager->FindOrBuildMaterial("G4_CALCIUM_CARBONATE");
        G4ThreeVector pos(0.0, 0.0, -0.5*airSize[2]);
        this->PlaceInVolume(name, groundSize, material, nullptr, pos, world);
    }
    
    G4LogicalVolume * detVolume = nullptr;
    {
        std::string name = "Detector";
        auto material = manager->FindOrBuildMaterial("G4_AIR");
        G4ThreeVector pos(0.0, 0.0, this->detectorOffset - 0.5 * groundSize[2]);
        detVolume = this->PlaceInVolume(name, detectorSize, material, nullptr, pos, airVolume);
    }
    this->detectorVolume = detVolume;
    
    return new G4PVPlacement(
        nullptr,
        G4ThreeVector(0.0, 0.0, 0.0),
        world,
        "World",
        nullptr,
        false,
        0,
        this->check_overlaps
    );
}

G4LogicalVolume * DetectorConstruction::PlaceInVolume(const std::string& name,
        G4double dim[3], G4Material * material,
        G4RotationMatrix * rot, G4ThreeVector pos,
        G4LogicalVolume * motherVolume) {
    auto solid = new G4Box(name, 0.5*dim[0], 0.5*dim[1], 0.5*dim[2]);
    auto logicalVolume = new G4LogicalVolume(solid, material, name);
    new G4PVPlacement(
        rot,
        pos,
        logicalVolume,
        name,
        motherVolume,
        false,
        0,
        this->check_overlaps
    );
    return logicalVolume;
}
