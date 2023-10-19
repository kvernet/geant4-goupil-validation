#ifndef PRIMARY_GENERATOR_H
#define PRIMARY_GENERATOR_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"

class G4ParticleGun;
class G4Event;

struct EventState {
    /* Kinetic energy, in MeV. */
    double energy;
    
    /* Initial position, in cm. */
    double position[3];
    
    /* Initial direction. */
    double direction[3];
};

struct PrimaryGenerator : public G4VUserPrimaryGeneratorAction {
    public:
        static PrimaryGenerator * Singleton();
        
        void GeneratePrimaries(G4Event*) override;
        
        void RandomState();
        
        EventState * event = nullptr;
        bool isInAir = true;
    
    private:
        PrimaryGenerator();
        ~PrimaryGenerator() override;
        G4ParticleGun * particleGun = nullptr;
};

#endif
