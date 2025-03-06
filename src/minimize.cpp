#include "ambermachine.h"
void write_mdin_minimize(JobSettings settings)
{
    int colpos = settings.COMPLEX_MASK.find(":");
    int dashpos = settings.COMPLEX_MASK.find("-");
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
    
    std::stringstream buffer;
    buffer.str("");
    buffer << "    Remove Bad Contacts" << std::endl;
    buffer << "&cntrl" << std::endl;
    buffer << "  imin   = 1," << std::endl;
    buffer << "  maxcyc = 50000," << std::endl;
    buffer << "  ncyc   = 5000," << std::endl;
    buffer << "  ntc    = 2, " << std::endl;
    buffer << "  ntf    = 1," << std::endl;
    buffer << "  iwrap  = 1," << std::endl;
    buffer << "  ntb    = 1," << std::endl;
    buffer << "  ntr    = 1," << std::endl;
    buffer << "  cut    = 10" << std::endl;
    buffer << "   /" << std::endl;
    buffer << "Hold molecule fixed" << std::endl;
    buffer << settings.MAX_RESTRAINT << std::endl;
    buffer << "RES " << startres << " " << endres << std::endl;
    buffer << "END" << std::endl;
    buffer << "END" << std::endl;
    utils::write_to_file("mdin.in",buffer.str());
    return;
}

namespace ambermachine
{
    void minimize(JobSettings settings,SlurmSettings slurm)
    {
        std::cout << "Beginning Minimization Stage." << std::endl;
        std::string job_subdir = "01_Minimization";
        std::string file_prefix = "init";

        // Update report
        slurm::update_job_name("Updating_Report_Timeline");
        std::stringstream buffer;
        buffer.str("");
        buffer << "Minimization & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
        buffer << "\\hline" << std::endl;
        utils::append_to_file("00_Report/timeline.tex",buffer.str());

        // create mdin.in for minimization
        slurm::update_job_name("Running_Minimization");
        write_mdin_minimize(settings);

        // create 01_Minimization directory.
        fs::create_directory(job_subdir);

        // set filenames
        std::string mdin_file = job_subdir + "/" + file_prefix + ".0000.in";
        std::string mdout_file = job_subdir + "/" + file_prefix + ".0000.out";
        std::string restart_file = job_subdir + "/" + file_prefix + ".0000.rst7";
        std::string trajectory_file = job_subdir + "/" + file_prefix + ".0000.mdcrd";

        // load amber module, then run Amber (pmemd.cuda)
        buffer.str("");
        buffer << "scontrol show jobid -dd $SLURM_JOB_ID; ";
        buffer << "module load " << slurm.SLURM_amber_module << "; $AMBERHOME/bin/pmemd.cuda -O";
        buffer << " -i mdin.in";
        buffer << " -o " << mdout_file;
        buffer << " -p " << settings.PRMTOP;
        buffer << " -c " << settings.INPCRD;
        buffer << " -r current_step.rst7";
        buffer << " -x " << trajectory_file;
        buffer << " -ref " << settings.INPCRD;
        std::cout << std::endl << buffer.str() << std::endl << std::endl;
        utils::silent_shell(buffer.str().c_str());

        // move mdin.in to numbered file in appropriate folder
        fs::rename("mdin.in",mdin_file);
        if (utils::CheckFileExists("mdinfo"))
        {
            fs::remove("mdinfo");
        }

        buffer.str("");
        // Compress 01_Minimization/ to 01_Minimization.tar.gz, then remove the folder
        utils::compress_and_delete(job_subdir);
        // Complete minimization job stage
        slurm::update_job_name("Completing_Minimization");
        // Compile Current Report
        latex::compile_report(settings);
        // Create .AMBER_MINIMIZE_COMPLETE
        utils::write_to_file(".AMBER_MINIMIZE_COMPLETE","");
        // Submit the next stage of the job: submit_cold_equil_job()
        slurm::submit_cold_equil_job(settings,slurm);
        // Cleanup
        slurm::cleanup_out_err(slurm);
        return;
    }
}
