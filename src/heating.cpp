#include "ambermachine.h"

void write_mdin_heating(JobSettings settings)
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
Heating
 &cntrl
  ntx      = 7,
  irest    = 1,
  nsnb     = 1,
  ntpr     = 100,
  ntwx     = 1000,
  ntwv     = 00,
  nstlim   = 100000,
  t        = 0.00,
  dt       = 0.00100,
  ntc      = 2,
  ntf      = 1,
  ntb      = 1,
  iwrap    = 1,
  ntt      = 3, 
  temp0    = 010.0, 
  tempi    = 010.0, 
  tautp    = 1.0, 
  gamma_ln = 5.0,
  ntr      = 1,
  cut      = 10.0,
  nmropt   = 1 
 /
 &wt type='TEMP0', istep1=00000, istep2=05000, value1=000., value2=010.,  &end
 &wt type='TEMP0', istep1=05001, istep2=10000, value1=010., value2=020.,  &end
 &wt type='TEMP0', istep1=10001, istep2=20000, value1=020., value2=050.,  &end
 &wt type='TEMP0', istep1=20001, istep2=30000, value1=050., value2=100.,  &end
 &wt type='TEMP0', istep1=30001, istep2=40000, value1=100., value2=150.,  &end
 &wt type='TEMP0', istep1=40001, istep2=50000, value1=150., value2=200.,  &end
 &wt type='TEMP0', istep1=50001, istep2=60000, value1=200., value2=250.,  &end
 &wt type='TEMP0', istep1=60001, istep2=70000, value1=250., value2=300.,  &end
 &wt type='TEMP0', istep1=70001, istep2=80000, value1=300., value2=325.,  &end
 &wt type='TEMP0', istep1=80001,istep2=100000, value1=325., value2=300.,  &end
 &wt type='END'  &end
Hold molecule fixed
)HEATSCRIPT";
    buffer.str("");
    buffer << heat_script << std::endl;
    buffer << settings.MAX_RESTRAINT << std::endl;
    buffer << "RES " << startres << " " << endres << std::endl;
    buffer << "END" << std::endl;
    buffer << "END" << std::endl;
    utils::write_to_file("mdin.in",buffer.str());
    return;
}

