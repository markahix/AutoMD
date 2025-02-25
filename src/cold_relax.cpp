#include "ambermachine.h"

void write_mdin_cold_relax(JobSettings settings, double restraint_value)
{
    int colpos = settings.COMPLEX_MASK.find(":");
    int dashpos = settings.COMPLEX_MASK.find("-");
    std::string startres,endres;
    if (dashpos != std::string::npos)
    {
        startres = settings.COMPLEX_MASK.substr(colpos + 1, dashpos - colpos);
        endres = settings.COMPLEX_MASK.substr(dashpos + 1, settings.COMPLEX_MASK.length());
    }
    else
    {
        startres = settings.COMPLEX_MASK.substr(colpos + 1, settings.COMPLEX_MASK.length());
        endres = settings.COMPLEX_MASK.substr(colpos + 1, settings.COMPLEX_MASK.length());
    } 
    std::stringstream buffer;
    buffer.str("");
    buffer << "Cold Density Equilibration" << std::endl;
    buffer << "&cntrl" << std::endl;
    buffer << "  ntx      = 1," << std::endl;
    buffer << "  irest    = 0," << std::endl;
    buffer << "  ntpr     = 5," << std::endl;
    buffer << "  ntwx     = 100," << std::endl;
    buffer << "  ntwv     = 00," << std::endl;
    buffer << "  nstlim   = 1000," << std::endl;
    buffer << "  t        = 0.00," << std::endl;
    buffer << "  dt       = 0.00100," << std::endl;
    buffer << "  ntc      = 2," << std::endl;
    buffer << "  ntf      = 1," << std::endl;
    buffer << "  ntb      = 2," << std::endl;
    buffer << "  ntp      = 2," << std::endl;
    buffer << "  iwrap    = 1," << std::endl;
    buffer << "  ig       = -1," << std::endl;
    buffer << "  ntt      = 3," << std::endl;
    buffer << "  temp0    = 010.0," << std::endl;
    buffer << "  tempi    = 010.0," << std::endl;
    buffer << "  tautp    = 1.0," << std::endl;
    buffer << "  gamma_ln = 5.0," << std::endl;
    buffer << "  ntr      = 1," << std::endl;
    buffer << "  cut      = 10.0," << std::endl;
    buffer << "   /" << std::endl;
    buffer << "Hold molecule fixed" << std::endl;
    buffer << restraint_value << std::endl;
    buffer << "RES " << startres << " " << endres << std::endl;
    buffer << "END" << std::endl;
    buffer << "END" << std::endl;
    utils::write_to_file("mdin.in",buffer.str());
    return;
}

