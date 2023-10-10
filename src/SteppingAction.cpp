#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "PrimaryGenerator.hh"
#include "SteppingAction.hh"


struct Event {
    int eventid;
    int tid;
    int parent;
    int pid;
    struct EventState primary;
    struct EventState detected;
    char creator[8];
};

void SteppingAction::UserSteppingAction(const G4Step * step) {
    auto && point = step->GetPostStepPoint();
    auto && physical = point->GetTouchableHandle()->GetVolume();
    if ((physical == nullptr) || (physical->GetLogicalVolume() == DetectorConstruction::Singleton()->worldVolume)) {
        return;
    }
    
    auto && track = step->GetTrack();
    track->SetTrackStatus(fStopAndKill);
    
    struct Event event;
    event.eventid = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    event.tid = track->GetTrackID();
    event.pid = track->GetParticleDefinition()->GetPDGEncoding();
    
    auto && generator = PrimaryGenerator::Singleton();
    if(event.tid == 1) {
        event.parent = 0;
        event.primary = *generator->event;
        strcpy(event.creator, "primary");
    }
    else {
        event.parent = track->GetParentID();
        event.primary.energy = track->GetVertexKineticEnergy() / CLHEP::MeV;
        for(int i = 0; i < 3; i++) {
            event.primary.position[i] = track->GetVertexPosition()[i] / CLHEP::cm;
            event.primary.direction[i] = track->GetVertexMomentumDirection()[i];
        }
        strncpy(event.creator, track->GetCreatorProcess()->GetProcessName().c_str(), sizeof(event.creator));        
    }
    
    
    auto && position = point->GetPosition();
    auto && direction = point->GetMomentumDirection();
    event.detected.energy = point->GetKineticEnergy() / CLHEP::MeV;
    for (int i = 0; i < 3; i++) {
        event.detected.position[i] = position[i] / CLHEP::cm;
        event.detected.direction[i] = direction[i];
    }
    
    FILE * stream = fopen(this->fileName.c_str(), "ab");
    if(stream) {
        fwrite(&event, sizeof(event), 1, stream);
        fclose(stream);
    }
}
