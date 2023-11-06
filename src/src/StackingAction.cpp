#include "StackingAction.hh"

struct StackingAction * StackingAction::Singleton() {
    static StackingAction * action = nullptr;
    if(action == nullptr) action = new StackingAction;
    return action;
}
    
G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track * track) {
    G4ClassificationOfNewTrack classification = fWaiting;
    if (!this->keepSecondaries) {
        classification = (track->GetTrackID() == 1) ? fUrgent : fKill;
    }
    
    return classification;
}