namespace ambermachine
{
    void cold_equilibration(JobSettings settings,SlurmSettings slurm)
    {
        std::string job_subdir = "02_ColdDensityEquilibration";
        std::string file_prefix = "cold_equil";
        double restraint_value;
        double rest_interval = settings.MAX_RESTRAINT / (settings.NUM_COLD_STEPS - 1);

        // Update report
        slurm::update_job_name("Updating_Report_Timeline");
        std::stringstream buffer;
        buffer.str("");
        buffer << "Cold Density Equilibration & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
        buffer << "\\hline" << std::endl;
        utils::append_to_file("00_Report/timeline.tex",buffer.str());

        // create job subdirectory.
        fs::create_directory(job_subdir);

        //identify current bead
        int startbead = 0;
        for (fs::path p : fs::directory_iterator(job_subdir))
        {
            if (p.extension() == ".mdcrd")
            {
                startbead++;
            }
        }

        //ensure that matching rst7 is in the "current_step.rst7" position in the main directory.
        if (startbead > 0)
        {
            std::stringstream lead_zero_number;
            lead_zero_number.str("");
            lead_zero_number << std::setw(4) << std::setfill('0') << startbead;
            std::string restart_file = job_subdir + "/" + file_prefix + "." + lead_zero_number.str() + ".rst7";
            fs::copy(restart_file,"current_step.rst7");
        }
        
        // copy to /tmp
        fs::copy(settings.PRMTOP,"/tmp/job.prmtop");
        fs::copy("current_step.rst7","/tmp/last_step.rst7");

        std::string filebasename = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/" + job_subdir + "/" + file_prefix + ".";
        // Loop over all cold steps
        for (int i=startbead; i < settings.NUM_COLD_STEPS; i++)
        {
            // change directory to /tmp
            fs::current_path("/tmp/");

            // set current restraint value
            restraint_value = settings.MAX_RESTRAINT - (i * rest_interval);
            if (restraint_value < 0.0)
            {
                restraint_value = 0.0;
            }

            // create mdin.in for cold equilibration
            std::stringstream jobname;
            jobname.str("");
            jobname << "Cold_Equilibration_Step_" << i+1 << "_of_" << settings.NUM_COLD_STEPS;
            slurm::update_job_name(jobname.str());
            write_mdin_cold_relax(settings,restraint_value);

            // set filenames
            std::stringstream lead_zero_number;
            lead_zero_number.str("");
            lead_zero_number << std::setw(4) << std::setfill('0') << i+1;
            std::string mdin_file = filebasename+ lead_zero_number.str() + ".in";
            std::string mdout_file = filebasename + lead_zero_number.str() + ".out";
            std::string restart_file = filebasename + lead_zero_number.str() + ".rst7";
            std::string trajectory_file = filebasename + lead_zero_number.str() + ".mdcrd";

            // load amber module, then run Amber (pmemd.cuda)
            std::cout << "DEBUG: In ColdRelax Function, slurm_amber_module is:  " << slurm.SLURM_amber_module << std::endl;
            buffer.str("");
            buffer << "module load " << slurm.SLURM_amber_module << "; $AMBERHOME/bin/pmemd.cuda -O";
            buffer << " -i mdin.in";
            buffer << " -o mdout.out";
            buffer << " -p job.prmtop";
            buffer << " -c last_step.rst7";
            buffer << " -r current_step.rst7";
            buffer << " -x trajectory.mdcrd";
            buffer << " -ref last_step.rst7";
            utils::silent_shell(buffer.str().c_str());

            // copy back from /tmp
            fs::copy("mdin.in",mdin_file);
            fs::remove("mdin.in");

            fs::copy("current_step.rst7",restart_file);
            fs::copy("current_step.rst7",(std::string)std::getenv("SLURM_SUBMIT_DIR")+"/current_step.rst7",fs::copy_options::update_existing);
            
            fs::copy("current_step.rst7","last_step.rst7",fs::copy_options::update_existing);
            fs::remove("current_step.rst7");

            fs::copy("mdout.out",mdout_file);
            fs::remove("mdout.out");
            
            fs::copy("trajectory.mdcrd",trajectory_file);
            fs::remove("trajectory.mdcrd");
            
            // Parse mdout into ColdEquil.csv
            std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
            csv_file += "/06_Analysis/ColdEquil.csv";

            utils::mdout_to_csv(mdout_file,csv_file);
        }
        
        // return to original directory when finished in /tmp
        fs::current_path(std::getenv("SLURM_SUBMIT_DIR"));

        if (utils::CheckFileExists("mdinfo"))
        {
            fs::remove("mdinfo");
        }

        // Error Checking after finishing loop
        startbead = 0;
        for (fs::path p : fs::directory_iterator(job_subdir))
        {
            if (p.extension() == ".mdcrd")
            {
                startbead++;
            }
        }
        if (startbead != settings.NUM_COLD_STEPS)
        {
            std::cout << "ERROR:  Number of cold density equilibration steps does not match expectations." << std::endl;
            return;
        }

        // Plot the ColdEquil.csv using python ... 
        slurm::update_job_name("Generating_Plots_Cold_Equilibration");
        python::plot_csv_data("06_Analysis/ColdEquil.csv");

        // Update report with completed cold equilibration figures, checking if each one exists as we go.
        if (utils::CheckFileExists("00_Report/ColdEquil_Figure_01.png"))
        {
        std::string report_update = R"LATEX(
    \begin{figure}[!htbp]
    \centering
    \includegraphics[width=0.9\textwidth]{ColdEquil_Figure_01.png}
    \caption{Temperature (K), pressure (bar), volume ($\AA$), and density ($g\cdot mL^{-1}$) during cold density equilibration stage.}
    \label{fig:cold_equil_fig_01}
    \end{figure}

    )LATEX";
        utils::append_to_file("00_Report/cold_equil.tex",report_update);
        }
        if (utils::CheckFileExists("00_Report/ColdEquil_Figure_02.png"))
        {
        std::string report_update = R"LATEX(
    \begin{figure}[!htbp]
    \centering
    \includegraphics[width=0.9\textwidth]{ColdEquil_Figure_02.png}
    \caption{Total, kinetic, and potential energies ($kcal\cdot mol^{-1}$) during cold density equilibration stage.}
    \label{fig:cold_equil_fig_02}
    \end{figure}

    )LATEX";
        utils::append_to_file("00_Report/cold_equil.tex",report_update);
        }
        if (utils::CheckFileExists("00_Report/ColdEquil_Figure_03.png"))
        {
        std::string report_update = R"LATEX(
    \begin{figure}[!htbp]
    \centering
    \includegraphics[width=0.9\textwidth]{ColdEquil_Figure_03.png}
    \caption{Bond, angle, dihedral, van der Waals, and electrostatic energies ($kcal\cdot mol^{-1}$) during cold density equilibration stage.}
    \label{fig:cold_equil_fig_03}
    \end{figure}

    )LATEX";
        utils::append_to_file("00_Report/cold_equil.tex",report_update);
        }

        // Complete minimization job stage
        slurm::update_job_name("Completing_Cold_Equilibration");
        // Compile Current Report
        latex::compile_report(settings);
        // Create .AMBER_COLD_RELAX_COMPLETE
        utils::write_to_file(".AMBER_COLD_RELAX_COMPLETE","");
        // Submit the next stage of the job: submit_heating_job()
        slurm::submit_heating_job(settings,slurm);
        // Cleanup
        slurm::cleanup_out_err(slurm);
        return;
    }
}