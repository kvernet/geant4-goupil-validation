#ifndef PRIMARY_GENERATOR_H
#define PRIMARY_GENERATOR_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"

#include <array>

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

        std::array<std::pair<double, double>, 11> spectrum = {
            // Po^218 -> Pb^214.
            std::make_pair(0.242,  7.3),
            std::make_pair(0.295, 18.4),
            std::make_pair(0.352, 35.6),
            // Pb^214 -> Bi^214.
            std::make_pair(0.609, 45.5),
            std::make_pair(0.768,  4.9),
            std::make_pair(0.934,  3.1),
            std::make_pair(1.120, 14.9),
            std::make_pair(1.238,  5.8),
            std::make_pair(1.378,  4.0),
            std::make_pair(1.764, 15.3),
            std::make_pair(2.204,  4.9),
        };
};

#endif
