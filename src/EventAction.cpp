#include "EventAction.hh"

EventAction::EventAction (std::string fName, size_t events, size_t p) :
        fileName(fName), left(events), generated(0), period(p) {

}

void EventAction::EndOfEventAction (const G4Event* ) {
    this->generated++;
    
    size_t min = this->left < this->period ? this->left : this->period;
    
    if(this->generated == min) {
        /* Update the file header */
        FILE * stream = fopen(this->fileName.c_str(), "r+b");
        if(stream) {
            struct header header;
            fseek(stream, 0L, SEEK_SET);
            fread(&header, sizeof(header), 1, stream);
            
            header.generated += this->generated;
            fseek(stream, 0L, SEEK_SET);
            fwrite(&header, sizeof(header), 1, stream);
            
            fclose(stream);
        }
        this->generated = 0;
        this->left -= this->period;
    }
}
