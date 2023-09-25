#ifndef PRIMARY_GENERATOR_H
#define PRIMARY_GENERATOR_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"

class G4ParticleGun;
class G4Event;

struct PrimaryEvent {
    /* Particle identifier, using PDG numering scheme. */
    int pid;
    
    /* Kinetic energy, in MeV. */
    double energy;
    
    /* Initial position, in cm. */
    double position[3];
    
    /* Initial direction. */
    double direction[3];
    
    /* Monte-Carlo weight. */
    double weight;
};

struct PrimaryGenerator : public G4VUserPrimaryGeneratorAction {
    public:
        static PrimaryGenerator * Singleton();
        
        void GeneratePrimaries(G4Event*) override;
        
        const PrimaryEvent * event = nullptr;
        double weight = 0.0;
    
    private:
        PrimaryGenerator();
        ~PrimaryGenerator() override;
        G4ParticleGun * particleGun = nullptr;
};

#endif
