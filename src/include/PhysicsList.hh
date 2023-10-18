#ifndef PHYSICS_LIST_H
#define PHYSICS_LIST_H

#include "G4VModularPhysicsList.hh"

struct PhysicsList : public G4VModularPhysicsList {
    public:
        static PhysicsList * Singleton(const G4String& model="standard");
        
        /* Disable physics verbosity. */
        void DisableVerbosity() const;
        
        /* Geant4 physics objects. */
        G4VPhysicsConstructor * decayPhysics;
        G4VPhysicsConstructor * emPhysics;
        
        /* Geant4 API. */
        void ConstructParticle();
        void ConstructProcess();        
    
    private:
        PhysicsList(const G4String& model);
        ~PhysicsList() override;
};

#endif
