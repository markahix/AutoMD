#include "classicaldynamics.h"

void write_mdin_production(JobSettings settings)
{
    std::string heat_script = R"(
Molecular Dynamics Production
 &cntrl
  ntx      = 5,
  irest    = 1,
  nsnb     = 1,
  ntpr     = )" + std::to_string(settings.TRAJ_WRITE_FREQ) + R"(,
  ntwx     = )" + std::to_string(settings.TRAJ_WRITE_FREQ) + R"(,
  ntwv     = 00,
  ioutfm   = 1,
  nstlim   = 1000000,
  t        = 0.00,
  dt       = 0.00100,
  ntc      = 2,
  ntf      = 2,
  ntp      = 2,
  ntt      = 3,
  temp0    = )" + std::to_string(settings.TEMPERATURE) + R"(, 
  tempi    = )" + std::to_string(settings.TEMPERATURE) + R"(, 
  gamma_ln = 5.0, 
  vlimit   = -1.0,
  iwrap    = 1,
  cut      = 10.0,
  nmropt   = 1,
  vlimit   = -1,
 /
 &wt type='REST', istep1=000000,istep2=5000, &end
 &wt type='END'  &end /
)";
    utils::write_to_file("mdin.in",heat_script);
    return;
}

void ProductionLoop(JobSettings settings, SlurmSettings slurm, int startbead, std::string filebasename, FileList &files)
{
    for (int i=startbead; i < settings.NUM_PROD_STEPS; i++)
    {
        // change directory to /tmp
        fs::current_path("/tmp/");

        // create mdin.in for production
        std::stringstream jobname;
        jobname.str("");
        jobname << "Running_Production_Step_" << i+1 << "_of_" << settings.NUM_PROD_STEPS;
        slurm::update_job_name(jobname.str());
        write_mdin_production(settings);

        // set filenames
        GenerateFileNames(files, filebasename, i+1);

        // load amber module, then run Amber (pmemd.cuda)
        std::cout << "DEBUG: In Production Function, slurm_amber_module is:  " << slurm.SLURM_amber_module << std::endl;
        ambermachine::AmberLoopCUDA(slurm);

        // Error Check the output, terminate job if a step fails.
        std::ifstream outfile("mdout.out");
        std::string line;
        while (std::getline(outfile,line))
        {
            if (line.find("ERROR: Calculation halted.") != std::string::npos)
            {   
                std::stringstream buffer;
                std::cout << "Job Failed.  Terminating." << std::endl;
                buffer.str("");
                buffer << "scancel " << slurm.SLURM_JOB_ID;
                utils::silent_shell(buffer.str().c_str());
                return;
            }
        }

        // Collect latest nanosecond files back to main folder.
        std::map<std::string, std::string> main_file_list = {{"mdin.in",files.GetFile("mdin")},
                                                             {"current_step.rst7",files.GetFile("restart")},
                                                             {"mdout.out",files.GetFile("mdout")},
                                                             {"trajectory.mdcrd",files.GetFile("trajectory")},
                                                            };
        ambermachine::AmberCopyBack(main_file_list);
        utils::mdout_to_csv(files.GetFile("mdout"), files.GetFile("csv"));
        fs::current_path(std::getenv("SLURM_SUBMIT_DIR"));
        CallByNanosecondAnalytics(settings, slurm, files);
    }
}

void classical::RunClassicalDynamics(JobSettings settings, SlurmSettings slurm, FileList files)
{
    std::string filebasename = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/05_Production/prod.";
    std::string production_directory = "05_Production/";
    UpdateReport();

    // create job subdirectory.
    fs::create_directory(production_directory);

    //identify current bead
    int startbead = GetStartBead(production_directory);

    //ensure that matching rst7 is in the "current_step.rst7" position in the main directory.
    SetRestartFile(startbead, production_directory);
    
    // copy to /tmp
    fs::copy(settings.PRMTOP,"/tmp/job.prmtop");
    fs::copy("current_step.rst7","/tmp/last_step.rst7");
    fs::copy(settings.INPCRD,"/tmp/start_coords.rst7");

    // Loop over all production steps
    
    ProductionLoop(settings, slurm, startbead, filebasename, files);
    
    fs::remove("mdinfo");
    
    // Error Checking after finishing loop
    startbead = GetStartBead(production_directory);
    if (startbead != settings.NUM_PROD_STEPS)
    {
        error_log("ERROR:  Number of production dynamics steps does not match expectations.",1);
    }

    // Plot the Production.csv using python ... 
    GeneratePlotsAndReport(files);

    CompressProductionFolder(settings,production_directory);

    // Complete Production job stage
    slurm::update_job_name("Completing_Production");

    // Compile Current Report
    latex::compile_report(settings);
    
    // Create .AMBER_PRODUCTION_COMPLETE
    utils::write_to_file(".AMBER_PRODUCTION_COMPLETE","");
}