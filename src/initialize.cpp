#include "ambermachine.h"

int get_replicate_count(std::string path)
{
    int r = 1;
    for(const fs::directory_entry& entry : fs::directory_iterator{path})
    {
        if (fs::is_directory(entry.path()))
        {
            std::string junk = entry.path().c_str();
            if (junk.find("Replicate_") != std::string::npos)
            {
                r++;
            }
        }
    }
    return r;
}

int cpptraj_mask_check(std::string prmtop, std::string inpcrd, std::string mask)
{
    if (mask == "")
    {
        return 0;
    }
    std::stringstream buffer;
    buffer.str("");
    buffer << "parm " << prmtop << std::endl;
    buffer << "trajin " << inpcrd << std::endl;
    buffer << "select " << mask <<std::endl;
    utils::write_to_file("cpptraj_mask_test.in",buffer.str());
    utils::silent_shell("module load Amber/20-cuda-11; cpptraj < cpptraj_mask_test.in > cpptraj_mask_test.out");

    std::ifstream file("cpptraj_mask_test.out");
    if (file.is_open()) 
    {
        std::string line;
        std::stringstream dummy;
        while (std::getline(file, line)) 
        {
            // if  "Selected" in line and "atoms." in line, check if the second value in the line is zero.  If so, return true, else return false.
            // Check for comments, clear them out
            // std::string::size_type sel = line.find('Selected');
            if (line.find("Selected") != std::string::npos)
            {
                std::string::size_type atoms = line.find("atoms.");
                if (atoms != std::string::npos)
                {
                    int startpos = line.find("Selected ")+9;
                    int endpos = line.find("atoms") - startpos;
                    std::string tmp = line.substr(startpos,endpos);

                    if (stoi(tmp) == 0)
                    {  
                        file.close();
                        fs::remove("cpptraj_mask_test.out");
                        fs::remove("cpptraj_mask_test.in");
                        return 1;
                    }
                    else
                    {
                        file.close();
                        fs::remove("cpptraj_mask_test.out");
                        fs::remove("cpptraj_mask_test.in");
                        return 0;
                    }
                }
            }
        }
    }
    return 0;
}

bool filechecks(JobSettings settings)
{
    // return true if a failure occurs.
    bool EPICFAIL = false;
    std::stringstream failure_log;
    failure_log.str("");
    if (!utils::CheckFileExists(settings.PRMTOP))
    {
        EPICFAIL = true;
        failure_log << "Unable to find file: " << settings.PRMTOP << std::endl;
    }
    if (!utils::CheckFileExists(settings.INPCRD))
    {
        EPICFAIL = true;
        failure_log << "Unable to find file: " << settings.INPCRD << std::endl;
    }
    if (settings.RUN_MMPBSA)
    {
        if (settings.RECEPTOR_MASK == "")
        {
            EPICFAIL = true;
            failure_log << "Keyword 'receptor_mask' cannot be empty if running MMPBSA." << std::endl;
        }
        if (settings.COMPLEX_MASK == "")
        {
            EPICFAIL = true;
            failure_log << "Keyword 'complex_mask' cannot be empty if running MMPBSA." << std::endl;
        }
        if (settings.LIGAND_MASK == "")
        {
            EPICFAIL = true;
            failure_log << "Keyword 'ligand_mask' cannot be empty if running MMPBSA." << std::endl;
        }
    }
    if (EPICFAIL)
    {
        std::cout << failure_log.str() << std::endl;
        return true;
    }
    return false;
}

bool mask_checks(JobSettings settings)
{
    bool EPICFAIL=false;
    std::stringstream failure_log;
    failure_log.str("");
   // Create mask check in cpptraj
    std::cout << "Validating provided selection masks." << std::endl;
    failure_log.str("");
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.COMPLEX_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"complex_mask\" is not valid for these input files." << std::endl;
    }
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.RECEPTOR_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"receptor_mask\" is not valid for these input files." << std::endl;
    }
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.LIGAND_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"ligand_mask\" is not valid for these input files." << std::endl;
    }
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.RMSD_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"rmsd_mask\" is not valid for these input files." << std::endl;
    }
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.RMSF_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"rmsf_mask\" is not valid for these input files." << std::endl;
    }
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.HBONDS_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"hbonds_mask\" is not valid for these input files." << std::endl;
    }
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.NORMAL_MODES_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"normal_modes_mask\" is not valid for these input files." << std::endl;
    }
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.CORREL_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"correl_mask\" is not valid for these input files." << std::endl;
    }
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.SOLVENT_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"solvent_mask\" is not valid for these input files." << std::endl;
    }
    if (cpptraj_mask_check(settings.PRMTOP, settings.INPCRD, settings.COUNTERIONS_MASK))
    {
        EPICFAIL = true;
        failure_log << "Keyword \"counterions_mask\" is not valid for these input files." << std::endl;
    }
    if (EPICFAIL)
    {
        std::cout << failure_log.str() << std::endl;
        return true;
    }
    return false;
}

