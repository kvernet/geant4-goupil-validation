#ifndef STACKING_ACTION_H
#define STACKING_ACTION_H

#include "G4Track.hh"
#include "G4UserStackingAction.hh"

struct StackingAction : public G4UserStackingAction {
    public:
        static StackingAction * Singleton();
        G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *);

        void PrintSecondaries() const;

        bool keepSecondaries = true;
        std::map<std::pair<int, std::string>, long> secondaries;

        
    private:
        StackingAction() = default;
        ~StackingAction() override = default;
};

#endif
