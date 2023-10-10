#ifndef EVENT_ACTION_H
#define EVENT_ACTION_H

#include "G4UserEventAction.hh"
#include "globals.hh"

struct header {
    char model[32];
    double energy;
    long events;
    long generated;
};

struct EventAction : public G4UserEventAction {
    public:
        EventAction (std::string fName, size_t events, size_t p=10000);
        
        void EndOfEventAction (const G4Event* ) override;
    
    private:        
        ~EventAction () override = default;
        
        std::string fileName;
        size_t generated;
        size_t left;
        size_t period;
};

#endif