bool generate_mmpbsa_inputs(JobSettings settings)
{
    slurm::update_job_name("Generating_MMPBSA_Inputs");
    // make MMPBSA_Inputs folder
    fs::create_directory("MMPBSA_Inputs/");

    // Generate complex.prmtop
    std::stringstream buffer;
    buffer.str("");
    buffer << "parm " << settings.PRMTOP << std::endl;
    buffer << "strip !" << settings.COMPLEX_MASK << std::endl;
    buffer << "outparm MMPBSA_Inputs/complex.prmtop" << std::endl << "quit" << std::endl;
    utils::write_to_file("parmed.in",buffer.str());
    utils::silent_shell("parmed -i parmed.in > /dev/null; rm parmed.in");

    // Generate receptor.prmtop
    buffer.str("");
    buffer << "parm " << settings.PRMTOP << std::endl;
    buffer << "strip !" << settings.RECEPTOR_MASK << std::endl;
    buffer << "outparm MMPBSA_Inputs/receptor.prmtop" << std::endl << "quit" << std::endl;
    utils::write_to_file("parmed.in",buffer.str());
    utils::silent_shell("parmed -i parmed.in > /dev/null; rm parmed.in");

    // Generate ligand.prmtop
    buffer.str("");
    buffer << "parm " << settings.PRMTOP << std::endl;
    buffer << "strip !" << settings.LIGAND_MASK << std::endl;
    buffer << "outparm MMPBSA_Inputs/ligand.prmtop" << std::endl << "quit" << std::endl;
    utils::write_to_file("parmed.in",buffer.str());
    utils::silent_shell("parmed -i parmed.in > /dev/null; rm parmed.in");

    // Generate mmpbsa.in
    buffer.str("");
    buffer << "Input file for running PB and GB" << std::endl;
    buffer << "&general" << std::endl;
    buffer << "   keep_files=1," << std::endl;
    buffer << "   entropy=0," << std::endl;
    buffer << "   ligand_mask=" << settings.LIGAND_MASK << "," << std::endl;
    buffer << "   strip_mask=!" << settings.COMPLEX_MASK << "," << std::endl;
    buffer << "   receptor_mask=" << settings.RECEPTOR_MASK << std::endl;
    buffer << "/" << std::endl;
    buffer << "&gb" << std::endl;
    buffer << "  igb=2, saltcon=" << settings.SALT_CONC << "," << std::endl;
    buffer << "/" << std::endl;
    buffer << "&decomp" << std::endl;
    buffer << " idecomp=1," << std::endl;
    buffer << " dec_verbose=1," << std::endl;
    buffer << "/" << std::endl;
    buffer << "" << std::endl;
    utils::write_to_file("MMPBSA_Inputs/mmpbsa.in",buffer.str());

    // check that all files exist
    if (!utils::CheckFileExists("MMPBSA_Inputs/complex.prmtop") || !utils::CheckFileExists("MMPBSA_Inputs/receptor.prmtop") || !utils::CheckFileExists("MMPBSA_Inputs/ligand.prmtop"))
    {
        std::cout << "ERROR:  failed to generate MMPBSA inputs." << std::endl;
        return true;
    }
    int n_com_atoms = stoi(utils::GetSysResponse("grep -A2 \"FLAG POINTERS\" MMPBSA_Inputs/complex.prmtop | tail -n 1 | awk '{print $1}'"));
    int n_rec_atoms = stoi(utils::GetSysResponse("grep -A2 \"FLAG POINTERS\" MMPBSA_Inputs/receptor.prmtop | tail -n 1 | awk '{print $1}'"));
    int n_lig_atoms = stoi(utils::GetSysResponse("grep -A2 \"FLAG POINTERS\" MMPBSA_Inputs/ligand.prmtop | tail -n 1 | awk '{print $1}'"));
    if (n_com_atoms != n_rec_atoms + n_lig_atoms)
    {
        std::cout << "Error:  Ligand + Receptor does not equal Complex!" << std::endl;
        return true;
    }
    return false;
}

namespace ambermachine
{
    void initialize(JobSettings settings,SlurmSettings slurm)
    {
        slurm::update_job_name("Validating_Input_Files");
        if (filechecks(settings))
        {
            // In here, something was wrong with the input files or the contents therein.
            return;
        }

        slurm::update_job_name("Validating_Amber_Selection_Masks");
        if (mask_checks(settings))
        {
            // In here, something was wrong about the provided amber selection masks with respect to the prmtop/inpcrd combination.
            return;
        }

        slurm::update_job_name("Generating_Replicate_Folder");
        // Generate our Replicate folder
        std::stringstream buffer;
        buffer.str("");
        buffer << "Replicate_" <<  get_replicate_count("./");
        fs::create_directory(buffer.str());
        fs::copy(settings.PRMTOP,buffer.str());
        fs::copy(settings.INPCRD,buffer.str());
        fs::copy("amberinput.in",buffer.str());
        fs::current_path(buffer.str());
        // make 06_Analysis/ subdirectory
        fs::create_directory("06_Analysis/");


        // if MMPBSA, generate MMPBSA inputs.
        if (settings.RUN_MMPBSA)
        {
            slurm::update_job_name("Generating_MMPBSA_Input_Files");

            if (generate_mmpbsa_inputs(settings))
            {
                //If this evaluates to true, it means there was a failure in the MMPBSA file generation and the job has been killed.
                return;
            }
        }

        // Generate Reporting Templates
        slurm::update_job_name("Generating_Report_Templates");
        latex::initialize_report(settings);
        
        // Compile Current Report
        latex::compile_report(settings);

        // Create .AMBER_INITIALIZE_COMPLETE
        utils::write_to_file(".AMBER_INITIALIZE_COMPLETE","");

        // Submit the next stage of the job: submit_minimize_job()
        slurm::submit_minimize_job(settings,slurm);

        // Cleanup
        slurm::cleanup_out_err(slurm);
    }

}