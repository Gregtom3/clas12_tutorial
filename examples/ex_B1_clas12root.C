/**
 * Description:
 * This code performs an analysis on a CLAS12 HIPO file. It reads in the file, applies event selection cuts,
 * calculates event variables and particle properties, and fills a TTree with the selected events and variables.
 * The output TTree is then written to a ROOT file.
 * 
 * Steps:
 * 1. Create a TFile to store the output ROOT file.
 * 2. Create a TTree named "events" to store the event data.
 * 3. Declare variables for the event and particle properties.
 * 4. Set the branch addresses for the TTree to link the variables.
 * 5. Configure the CLAS12 Reader and HipoChain to read the HIPO file.
 * 6. Set up the necessary bank structures for accessing data in the HIPO file.
 * 7. Loop over the events in the HIPO file.
 *    - Get the event information from the RUN::config bank.
 *    - Get the reconstructed particles from the REC::Particle bank.
 *    - Calculate the highest energy electron index.
 *    - Apply selection cuts on the scattered electron.
 *    - Fill the TTree with the selected event and particle properties.
 * 8. Write the TTree to the TFile.
 * 9. Close the TFile and return 0.
 * 
 * Usage:
 *   clas12root ex_B1_clas12root.C 
 */



// =============================================================================
// User-defined structs for this analysis code
struct EventVariables {
    double Q2;
    double x;
    double W;
    double y;
};

struct Particle {
    int  pid=0;
    int  pindex=0;
    float px=0;
    float py=0;
    float pz=0;
    float p=0;
    float E=0;
    float theta=0;
    float phi=0;
    float Epcal=0; 
};

// =============================================================================
// User-defined functions for this analysis code
EventVariables calculateEventVariables(TLorentzVector kf) {
    const double Mp = 0.938272; // proton mass, GeV
    const double Me = 0.000511; // electron mass, GeV
    const double beamE = 10.6041; // electron beam energy for Fall2018 RG-A, GeV

    TLorentzVector vec_eIn(0, 0, std::sqrt(beamE * beamE - Me * Me), beamE); // Px, Py, Pz, E for initial electron
    TLorentzVector vec_pIn(0, 0, 0, Mp); // Px, Py, Pz, E for initial (at rest) proton


    TLorentzVector q = vec_eIn - kf;
    double Q2 = -q.M2();
    double x = Q2 / (2 * vec_pIn.Dot(q));
    double W = (q + vec_pIn).M();
    double y = (q * vec_pIn) / (vec_eIn * vec_pIn);

    return {Q2, x, W, y};
}

int findHighestEnergyElectronIndex(const std::vector<Particle>& particles) {
    float maxEnergy = -1;
    int maxIndex = -1;

    for (int i = 0; i < particles.size(); ++i) {
        const Particle& particle = particles[i];
        if (particle.pid == 11 && particle.E > maxEnergy) {
            maxEnergy = particle.E;
            maxIndex = i;
        }
    }

    return maxIndex;
}