namespace ambermachine
{
    void heating(JobSettings settings,SlurmSettings slurm)
    {
        std::string job_subdir = "03_Heating";
        std::string file_prefix = "heating";

        // Update report
        slurm::update_job_name("Updating_Report_Timeline");
        std::stringstream buffer;
        buffer.str("");
        buffer << "Iterative Heating & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
        buffer << "\\hline" << std::endl;
        utils::append_to_file("00_Report/timeline.tex",buffer.str());

        // create job subdirectory.
        std::experimental::filesystem::create_directory(job_subdir);

        // copy to /tmp
        std::experimental::filesystem::copy(settings.PRMTOP,"/tmp/job.prmtop");
        std::experimental::filesystem::copy("current_step.rst7","/tmp/last_step.rst7");

        // change directory to /tmp
        std::experimental::filesystem::current_path("/tmp/");
        write_mdin_heating(settings);

        // define necessary filenames
        std::stringstream lead_zero_number;
        lead_zero_number.str("");
        lead_zero_number << std::setw(4) << std::setfill('0') << settings.NUM_COLD_STEPS + 1;
        std::string mdin_file = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/" + job_subdir + "/" + file_prefix + "." + lead_zero_number.str() + ".in";
        std::string mdout_file = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/" + job_subdir + "/" + file_prefix + "." + lead_zero_number.str() + ".out";
        std::string restart_file = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/" + job_subdir + "/" + file_prefix + "." + lead_zero_number.str() + ".rst7";
        std::string trajectory_file = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/" + job_subdir + "/" + file_prefix + "." + lead_zero_number.str() + ".mdcrd";

        // Run Amber Job
        slurm::update_job_name("Performing_Iterative_Heating");

        std::cout << "DEBUG: In Heating Function, slurm_amber_module is:  " << slurm.SLURM_amber_module << std::endl;
        buffer.str("");
        buffer << "module load " << slurm.SLURM_amber_module << "; ";
        buffer << "$AMBERHOME/bin/pmemd.cuda -O -i mdin.in -o mdout.out -p job.prmtop -c last_step.rst7 -r current_step.rst7 -x trajectory.mdcrd -ref last_step.rst7";
        utils::silent_shell(buffer.str().c_str());

        // copy back from /tmp
        std::experimental::filesystem::copy("mdin.in",mdin_file);
        std::experimental::filesystem::remove("mdin.in");

        std::experimental::filesystem::copy("current_step.rst7",restart_file);
        std::experimental::filesystem::copy("current_step.rst7",(std::string)std::getenv("SLURM_SUBMIT_DIR")+"/current_step.rst7",std::experimental::filesystem::copy_options::update_existing);
        
        std::experimental::filesystem::copy("current_step.rst7","last_step.rst7",std::experimental::filesystem::copy_options::update_existing);
        std::experimental::filesystem::remove("current_step.rst7");

        std::experimental::filesystem::copy("mdout.out",mdout_file);
        std::experimental::filesystem::remove("mdout.out");
        
        std::experimental::filesystem::copy("trajectory.mdcrd",trajectory_file);
        std::experimental::filesystem::remove("trajectory.mdcrd");

        // return to original directory when finished in /tmp
        std::experimental::filesystem::current_path(std::getenv("SLURM_SUBMIT_DIR"));

        // Parse output to CSV file.
        std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
        csv_file += "/Heating.csv";
        utils::mdout_to_csv(mdout_file,csv_file);

        // Plot the Heating.csv using python ... 
        slurm::update_job_name("Generating_Plots_Heating");
        python::plot_csv_data("Heating.csv");

        // Update report with completed heating figures, checking if each one exists as we go.
        if (utils::CheckFileExists("00_Report/Heating_Figure_01.png"))
        {
        std::string report_update = R"LATEX(
    \begin{figure}[!htbp]
    \centering
    \includegraphics[width=0.9\textwidth]{Heating_Figure_01.png}
    \caption{Temperature (K), pressure (bar), volume ($\AA$), and density ($g\cdot mL^{-1}$) during iterative heating stage.}
    \label{fig:heating_fig_01}
    \end{figure}

    )LATEX";
        utils::append_to_file("00_Report/heating.tex",report_update);
        }
        if (utils::CheckFileExists("00_Report/Heating_Figure_02.png"))
        {
        std::string report_update = R"LATEX(
    \begin{figure}[!htbp]
    \centering
    \includegraphics[width=0.9\textwidth]{Heating_Figure_02.png}
    \caption{Total, kinetic, and potential energies ($kcal\cdot mol^{-1}$) during iterative heating stage.}
    \label{fig:heating_fig_02}
    \end{figure}

    )LATEX";
        utils::append_to_file("00_Report/heating.tex",report_update);
        }
        if (utils::CheckFileExists("00_Report/Heating_Figure_03.png"))
        {
        std::string report_update = R"LATEX(
    \begin{figure}[!htbp]
    \centering
    \includegraphics[width=0.9\textwidth]{Heating_Figure_03.png}
    \caption{Bond, angle, dihedral, van der Waals, and electrostatic energies ($kcal\cdot mol^{-1}$) during iterative heating stage.}
    \label{fig:heating_fig_03}
    \end{figure}

    )LATEX";
        utils::append_to_file("00_Report/heating.tex",report_update);
        }

        // Complete minimization job stage
        slurm::update_job_name("Completing_Iterative_Heating");
        // Compile Current Report
        latex::compile_report(settings);
        // Create .AMBER_HEATING_COMPLETE
        utils::write_to_file(".AMBER_HEATING_COMPLETE","");
        // Submit the next stage of the job: submit_hot_equil_job()
        slurm::submit_hot_equil_job(settings,slurm);
        // Cleanup
        slurm::cleanup_out_err(slurm);
        return;
    }
}
