/**
 * Description:
 * This code reads a HIPO file containing data from the "HEL::scaler" bank,
 * which stores helicity and FCup scaler values. It parses through the events
 * in the HIPO file, accumulates statistics based on the helicity value, and
 * calculates the FCup asymmetry.
 * 
 * Steps:
 * 1. Open the HIPO file and set up the reader and event structures.
 * 2. Initialize variables to store FCup and helicity statistics.
 * 3. Iterate over each event in the HIPO file.
 *    - Retrieve the helicity and FCup values from the "HEL::scaler" bank.
 *    - Accumulate statistics based on the helicity value.
 * 4. Print out the collected information, including FCup sums for different helicity states.
 * 5. Calculate and print the FCup asymmetry as a percentage.
 * 6. Close the HIPO file and return 0.
 * 
 * Usage:
 * clas12root ex_C_fcupgated.C
 *
 */


int ex_C_fcupgated(const char * hipoFile = "/volatile/clas12/rg-c/production/dst/8.7.0_TBT/dst/train/sidisdvcs/sidisdvcs_016322.hipo"
){
    
    // Parse through HEL::scaler
    Double_t fcup = 0.0;
    Double_t fcup_pos = 0.0;
    Double_t fcup_neg = 0.0;
    Double_t fcup_zero = 0.0;
    Double_t fcup_bad = 0.0;
    int hel = -999;


    hipo::reader     reader_;
    hipo::event      event_;
    hipo::dictionary  factory_;

    reader_.setTags(1);
    reader_.open(hipoFile); //keep a pointer to the reader
    reader_.readDictionary(factory_);
    hipo::bank HEL(factory_.getSchema("HEL::scaler"));
    while(reader_.next()){
        reader_.read(event_);
        event_.getStructure(HEL);

        hel = HEL.getInt("helicity",0);
        fcup = HEL.getFloat("fcupgated",0);


        if(HEL.getRows()==0){
          fcup_bad+=fcup;
        }
        else if(hel==1)
          fcup_pos+=fcup;
        else if(hel==0)
          fcup_zero+=fcup;
        else if(hel==-1)
          fcup_neg+=fcup;
        else
          cout << "ERROR" << endl;
    }

    // Print out the collected information
    std::cout << "fcup_pos: " << fcup_pos << std::endl;
    std::cout << "fcup_neg: " << fcup_neg << std::endl;
    std::cout << "fcup_zero: " << fcup_zero << std::endl;
    std::cout << "fcup_bad: " << fcup_bad << std::endl;

    std::cout << "\n FCup Asymmetry = " << (fcup_pos-fcup_neg)/(fcup_pos+fcup_neg) * 100 << "%\n" << std::endl;
    
    return 0;
}