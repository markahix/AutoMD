#include "ambermachine.h"

void update_report()
{
    slurm::update_job_name("Updating_Report_Timeline");
    std::stringstream buffer;
    buffer.str("");
    buffer << "Minimization & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\ " << std::endl;
    buffer << "\\hline" << std::endl;
    utils::append_to_file("00_Report/timeline.tex",buffer.str());
}

void write_mdin_minimize(JobSettings settings)
{
    unsigned int colpos = settings.COMPLEX_MASK.find(":");
    unsigned int dashpos = settings.COMPLEX_MASK.find("-");
    std::string startres,endres;
    if (dashpos != std::string::npos)
    {
        startres = settings.COMPLEX_MASK.substr(colpos + 1, dashpos - colpos - 1);
        endres = settings.COMPLEX_MASK.substr(dashpos + 1, settings.COMPLEX_MASK.length());
    }
    else
    {
        startres = settings.COMPLEX_MASK.substr(colpos + 1, settings.COMPLEX_MASK.length());
        endres = settings.COMPLEX_MASK.substr(colpos + 1, settings.COMPLEX_MASK.length());
    } 
    slurm::update_job_name("Writing_AMBER_input_file");
    std::stringstream res_val;
    res_val.str("");
    res_val << std::fixed << std::setprecision(2) << std::setfill('0') << settings.MAX_RESTRAINT;
    std::string mdin_text = R"(    Remove Bad Contacts
&cntrl
  imin   = 1,
  maxcyc = 50000,
  ncyc   = 5000,
  ntc    = 2, 
  ntf    = 1,
  iwrap  = 1,
  ntb    = 1,
  ntr    = 1,
  cut    = 10
   /
END
)";
/*
Hold molecule fixed
)" + std::to_string(settings.MAX_RESTRAINT) + R"(
RES )" + startres + " " + endres + R"(
END
END
*/
    utils::write_to_file("01_Minimization/init.0000.in", mdin_text);
    return;
}

int main(int argc, char** argv)
{
    // Make sure I can actually RUN the classical dynamics simulations.
    if (!utils::CheckProgAvailable("pmemd"))
    {
        error_log("Unable to locate pmemd.  Make sure you have provided the correct Amber module.",1);
    }
    normal_log("Located pmemd at: ");
    std::string pmemd_loc = utils::GetSysResponse("which pmemd");
    normal_log("\t" + pmemd_loc);
    
    // Variable Declarations.
    JobSettings settings;
    SlurmSettings slurm;
    FileList files;
    slurm.SLURM_executable = argv[0];
    ambermachine::read_amberinput(settings,slurm);

    // Update report
    update_report();
    normal_log("Updating report with minimization job information");

    // create 01_Minimization directory.
    fs::create_directory("01_Minimization");

    // create mdin.in for minimization
    write_mdin_minimize(settings);

    // set filenames
    std::string mdout_file = "01_Minimization/init.0000.out";
    std::string restart_file = "01_Minimization/init.0000.rst7";
    std::string trajectory_file = "01_Minimization/init.0000.mdcrd";
    normal_log("MDOUT:      " + mdout_file);
    normal_log("RESTART:    " + restart_file);
    normal_log("TRAJECTORY: " + trajectory_file);

    // load amber module, then run Amber (pmemd.cuda)
    std::stringstream buffer;
    buffer.str("");
    buffer << "module load " << slurm.SLURM_amber_module << "; $AMBERHOME/bin/pmemd.cuda -O";
    buffer << " -i 01_Minimization/init.0000.in";
    buffer << " -o " << mdout_file;
    buffer << " -p " << settings.PRMTOP;
    buffer << " -c " << settings.INPCRD;
    buffer << " -r current_step.rst7";
    buffer << " -x " << trajectory_file;
    buffer << " -ref " << settings.INPCRD;
    slurm::update_job_name("Running_Minimization");
    normal_log("Running AMBER minimization...");
    utils::silent_shell(buffer.str().c_str());
    normal_log("AMBER minimization ended.");

    // remove mdinfo
    fs::remove("mdinfo");

    buffer.str("");

    // Compress 01_Minimization/ to 01_Minimization.tar.gz, then remove the folder
    slurm::update_job_name("Compressing_Minimization_Data");
    normal_log("Compressing AMBER minimization data...");
    utils::compress_and_delete("01_Minimization");
    normal_log("Compression complete.");

    // Compile Current Report
    latex::compile_report(settings);
    normal_log("Report compiled.");
    
    // Create .AMBER_MINIMIZE_COMPLETE
    utils::write_to_file(".AMBER_MINIMIZE_COMPLETE","");
    return 0;
}