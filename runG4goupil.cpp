#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGenerator.hh"
#include "SteppingAction.hh"
/* Geant4 interface */
#include "G4RunManagerFactory.hh"

struct header {
    char model[32];
    double energy;
    long events;
};

int main() {
    /* Simulation parameters */
    const G4String model("standard");
    const double energy = 1;
    const long events = 1000000;
    std::string fileName = "test.dat";
    FILE * stream = fopen(fileName.c_str(), "wb");
    
    if(stream) {
        struct header header;
        std::strcpy(header.model, model.c_str());
        header.energy = energy;
        header.events = events;
        fwrite(&header, sizeof(header), 1, stream);
        fclose(stream);
    }
    else {
        std::cerr << "Could not open file " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }
    
    auto buffer = std::cout.rdbuf();
    std::cout.rdbuf(nullptr); /* Disable cout temporarly. */
    /* Construct the default run manager */
    auto * runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
    std::cout.rdbuf(buffer);
    
    /* Set mandatory initialization classes */
    runManager->SetUserInitialization(DetectorConstruction::Singleton());
    auto && physics = PhysicsList::Singleton(model);
    runManager->SetUserInitialization(physics);
    physics->DisableVerbosity();
    
    auto && generator = PrimaryGenerator::Singleton();
    runManager->SetUserAction(generator);
    runManager->SetUserAction(new SteppingAction(fileName));
    
    runManager->Initialize();
    
    generator->event->energy = energy;
    
    runManager->BeamOn(events);
    
    
    delete runManager;
    return 0;
}
