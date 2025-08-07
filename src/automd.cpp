/* 
Source code for AutoMD 
- Automated Amber job runner designed for use with the Wayne State University HPC ("Grid") )
*/
#include "ambermachine.h"

std::string GetReplicateDirectory(std::string path)
{
    int r = 0;
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

    std::stringstream rep_dir;
    rep_dir.str("");
    rep_dir << "Replicate_" << r << "/";
    return rep_dir.str();
}

void hostname_check(SlurmSettings slurm)
{
    std::string hostname = utils::GetSysResponse((char*)"hostname");
    if (hostname == "warrior")
    {
        utils::silent_shell(slurm.SLURM_executable.c_str());
        error_log("Error:  Flagged job attempting to run on login node (warrior). Resubmitting to queue instead.",1);
    }
}

int main(int argc, char **argv)
{
    // utils::splash_screen();

    JobSettings settings;
    SlurmSettings slurm;
    slurm.SLURM_executable = argv[0];

    // Validate amberinput.in (done for each job stage to ensure the file has not been corrupted and update settings
    // if it has been changed in a manner that will affect subsequent stages (such as changing n_prod_steps)
    ambermachine::read_amberinput(settings,slurm);

    // If we're called without flags, figure out where we are and continue from there.
    if (argc == 1)
    {
        ambermachine::identify_current_state(settings,slurm);
        return 0;
    }

    // Make sure this program is running on a node, not on the login (hostname == 'warrior' is BAD.)
    hostname_check(slurm);

    // Move to $SLURM_SUBMIT_DIR (should already be there, but just in case...)
    fs::current_path(std::getenv("SLURM_SUBMIT_DIR"));
    slurm.SLURM_JOB_ID = std::getenv("SLURM_JOB_ID");

    std::string jobflag = (std::string)argv[1];
    // Identify the correct function to call based on provided command line flag.
    if (jobflag == "--initialize")
    {
        utils::silent_shell("automd_initialize");
        if (!fs::exists(".AMBER_INITIALIZE_COMPLETE"))
        {
            error_log("Failed to initialize simulation.  Please check error logs",1);
        }
        fs::current_path(GetReplicateDirectory("./"));
        slurm::submit_minimize_job(settings,slurm);
        slurm::cleanup_out_err(slurm);
        return 0;
    }

    if (jobflag == "--minimize")
    {
        utils::silent_shell("automd_minimize");
        if (!fs::exists(".AMBER_MINIMIZE_COMPLETE"))
        {
            error_log("Failed to complete minimization.  Please check error logs",1);
        }
        slurm::submit_cold_equil_job(settings,slurm);
        slurm::cleanup_out_err(slurm);
        return 0;
    }

    if (jobflag == "--coldrelax")
    {
        utils::silent_shell("automd_coldequilibrate");
        if (!fs::exists(".AMBER_COLD_RELAX_COMPLETE"))
        {
            error_log("Failed to complete cold density equilibration.  Please check error logs",1);
        }
        slurm::submit_heating_job(settings,slurm);
        slurm::cleanup_out_err(slurm);
        return 0;
    }

    if (jobflag == "--heating")
    {
        utils::silent_shell("automd_heating");
        if (!fs::exists(".AMBER_HEATING_COMPLETE"))
        {
            error_log("Failed to complete heating.  Please check error logs",1);
        }
        slurm::submit_hot_equil_job(settings,slurm);
        slurm::cleanup_out_err(slurm);
        return 0;
    }

    if (jobflag == "--hotrelax")
    {
        utils::silent_shell("automd_hotequilibrate");
        if (!fs::exists(".AMBER_HOT_RELAX_COMPLETE"))
        {
            error_log("Failed to complete hot density equilibration.  Please check error logs",1);
        }
        slurm::submit_production_job(settings,slurm);
        slurm::cleanup_out_err(slurm);    
        return 0;
    }

    if (jobflag == "--production")
    {
        utils::silent_shell("automd_production");
        if (!fs::exists(".AMBER_PRODUCTION_COMPLETE"))
        {
            error_log("Failed to complete production.  Please check error logs",1);
        }
        slurm::submit_analysis_job(settings,slurm);
        slurm::cleanup_out_err(slurm);
        return 0;
    }

    if (jobflag == "--sasa")
    {
        std::string trajectory = (std::string)argv[2];
        ambermachine::sasa(settings,slurm,trajectory);
        return 0;
    }

    if (jobflag == "--analysis")
    {
        ambermachine::analysis(settings,slurm);
        return 0;
    }

    if (jobflag == "--report")
    {
        // ambermachine::submit_reporting_job(settings,slurm);
        return 0;
    }

    //If we get past all recognized input flags and still haven't figured out where to go, it's time to bail out.
    std::cout << "Unrecognized command line flag.  Check your inputs and try again." << std::endl;
    return 0;
}