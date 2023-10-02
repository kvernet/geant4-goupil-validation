#ifndef DETECTOR_CONSTRUCTION_H
#define DETECTOR_CONSTRUCTION_H

#include "G4Material.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"

struct DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        static DetectorConstruction * Singleton();
        G4VPhysicalVolume * Construct();
        
        G4double outerSize[3], innerSize[3];
        
        G4LogicalVolume * worldVolume = nullptr;
        G4Material * material;
    
    private:
        DetectorConstruction();
        ~DetectorConstruction() override = default;
};

#endif
