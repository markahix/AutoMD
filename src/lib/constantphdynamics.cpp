#include "constantphdynamics.h"

/*
mdin production:
&cntrl
icnstph=2, ntcnstph=100,
   solvph=7.0, ntrelax=100, saltcon=0.1,
 /
*/

void write_mdin_production(JobSettings settings, double current_pH)
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
  nstlim   = 500000, 
  t        = 0.00,
  dt       = 0.00200,
  ntc      = 2,
  ntf      = 2,
  ntp      = 0,
  ntt      = 3,
  temp0    = )" + std::to_string(settings.TEMPERATURE) + R"(, 
  tempi    = )" + std::to_string(settings.TEMPERATURE) + R"(, 
  gamma_ln = 5.0, 
  vlimit   = -1.0,
  iwrap    = 1,
  cut      = 10.0,
  nmropt   = 1,
  vlimit   = -1,
  icnstph  = 2,
  ntcnstph = 100,
  solvph   = )" + std::to_string(current_pH) + R"(,
 /
 &wt type='REST', istep1=000000,istep2=5000, &end
 &wt type='END'  &end /
)";
    utils::write_to_file("mdin.in",heat_script);
    return;
}

void Generate_cpin_file(JobSettings settings)
{
    std::stringstream buffer;
    buffer.str("");
    buffer << "cpinutil.py -resname AS4 GL4 HIP -p " << settings.PRMTOP << " -o cpin.in -op cphmd.prmtop";
    utils::silent_shell(buffer.str().c_str());
    // Initial prmtop must have all titratable residues by name already in there from the PDB. ASP -> AS4, GLU -> GL4, HIE -> HIP
    /*
    cpinutil.py -resnames AS4 GL4 HIP -p <prmtop> -o <cpin.cpin> -op <newprmtop>
    
    //newprmtop is required for explicit solvation in CpH-MD
    // beware of limit of maximum 50 titratable residues for CpH-MD
    */
    if (!fs::exists("cphmd.prmtop"))
    {
        error_log("Unable to generate Constant pH inputs from provided files.  Check that your .prmtop has titratable residues.",1);
    }
}

bool JobFailed(std::string mdout_file)
{
    std::ifstream outfile(mdout_file,std::ios::in);
    if (!outfile.is_open())
    {
        return true;
    }
    std::string line;
    while (std::getline(outfile,line))
    {
        if (line.find("ERROR: Calculation halted.") != std::string::npos)
        {   
            outfile.close();
            return true;
        }
    }
    outfile.close();
    return false;
}

std::string GetPhProdFolderName(double ph_level)
{
    std::stringstream foldername;
    foldername.str("");
    foldername << "05_Production/pH_";
    foldername << std::fixed << std::setw(5) << std::setprecision(2) << std::setfill('0') << ph_level << "/";
    return fs::absolute(foldername.str());
}

void ConstantpHProductionLoop(JobSettings settings, SlurmSettings slurm, int startbead, std::string filebasename, FileList &files, double ph_level)
{
    for (int i=startbead; i < settings.NUM_PROD_STEPS; i++)
    {
        // change directory to /tmp
        fs::current_path("/tmp/");

        // create mdin.in for production
        std::stringstream jobname;
        jobname.str("");
        jobname << "Running_Constant_pH_Production_Step_" << i+1 << "_of_" << settings.NUM_PROD_STEPS;
        slurm::update_job_name(jobname.str());
        write_mdin_production(settings, ph_level);

        // set filenames
        GenerateFileNames(files, filebasename, i+1);

        // load amber module, then run Amber (pmemd.cuda)
        // std::cout << "DEBUG: In Production Function, slurm_amber_module is:  " << slurm.SLURM_amber_module << std::endl;
        ambermachine::AmberLoopCUDAConstpH(slurm);

        // Error Check the output, terminate job if a step fails.
        if (JobFailed("/tmp/mdout.out"))
        {
            std::stringstream buffer;
            error_log("Job Failed. Terminating.",1);
        }

        // Collect latest nanosecond files back to main folder.
        std::map<std::string, std::string> main_file_list = {{"mdin.in",files.GetFile("mdin")},
                                                             {"current_step.rst7",files.GetFile("restart")},
                                                             {"mdout.out",files.GetFile("mdout")},
                                                             {"trajectory.mdcrd",files.GetFile("trajectory")},
                                                             {"cpin.in",files.GetFile("cpin")},
                                                             {"cpout.out",files.GetFile("cpout")},
                                                             {"cpin.rstrt",files.GetFile("cprst")},
                                                             /*
                                                buffer << " -cpin ";
                                                buffer << " -cpout ";
                                                buffer << " -cprestrt ";*/
                                                            };
        ambermachine::AmberCopyBack(main_file_list);
        // utils::mdout_to_csv(files.GetFile("mdout"), files.GetFile("csv")); // This gets moved to AutoAnalytics
        fs::current_path(std::getenv("SLURM_SUBMIT_DIR"));
        CallByNanosecondAnalytics(settings, slurm, files);
    }
}

