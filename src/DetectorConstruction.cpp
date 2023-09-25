#include "DetectorConstruction.hh"
/* Geant4 volumes. */
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Navigator.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4VUserDetectorConstruction.hh"

DetectorConstruction::DetectorConstruction() {}

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
        G4double hw = 1.0 * CLHEP::km;
        auto solid = new G4Box(name, hw, hw, hw);
        auto material = manager->FindOrBuildMaterial("G4_AIR");
        world = new G4LogicalVolume(solid, material, name);
    }
    
    G4LogicalVolume * inner = nullptr;
    {
        std::string name = "Inner";
        G4double hw = 10. * CLHEP::m;
        auto solid = new G4Box(name, hw, hw, hw);
        auto material = manager->FindOrBuildMaterial("G4_Pb");
        inner = new G4LogicalVolume(solid, material, name);
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




