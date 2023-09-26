#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "PrimaryGenerator.hh"
#include "SteppingAction.hh"


struct Event {
    int eventid;
    int tid;
    // XXX int parent (track->GetParentID());
    int pid;
    struct EventState primary;
    struct EventState detected;
};

void SteppingAction::UserSteppingAction(const G4Step * step) {
    
    if (step->GetTrack()->GetTrackStatus() == fStopAndKill) {
        return;
    }
    
    G4LogicalVolume * volume = step->GetPreStepPoint()->GetTouchableHandle()
        ->GetVolume()->GetLogicalVolume();
    
    if(volume == DetectorConstruction::Singleton()->worldVolume) {
        return;
    }
    
    G4cout << "XXX An event!" << G4endl;
    
    auto && track = step->GetTrack();
    track->SetTrackStatus(fStopAndKill);
    
    struct Event event;
    event.eventid = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    event.tid = track->GetTrackID();
    event.pid = track->GetParticleDefinition()->GetPDGEncoding();
    
    auto && generator = PrimaryGenerator::Singleton();
    event.primary = *generator->event; // XXX if tid == 1 else GetVertexPosition, etc.
    
    auto && point = step->GetPreStepPoint();
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
