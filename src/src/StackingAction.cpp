#include "StackingAction.hh"

#include "G4VProcess.hh"

struct StackingAction * StackingAction::Singleton() {
    static StackingAction * action = nullptr;
    if(action == nullptr) action = new StackingAction;
    return action;
}
    
G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track * track) {
    G4ClassificationOfNewTrack classification = fWaiting;
    if (!this->keepSecondaries) {
        classification = (track->GetTrackID() == 1) ? fUrgent : fKill;
    } else if (track->GetTrackID() != 1) {
        int pid = track->GetParticleDefinition()->GetPDGEncoding();
        std::string process = track->GetCreatorProcess()->GetProcessName();
        auto key = std::make_pair(pid, process);
        long n = 1;
        try {
            n = this->secondaries.at(key) + 1;
        } catch (const std::out_of_range &e) {}
        this->secondaries[key] = n;
    }
    return classification;
}

void StackingAction::PrintSecondaries() const {
    std::puts("=== secondaries ===");
    std::puts(" pid  process  count");
    for (auto it: this->secondaries) {
        auto && key = it.first;
        std::printf(
            "%4d %8s  %ld\n",
            key.first, key.second.c_str(), it.second
        );
    }
}
