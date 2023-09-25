#include "PhysicsList.hh"
/* Geant4 physics. */
#include "G4DecayPhysics.hh"
#include "G4EmDNAPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
/* UImanager for disabling verbosity. */
#include "G4UImanager.hh"

PhysicsList::PhysicsList(const G4String& model) : G4VModularPhysicsList() {
    this->defaultCutValue = 1e-02 * CLHEP::cm;
    this->decayPhysics = new G4DecayPhysics();
    
    if (std::strcmp(model, "dna") == 0) {
        this->emPhysics = new G4EmDNAPhysics();
    } else if (std::strcmp(model, "standard") == 0) {
        this->emPhysics = new G4EmStandardPhysics();
    } else if (std::strcmp(model, "livermore") == 0) {
        this->emPhysics = new G4EmLivermorePhysics();
    } else if (std::strcmp(model, "option1") == 0) {
        this->emPhysics = new G4EmStandardPhysics_option1();
    } else if (std::strcmp(model, "option2") == 0) {
        this->emPhysics = new G4EmStandardPhysics_option2();
    } else if (std::strcmp(model, "option3") == 0) {
        this->emPhysics = new G4EmStandardPhysics_option3();
    } else if (std::strcmp(model, "option4") == 0) {
        this->emPhysics = new G4EmStandardPhysics_option4();
    } else if (std::strcmp(model, "penelope") == 0) {
        this->emPhysics = new G4EmPenelopePhysics();
    } else {
        exit(EXIT_FAILURE);
    }
}

PhysicsList::~PhysicsList() {
    delete this->decayPhysics;
    delete this->emPhysics;
}

PhysicsList * PhysicsList::Singleton(const G4String& model) {
    static PhysicsList * physics = nullptr;
    if(physics == nullptr) {
        physics = new PhysicsList(model);
    }
    return physics;
}

void PhysicsList::DisableVerbosity() const {
    auto UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/process/em/verbose 0");
    UImanager->ApplyCommand("/process/had/verbose 0");
}

void PhysicsList::ConstructParticle() {
    this->decayPhysics->ConstructParticle();
}

void PhysicsList::ConstructProcess() {
    AddTransportation();
    this->emPhysics->ConstructProcess();
}