// =============================================================================
// Main function
int ex_B1_clas12root(const char * hipoFile = "/cache/clas12/rg-a/production/recon/fall2018/torus-1/pass1/v1/dst/train/nSidis/nSidis_005032.hipo"){
    
    // Create TFile
    TFile *file = new TFile("../data/ex_B1_nSidis_5032.root","RECREATE");
    
    // Create the TTree
    TTree *tree = new TTree("events", "DIS event data");

    // Declare variables
    double ele_P;
    double ele_Theta;
    double ele_Phi;
    double x;
    double Q2;
    double W;
    double y;

    int run, evnum, torus, helicity;
    
    // Set branch addresses
    tree->Branch("ele_P", &ele_P);
    tree->Branch("ele_Theta", &ele_Theta);
    tree->Branch("ele_Phi", &ele_Phi);
    tree->Branch("x", &x);
    tree->Branch("Q2", &Q2);
    tree->Branch("W", &W);
    tree->Branch("y", &y);
    tree->Branch("run", &run);
    tree->Branch("evnum", &evnum);
    tree->Branch("torus", &torus);
    tree->Branch("helicity", &helicity);
    
    // **************************************************
    // clas12root relevant code
    // 
    // --> Need for reading in the hipo files!
    // --> Create chain --> Link to clas12reader --> Get parser from reader
    // See (https://github.com/JeffersonLab/clas12root) for some examples
    
    
    // Configure CLAS12 Reader and HipoChain
    // Demand we see at least one electron
    // addAtLeastPid(#,#) stack, so you can define final states this way
    // -------------------------------------
    clas12root::HipoChain _chain;
    clas12::clas12reader *_config_c12{nullptr};

    _chain.Add(hipoFile); // Can add more files for longer analysis
    _config_c12=_chain.GetC12Reader();
    _config_c12->addAtLeastPid(11,1);     // At least 1 electron
    //_config_c12->addAtLeastPid(211,1);     // At least 1 piplus
    
    // Add RUN::config bank
    // -------------------------------------
    int _idx_RUNconfig = _config_c12->addBank("RUN::config");
    int _irun = _config_c12->getBankOrder(_idx_RUNconfig,"run");
    int _ievnum = _config_c12->getBankOrder(_idx_RUNconfig,"event");
    int _itorus = _config_c12->getBankOrder(_idx_RUNconfig,"torus");
    
    // Establish CLAS12 event parser
    // -------------------------------------
    auto &_c12=_chain.C12ref();

    // Add REC::Calorimeter bank
    // -------------------------------------
    int _idx_RECCal = _c12->addBank("REC::Calorimeter");
    int _ipindex_RECCal = _c12->getBankOrder(_idx_RECCal,"pindex");
    int _ienergy_RECCal = _c12->getBankOrder(_idx_RECCal,"energy");
    int _ilayer_RECCal = _c12->getBankOrder(_idx_RECCal,"layer");
    
    // ^^ for more tutorials, see (https://github.com/JeffersonLab/clas12root/tree/master/tutorial)
    //
    // ***************************************************
    
    int whileidx=0;
    int num_passed=0;
    // Create a vector of structs of particles to keep track of per event
    std::vector<Particle> parts;
    
    // Loop over the hipo file, event-by-event
    while(_chain.Next()==true){
        if(whileidx%10000==0 && whileidx!=0){
          std::cout << whileidx << " events read | " << num_passed*100.0/whileidx << "% passed event selection | " << std::endl;
        }
        
        whileidx++;
        parts.clear(); // Empty particle vector
        
        // Get the event
        // see documentation (https://github.com/JeffersonLab/clas12root/blob/master/Clas12Banks/event.h)
        // -------------------------------------
        auto event = _c12->event();
        
        // Get RUN::config bank information
        // -------------------------------------
        run = _c12->getBank(_idx_RUNconfig)->getInt(_irun,0);
        evnum = _c12->getBank(_idx_RUNconfig)->getInt(_ievnum,0);
        torus = _c12->getBank(_idx_RUNconfig)->getFloat(_itorus,0);
        
        // Get the event helicity
        // -------------------------------------
        helicity = event->getHelicity();
        
        
        // Loop over reconstructed particles (in REC::Particle)
        // -------------------------------------------------------
        auto particles=_c12->getDetParticles();
        for(unsigned int idx = 0 ; idx < particles.size() ; idx++){
            auto particle = particles.at(idx);
            // ^^^ see documentation (https://github.com/JeffersonLab/clas12root/blob/master/Clas12Banks/region_particle.h)
            int  pid = particle->getPid();
            int  pindex = idx;
            float px = particle->getPx();
            float py = particle->getPy();
            float pz = particle->getPz();
            float m  = particle->getPdgMass();
            float p  = sqrt(px*px+py*py+pz*pz);
            float E  = sqrt(p*p+m*m);
            float theta = particle->getTheta();
            float phi = particle->getPhi();
            
            // Create a new Particle object and push it to the vector
            Particle part;
            part.pid = pid;
            part.pindex = pindex;
            part.px = px;
            part.py = py;
            part.pz = pz;
            part.p = p;
            part.E = E;
            part.theta = theta;
            part.phi = phi;
            parts.push_back(part);
            
        } // for loop REC::Particle particles
        
        // Determine scattered electron as highest energy pid==11 particle
        int highestEnergyElectronIndex = findHighestEnergyElectronIndex(parts);
        if(highestEnergyElectronIndex==-1)
            continue; // No pid==11 particle found

        // Insist the scattered electron has at least 2.5 GeV
        if(parts[highestEnergyElectronIndex].E<2.5)
            continue;

        // Read through the REC::Calorimeter bank to find the particle's PCAL
        // For each particle, loop over each "hit" in REC::Calorimeter
        // If that hit's pindex is not equal to the particle we are looping with, skip to the next cal hit
        // If the Calo's hit happen in the PCal, store the PCAL deposited by the particle

        for(auto i = 0 ; i < parts.size(); i++){
            int pindex = parts.at(i).pindex;
            for(auto j = 0 ; j < _c12->getBank(_idx_RECCal)->getRows() ; j++){
                // Continue loop if the pindex in the calo bank does not match
                if(_c12->getBank(_idx_RECCal)->getInt(_ipindex_RECCal,j)!=pindex)
                  continue;
                
                int layerCal = _c12->getBank(_idx_RECCal)->getInt(_ilayer_RECCal,j);
                float energyCal = _c12->getBank(_idx_RECCal)->getFloat(_ienergy_RECCal,j);
                if(layerCal==1) // (1, PCAL) (4, ECIN) (7, ECOUT)
                    parts.at(i).Epcal = energyCal; // set particle's PCAL energy
                
            } // for loop calo hits
        } // for loop particles

        // Insist the scattered electron has deposited at least 0.1 GeV into the PCAL
        if(parts[highestEnergyElectronIndex].Epcal<0.1)
            continue;
        
        // Our scattered electron passed all our cuts! Fill the TTree
        num_passed++;
        TLorentzVector kf(parts[highestEnergyElectronIndex].px,
                          parts[highestEnergyElectronIndex].py,
                          parts[highestEnergyElectronIndex].pz,
                          parts[highestEnergyElectronIndex].E); // set TLorentzVector of scattered electron
        ele_P = kf.P();
        ele_Theta = kf.Theta()*180/3.14159;
        ele_Phi = kf.Phi()*180/3.14159;

        EventVariables ev_vars = calculateEventVariables(kf);

        x = ev_vars.x;
        Q2 = ev_vars.Q2;
        W = ev_vars.W;
        y = ev_vars.y;
        
        tree->Fill();
        
        
    } // for loop events

    // Write TTree to TFile
    tree->Write();
    // Close TFile
    file->Close();
    
    return 0;
    
}

