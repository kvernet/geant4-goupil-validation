#ifndef DETECTOR_CONSTRUCTION_H
#define DETECTOR_CONSTRUCTION_H

#include "G4Material.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"

struct DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        static DetectorConstruction * Singleton();
        G4VPhysicalVolume * Construct();
        
        G4double worldSize[3], detectorSize[3];
        G4double airSize[3], groundSize[3];
        
        G4LogicalVolume * worldVolume = nullptr;
        G4Material * material;
    
    private:
        DetectorConstruction();
        ~DetectorConstruction() override = default;
        
        void ComputeSize();
        
        void PlaceInVolume(const std::string& name,
                G4double dim[3], G4Material * material,
                G4RotationMatrix * rot, G4ThreeVector pos,
                G4LogicalVolume * motherVolume);
};

#endif
