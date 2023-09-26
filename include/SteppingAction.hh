#ifndef STEPPING_ACTION_H
#define STEPPING_ACTION_H

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4Step.hh"


struct SteppingAction : public G4UserSteppingAction {
    SteppingAction(std::string fName) : fileName(fName) {}
    ~SteppingAction() override = default;
    
    // method from the base class
    void UserSteppingAction(const G4Step*) override;
    
    std::string fileName;
};

#endif
