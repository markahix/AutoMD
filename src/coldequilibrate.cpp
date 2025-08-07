#include "ambermachine.h"

void write_mdin_cold_relax(JobSettings settings, double restraint_value)
{
    unsigned int colpos = settings.COMPLEX_MASK.find(":");
    unsigned int dashpos = settings.COMPLEX_MASK.find("-");
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
    std::string mdin_text = R"(Cold Density Equilibration
&cntrl
  ntx      = 1,
  irest    = 0,
  ntpr     = 5,
  ntwx     = 100,
  ntwv     = 00,
  nstlim   = 1000,
  t        = 0.00,
  dt       = 0.00100,
  ntc      = 2,
  ntf      = 1,
  ntb      = 2,
  ntp      = 2,
  iwrap    = 1,
  ig       = -1,
  ntt      = 3,
  temp0    = 010.0,
  tempi    = 010.0,
  tautp    = 1.0,
  gamma_ln = 5.0,
  ntr      = 1,
  cut      = 10.0,
   /
Hold molecule fixed
)" + std::to_string(restraint_value) + R"(
RES )" + startres + " " + endres + R"(
END
END
)";
    utils::write_to_file("mdin.in", mdin_text);
    return;
}

void update_report()
{
    slurm::update_job_name("Updating_Report_Timeline");
    std::stringstream buffer;
    buffer.str("");
    buffer << "Cold Density Equilibration & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    utils::append_to_file("00_Report/timeline.tex",buffer.str());
}

int GetStartBead()
{
    int startbead = 0;
    for (fs::path p : fs::directory_iterator("02_ColdDensityEquilibration/"))
    {
        if (p.extension() == ".mdcrd")
        {
            startbead++;
        }
    }
    
    return startbead;
}

void SetRestartFile(int startbead)
{
    //ensure that matching rst7 is in the "current_step.rst7" position in the main directory.
    if (startbead > 0)
    {
        std::stringstream lead_zero_number;
        lead_zero_number.str("");
        lead_zero_number << std::setw(4) << std::setfill('0') << startbead;
        std::string restart_file = "02_ColdDensityEquilibration/cold_equil." + lead_zero_number.str() + ".rst7";
        fs::copy(restart_file,"current_step.rst7");
    }
}

void cold_equil_loop(JobSettings settings, SlurmSettings slurm, int startbead, std::string filebasename, std::string csv_file)
{
    // Loop over all cold steps
    double restraint_value;
    double rest_interval = settings.MAX_RESTRAINT / (settings.NUM_COLD_STEPS - 1);
    

    for (int i=startbead; i < settings.NUM_COLD_STEPS; i++)
    {
        
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
        ambermachine::AmberLoop(slurm);
        ambermachine::AmberCopyBack(mdin_file,restart_file,mdout_file,trajectory_file,csv_file);
    }
}

void GenerateFiguresAndReport()
{
// Plot the ColdEquil.csv using python ... 
    slurm::update_job_name("Generating_Plots_Cold_Equilibration");
    python::plot_csv_data("06_Analysis/ColdEquil.csv");

    // Update report with completed cold equilibration figures, checking if each one exists as we go.
    if (fs::exists("00_Report/ColdEquil_Figure_01.png"))
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
    if (fs::exists("00_Report/ColdEquil_Figure_02.png"))
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
    if (fs::exists("00_Report/ColdEquil_Figure_03.png"))
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

}

int main(int argc, char** argv)
{
    // Make sure I can actually RUN the classical dynamics simulations.
    if (! utils::CheckProgAvailable("$AMBERHOME/bin/pmemd.cuda"))
    {
        error_log("Unable to locate pmemd.cuda.  Make sure you have provided the correct Amber module.",1);
    }
    
    // Variable Declarations.
    JobSettings settings;
    SlurmSettings slurm;
    slurm.SLURM_executable = argv[0];
    ambermachine::read_amberinput(settings,slurm);
    
    // Update report
    update_report();

    // create 02_ColdDensityEquilibration directory.
    fs::create_directory("02_ColdDensityEquilibration");

    //identify current bead
    int startbead = GetStartBead();
    
    // Ensure current.rst7 is the correct file.
    SetRestartFile(startbead);

    // copy to /tmp
    fs::copy(settings.PRMTOP,"/tmp/job.prmtop");
    fs::copy("current_step.rst7","/tmp/last_step.rst7");

    std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
    csv_file += "/06_Analysis/ColdEquil.csv";
    std::string filebasename = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/02_ColdDensityEquilibration/cold_equil.";

    // change directory to /tmp
    fs::current_path("/tmp/");

    // Perform cold equilibration loop
    cold_equil_loop(settings, slurm, startbead, filebasename, csv_file);
    
    // return to original directory when finished in /tmp
    fs::current_path(std::getenv("SLURM_SUBMIT_DIR"));
    fs::remove("mdinfo");

    // Error Checking after finishing loop
    startbead = GetStartBead();
    
    if (startbead != settings.NUM_COLD_STEPS)
    {
        error_log("ERROR:  Number of cold density equilibration steps does not match expectations.",1);
    }

    // Plot the coldequil.csv to figures and append the latex report.
    GenerateFiguresAndReport();

    // Compress 02_ColdDensityEquilibration/ to 02_ColdDensityEquilibration.tar.gz, then remove the folder
    utils::compress_and_delete("02_ColdDensityEquilibration");
    
    // Compile Current Report
    latex::compile_report(settings);
    
    // Create .AMBER_COLD_RELAX_COMPLETE
    utils::write_to_file(".AMBER_COLD_RELAX_COMPLETE","");
    return 0;
}