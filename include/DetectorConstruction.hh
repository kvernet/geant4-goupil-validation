#ifndef DETECTOR_CONSTRUCTION_H
#define DETECTOR_CONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"

struct DetectorConstruction : public G4VUserDetectorConstruction {    
    public:
        static DetectorConstruction * Singleton();
        G4VPhysicalVolume * Construct();
    
    private:
        DetectorConstruction();
        ~DetectorConstruction() override = default;
};

#endif
