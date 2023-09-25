#include "PrimaryGenerator.hh"
/* Geant4 interface. */
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4Gamma.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4Geantino.hh"

PrimaryGenerator::PrimaryGenerator() : G4VUserPrimaryGeneratorAction() {
    G4int n_particle = 1;
    this->particleGun = new G4ParticleGun(n_particle);
}

PrimaryGenerator::~PrimaryGenerator() {
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
    G4ParticleDefinition * definition = nullptr;
    if(this->event->pid == G4Gamma::GammaDefinition()->GetPDGEncoding()) {
        definition = G4Gamma::GammaDefinition();
    }else if (this->event->pid == G4MuonMinus::MuonMinusDefinition()->GetPDGEncoding()) {
        definition = G4MuonMinus::MuonMinusDefinition();
    }else if (this->event->pid == G4MuonPlus::MuonPlusDefinition()->GetPDGEncoding()) {
        definition = G4MuonPlus::MuonPlusDefinition();
    }
    else {
        definition = G4Geantino::GeantinoDefinition();
    }
    particleGun->SetParticleDefinition(definition);
    
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
    
    // Consume current event.
    //
    // Note that event weight would be lost at this point. Thus, we copy
    // it locally, before incrementing the events counter.
    this->weight = this->event->weight;
    
    // Create primary vertex.
    particleGun->GeneratePrimaryVertex(anEvent);
}
