#include "DetectorConstruction.hh"
#include "PrimaryGenerator.hh"
/* Geant4 interface. */
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4Gamma.hh"
#include "Randomize.hh"

PrimaryGenerator::PrimaryGenerator() : G4VUserPrimaryGeneratorAction() {
    this->event = new EventState;
    G4int n_particle = 1;
    this->particleGun = new G4ParticleGun(n_particle);
}

PrimaryGenerator::~PrimaryGenerator() {
    delete this->event;
    delete this->particleGun;
}

PrimaryGenerator * PrimaryGenerator::Singleton() {
    static PrimaryGenerator * generator = nullptr;
    if(generator == nullptr) {
        generator = new PrimaryGenerator;
    }
    return generator;
}

void PrimaryGenerator::GeneratePrimaries(G4Event * anEvent) {
    this->RandomState();
    
    particleGun->SetParticleDefinition(G4Gamma::GammaDefinition());
    
    // Forward primary energy.
    particleGun->SetParticleEnergy(this->event->energy * CLHEP::MeV);
    
    // Forward primary direction.
    auto direction = G4ThreeVector(
        this->event->direction[0],
        this->event->direction[1],
        this->event->direction[2]
    );
    direction /= direction.mag();
    particleGun->SetParticleMomentumDirection(direction);
    
    // Forward primary position.
    particleGun->SetParticlePosition(
        G4ThreeVector(
            this->event->position[0] * CLHEP::cm,
            this->event->position[1] * CLHEP::cm,
            this->event->position[2] * CLHEP::cm
        )
    );
    
    // Create primary vertex.
    particleGun->GeneratePrimaryVertex(anEvent);
}

void PrimaryGenerator::RandomState() {
    const double cosTheta = 2.0 * G4UniformRand() - 1.0;
    const double sinTheta = std::sqrt(1.0 - cosTheta*cosTheta);
    const double phi = 2.0 * M_PI * G4UniformRand();
    const double cosPhi = std::cos(phi);
    const double sinPhi = std::sin(phi);
    
    /* Set momentum direction */
    this->event->direction[0] = sinTheta * cosPhi;
    this->event->direction[1] = sinTheta * sinPhi;
    this->event->direction[2] = cosTheta;
    
    const auto outerSize = DetectorConstruction::Singleton()->worldSize;
    const auto innerSize = DetectorConstruction::Singleton()->detectorSize;
    
    G4ThreeVector position(0.0, 0.0, 0.0);
    while (
        (std::fabs(position[0]) <= 0.5 * innerSize[0]) &&
        (std::fabs(position[1]) <= 0.5 * innerSize[1]) &&
        (std::fabs(position[2]) <= 0.5 * innerSize[2])
        ) {
        position[0] = outerSize[0] * (0.5 - G4UniformRand());
        position[1] = outerSize[1] * (0.5 - G4UniformRand());
        position[2] = outerSize[2] * (0.5 - G4UniformRand());
    }
    
    for(int i = 0; i < 3; i++) this->event->position[i] = position[i] / CLHEP::cm;
}
