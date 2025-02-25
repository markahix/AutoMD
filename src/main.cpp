/* 
Source code for the Amber Machine 
- Automated Amber job runner designed for use with the Wayne State University HPC ("Grid") )
*/
#include "ambermachine.h"

int main(int argc, char **argv)
{
    // utils::splash_screen();

    // if we're missing the amberinput.in, create the template, inform the user, and exit immediately.
    if ( !utils::CheckFileExists("amberinput.in") )
    {        
        std::cout << "Input file was not found in this directory.  Generating template.  Please edit this template for your specific job before rerunning this program." << std::endl;
        ambermachine::write_amberinput();
        return 0;
    }

    JobSettings settings;
    SlurmSettings slurm;
    slurm.SLURM_executable = argv[0];

    // Validate amberinput.in (done for each job stage to ensure the file has not been corrupted and update settings
    // if it has been changed in a manner that will affect subsequent stages (such as changing n_prod_steps)
    if (ambermachine::read_amberinput(settings,slurm))
    {
        std::cout << "Unable to parse amberinput.in file.  Terminating." << std::endl;
        return 0;
    }

    // If we're called without flags, figure out where we are and continue from there.
    if (argc == 1)
    {
        ambermachine::identify_current_state(settings,slurm);
        return 0;
    }

    // Make sure this program is running on a node, not on the login (hostname == 'warrior' is BAD.)
    std::string hostname = utils::GetSysResponse((char*)"hostname");
    if (hostname == "warrior")
    {
        std::cout << "Error:  Flagged job attempting to run on login node (warrior). Resubmitting to queue instead." << std::endl;
        std::stringstream buffer;
        buffer.str("");
        buffer << slurm.SLURM_executable;
        utils::silent_shell(buffer.str().c_str());
        return 0;
    }


    // Move to $SLURM_SUBMIT_DIR (should already be there, but just in case...)
    fs::current_path(std::getenv("SLURM_SUBMIT_DIR"));
    slurm.SLURM_JOB_ID = std::getenv("SLURM_JOB_ID");

    // Identify the correct function to call based on provided command line flag.
    if ((std::string)argv[1] == "--initialize")
    {
        ambermachine::initialize(settings,slurm);
        return 0;
    }

    if ((std::string)argv[1] == "--minimize")
    {
        ambermachine::minimize(settings,slurm);
        return 0;
    }

    if ((std::string)argv[1] == "--coldrelax")
    {
        ambermachine::cold_equilibration(settings,slurm);
        return 0;
    }

    if ((std::string)argv[1] == "--heating")
    {
        ambermachine::heating(settings,slurm);
        return 0;
    }

    if ((std::string)argv[1] == "--hotrelax")
    {
        ambermachine::hot_equilibration(settings,slurm);
        return 0;
    }

    if ((std::string)argv[1] == "--production")
    {
        ambermachine::production(settings,slurm);
        return 0;
    }

    if ((std::string)argv[1] == "--mmpbsa")
    {
        std::string trajectory = (std::string)argv[2];
        ambermachine::mmpbsa(settings,slurm,trajectory);
        return 0;
    }

    if ((std::string)argv[1] == "--sasa")
    {
        std::string trajectory = (std::string)argv[2];
        ambermachine::sasa(settings,slurm,trajectory);
        return 0;
    }

    if ((std::string)argv[1] == "--analysis")
    {
        ambermachine::analysis(settings,slurm);
        return 0;
    }

    if ((std::string)argv[1] == "--report")
    {
        // ambermachine::submit_reporting_job(settings,slurm);
        return 0;
    }

    //If we get past all recognized input flags and still haven't figured out where to go, it's time to bail out.
    std::cout << "Unrecognized command line flag.  Check your inputs and try again." << std::endl;
    return 0;
}