#include "ambermachine.h"

void write_mdin_production(JobSettings settings)
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
    std::string heat_script = R"HEATSCRIPT(
Molecular Dynamics Production
 &cntrl
  ntx      = 5,
  irest    = 1,
  nsnb     = 1,
  ntpr     = 100,
  ntwx     = 1000,
  ntwv     = 00,
  ioutfm   = 1,
  nstlim   = 1000000,
  t        = 0.00,
  dt       = 0.00100,
  ntc      = 2,
  ntf      = 2,
  ntp      = 2,
  ntt      = 3,
  temp0    = 300.0, 
  tempi    = 300.0, 
  gamma_ln = 5.0, 
  vlimit   = -1.0,
  iwrap    = 1,
  cut      = 10.0,
  nmropt   = 1,
  vlimit   = -1,
 /
 &wt type='REST', istep1=000000,istep2=5000, &end
 &wt type='END'  &end /
)HEATSCRIPT";
    buffer.str("");
    buffer << heat_script << std::endl;
    utils::write_to_file("mdin.in",buffer.str());
    return;
}

namespace ambermachine
{
    void production(JobSettings settings,SlurmSettings slurm)
    {
        std::string job_subdir = "05_Production";
        std::string file_prefix = "prod";

        // Update report
        slurm::update_job_name("Updating_Report_Timeline");
        std::stringstream buffer;
        buffer.str("");
        buffer << "Production Dynamics & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
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
            fs::copy(restart_file,"current_step.rst7",fs::copy_options::update_existing);
        }
        
        // copy to /tmp
        fs::copy(settings.PRMTOP,"/tmp/job.prmtop");
        fs::copy("current_step.rst7","/tmp/last_step.rst7");

        // Loop over all hot steps
        for (int i=startbead; i < settings.NUM_PROD_STEPS; i++)
        {
            // change directory to /tmp
            fs::current_path("/tmp/");

            // create mdin.in for hot equilibration
            std::stringstream jobname;
            jobname.str("");
            jobname << "Running_Production_Step_" << i+1 << "_of_" << settings.NUM_PROD_STEPS;
            slurm::update_job_name(jobname.str());
            write_mdin_production(settings);

            // set filenames
            std::stringstream lead_zero_number;
            lead_zero_number.str("");
            lead_zero_number << std::setw(4) << std::setfill('0') << i+1;
            std::string mdin_file = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/" + job_subdir + "/mdin.prod";
            std::string mdout_file = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/" + job_subdir + "/" + file_prefix + "." + lead_zero_number.str() + ".out";
            std::string restart_file = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/" + job_subdir + "/" + file_prefix + "." + lead_zero_number.str() + ".rst7";
            std::string trajectory_file = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/" + job_subdir + "/" + file_prefix + "." + lead_zero_number.str() + ".mdcrd";

            // load amber module, then run Amber (pmemd.cuda)
            std::cout << "DEBUG: In Production Function, slurm_amber_module is:  " << slurm.SLURM_amber_module << std::endl;
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

            // Error Check the output, terminate job if a step fails.
            std::ifstream outfile("mdout.out");
            std::string line;
            while (std::getline(outfile,line))
            {
                if (line.find("ERROR: Calculation halted.") != std::string::npos)
                {
                    std::cout << "Job Failed.  Terminating." << std::endl;
                    buffer.str("");
                    buffer << "scancel "<<slurm.SLURM_JOB_ID;
                    utils::silent_shell(buffer.str().c_str());
                    return;
                }
            }

            // copy back from /tmp
            fs::copy("mdin.in",mdin_file,fs::copy_options::update_existing);
            fs::remove("mdin.in");

            fs::copy("current_step.rst7",restart_file);
            fs::copy("current_step.rst7",(std::string)std::getenv("SLURM_SUBMIT_DIR")+"/current_step.rst7",fs::copy_options::update_existing);
            
            fs::copy("current_step.rst7","last_step.rst7",fs::copy_options::update_existing);
            fs::remove("current_step.rst7");

            fs::copy("mdout.out",mdout_file);
            fs::remove("mdout.out");
            
            fs::copy("trajectory.mdcrd",trajectory_file);
            fs::remove("trajectory.mdcrd");
            
            // Parse mdout into Production.csv
            std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
            csv_file += "/06_Analysis/Production.csv";

            utils::mdout_to_csv(mdout_file,csv_file);

            // Check if MMPBSA job, spawn if necessary.
            if (settings.RUN_MMPBSA)
            {
                slurm::submit_mmpbsa_job(settings, slurm, trajectory_file);
            }          

            // Check if SASA job, spawn if necessary.
            if (settings.RECEPTOR_MASK != " " && settings.LIGAND_MASK != " " && settings.COMPLEX_MASK != " ")
            {
                slurm::submit_sasa_job(settings, slurm, trajectory_file);
            }
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
        if (startbead != settings.NUM_PROD_STEPS)
        {
            std::cout << "ERROR:  Number of production dynamics steps does not match expectations." << std::endl;
            return;
        }

        // Plot the Production.csv using python ... 
        slurm::update_job_name("Generating_Plots_Production");
        python::plot_csv_data("06_Analysis/Production.csv");

        // Update report with completed hot equilibration figures, checking if each one exists as we go.
        if (utils::CheckFileExists("00_Report/Production_Figure_01.png"))
        {
        std::string report_update = R"LATEX(
    \begin{figure}[!htbp]
    \centering
    \includegraphics[width=0.9\textwidth]{Production_Figure_01.png}
    \caption{Temperature (K), pressure (bar), volume ($\AA$), and density ($g\cdot mL^{-1}$) during production dynamics stage.}
    \label{fig:production_fig_01}
    \end{figure}

    )LATEX";
        utils::append_to_file("00_Report/production.tex",report_update);
        }
        if (utils::CheckFileExists("00_Report/Production_Figure_02.png"))
        {
        std::string report_update = R"LATEX(
    \begin{figure}[!htbp]
    \centering
    \includegraphics[width=0.9\textwidth]{Production_Figure_02.png}
    \caption{Total, kinetic, and potential energies ($kcal\cdot mol^{-1}$) during production dynamics stage.}
    \label{fig:production_fig_02}
    \end{figure}

    )LATEX";
        utils::append_to_file("00_Report/production.tex",report_update);
        }
        if (utils::CheckFileExists("00_Report/Production_Figure_03.png"))
        {
        std::string report_update = R"LATEX(
    \begin{figure}[!htbp]
    \centering
    \includegraphics[width=0.9\textwidth]{Production_Figure_03.png}
    \caption{Bond, angle, dihedral, van der Waals, and electrostatic energies ($kcal\cdot mol^{-1}$) during production dynamics stage.}
    \label{fig:production_fig_03}
    \end{figure}

    )LATEX";
        utils::append_to_file("00_Report/production.tex",report_update);
        }

        // Complete hot equilibration job stage
        slurm::update_job_name("Completing_Production");
        // Compile Current Report
        latex::compile_report(settings);
        // Create .AMBER_PRODUCTION_COMPLETE
        utils::write_to_file(".AMBER_PRODUCTION_COMPLETE","");
        // Submit the next stage of the job: submit_analysis_job()
        slurm::submit_analysis_job(settings,slurm);
        // Cleanup
        slurm::cleanup_out_err(slurm);
        return;
    }
}