void constph::PrepareConstantpHDynamics(JobSettings settings, SlurmSettings slurm, FileList files)
{
    // Generate initial cpin file...
    Generate_cpin_file(settings);
    double current_ph = settings.MIN_PH_LEVEL;
    double max_ph = settings.MAX_PH_LEVEL;
        // currently in Replicate_X/ directory...
    fs::create_directory(settings.PRODUCTION_DIRECTORY);
    do
    {
        std::string foldername = GetPhProdFolderName(current_ph);
        if (!fs::is_directory(foldername))
        {
            // if folder already exists, just do the submit and move on.
            fs::create_directories(foldername);
            // put necessary files into starting directory
            // write_mdin_production(settings, current_ph);
            // fs::copy("mdin.in", foldername + "prod.in");
            fs::remove("mdin.in");
            fs::copy("cphmd.prmtop", foldername + "file.prmtop");
            fs::copy("current_step.rst7", foldername + "current_step.rst7");
        }
        else
        {
            normal_log("Identified existing constant pH directory at "+foldername + " . Submitting continuation of job.");
        }
        slurm::submit_const_ph_production_job(settings, slurm, current_ph);
        current_ph = current_ph + settings.PH_INCREMENT;
    } while (current_ph < max_ph + settings.PH_INCREMENT); 
}

void constph::RunConstantpHDynamics(JobSettings settings, SlurmSettings slurm, FileList files, double ph_level)
{
    settings.UpdateProductionDirectory(GetPhProdFolderName(ph_level));
    std::string filebasename = settings.PRODUCTION_DIRECTORY + "prod.";

    // FINISH THIS FUNCTIONALITY
    // copy current files to /tmp/ -- INCLUDE CPIN and cphmd.prmtop (new prmtop made by cpinutil.py)

    // run AmberLoopCUDAConstpH(SlurmSettings slurm) for each timestep... So that means building a ConstPHProductionLoop

    UpdateReport();


    //identify current bead
    int startbead = GetStartBead(settings.PRODUCTION_DIRECTORY);

    //ensure that matching rst7 is in the "current_step.rst7" position in the main directory.
    SetRestartFile(startbead, settings.PRODUCTION_DIRECTORY);
    
    // copy to /tmp
    fs::copy("cpin.in","/tmp/cpin.in");
    fs::copy(settings.PRODUCTION_DIRECTORY + "file.prmtop","/tmp/job.prmtop");
    fs::copy(settings.PRODUCTION_DIRECTORY + "current_step.rst7","/tmp/last_step.rst7");
    // fs::copy(settings.PRODUCTION_DIRECTORY + "prod.in","/tmp/mdin.in");
    fs::copy(settings.INPCRD,"/tmp/start_coords.rst7");

    // Loop over all production steps
    ConstantpHProductionLoop(settings, slurm, startbead, filebasename, files, ph_level);
    
    fs::remove("mdinfo");
    
    // Error Checking after finishing loop
    startbead = GetStartBead(settings.PRODUCTION_DIRECTORY);
    if (startbead != settings.NUM_PROD_STEPS)
    {
        error_log("ERROR:  Number of production dynamics steps does not match expectations.",1);
    }

    CompressProductionFolder(settings);

    // Complete Production job stage
    slurm::update_job_name("Completing_Production");

    // Compile Current Report
    latex::compile_report(settings);
    
    // Create .AMBER_PRODUCTION_COMPLETE
    utils::write_to_file(".AMBER_PRODUCTION_COMPLETE","");
}

