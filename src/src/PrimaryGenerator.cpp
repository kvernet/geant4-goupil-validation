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

    // Normalize intensities.
    double norm = 0.0;
    for (auto pair: this->spectrum) {
        norm += pair.second;
    }
    norm = 1.0 / norm;
    double cdf = 0.0;
    for (auto &pair: this->spectrum) {
        cdf += pair.second * norm;
        pair.second = cdf;
    }
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
    
    // Forward or sample the primary energy.
    double energy;
    if (this->event->energy > 0.0) {
        energy = this->event->energy;
    } else {
        energy = this->spectrum.back().first;
        const double u = G4UniformRand();
        for (auto pair: this->spectrum) {
            if (u <= pair.second) {
                energy = pair.first;
                break;
            }
        }
    }
    particleGun->SetParticleEnergy(energy * CLHEP::MeV);
    
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
    
    auto && geometry = DetectorConstruction::Singleton();
    G4ThreeVector position(0.0, 0.0, 0.0);
    if(this->isInAir) { 
        const auto airSize = geometry->airSize;
        //const auto groundSize = geometry->groundSize;
        const auto detectorSize = geometry->detectorSize;
        const auto airOffset = 0.5 * geometry->groundSize[2];
        const auto detectorOffset = geometry->detectorOffset;
        for (;;) {
            position[0] = airSize[0] * (0.5 - G4UniformRand());
            position[1] = airSize[1] * (0.5 - G4UniformRand());
            position[2] = airSize[2] * (0.5 - G4UniformRand()) + airOffset;
            
            if ((std::fabs(position[0]) > 0.5 * detectorSize[0]) ||
                (std::fabs(position[1]) > 0.5 * detectorSize[1]) ||
                (std::fabs(position[2] - detectorOffset) > 0.5 * detectorSize[2])
            ) {
                break;
            }
        }
    }
    else {
        const auto groundSize = geometry->groundSize;
        const auto detectorSize = geometry->detectorSize;
        const auto offset = -0.5 * geometry->airSize[2];
        position[0] = (detectorSize[0] + 4.0 * groundSize[2]) * (0.5 - G4UniformRand());
        position[1] = (detectorSize[1] + 4.0 * groundSize[2]) * (0.5 - G4UniformRand());
        position[2] = groundSize[2] * (0.5 - G4UniformRand()) + offset;
    }
    
    for(int i = 0; i < 3; i++) this->event->position[i] = position[i] / CLHEP::cm;
}
