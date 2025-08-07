#include "ambermachine.h"

void write_mdin_heating(JobSettings settings)
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
    std::string heat_script = R"HEATSCRIPT(Heating
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
)HEATSCRIPT" + std::to_string(settings.MAX_RESTRAINT) + R"(
RES )" + startres + " " + endres + R"(
END
END
)";
    utils::write_to_file("mdin.in",heat_script);
    return;
}

void UpdateReport()
{
slurm::update_job_name("Updating_Report_Timeline");
    std::stringstream buffer;
    buffer.str("");
    buffer << "Iterative Heating & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    utils::append_to_file("00_Report/timeline.tex",buffer.str());

}

void GenerateFileNames(FileList &files, int step_num)
{
    std::stringstream lead_zero_number;
    lead_zero_number.str("");
    lead_zero_number << std::setw(4) << std::setfill('0') << step_num;
    std::string filebase = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/03_Heating/heating." + lead_zero_number.str();
    files.AddFile("mdin",filebase + ".in");
    files.AddFile("mdout",filebase + ".out");
    files.AddFile("restart",filebase + ".rst7");
    files.AddFile("trajectory",filebase + ".mdcrd");
    
    std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
    csv_file += "/06_Analysis/Heating.csv";
    files.AddFile("csv",csv_file);
}

void GeneratePlotsAndReport(FileList files)
{
// Plot the Heating.csv using python ... 
    slurm::update_job_name("Generating_Plots_Heating");
    python::plot_csv_data(files.GetFile("csv"));

    // Update report with completed heating figures, checking if each one exists as we go.
    if (fs::exists("00_Report/Heating_Figure_01.png"))
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
    if (fs::exists("00_Report/Heating_Figure_02.png"))
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
    if (fs::exists("00_Report/Heating_Figure_03.png"))
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
    FileList files;
    ambermachine::read_amberinput(settings,slurm);

    // Update report
    UpdateReport();

    // create job subdirectory.
    fs::create_directory("03_Heating");

    // copy to /tmp
    fs::copy(settings.PRMTOP,"/tmp/job.prmtop");
    fs::copy("current_step.rst7","/tmp/last_step.rst7");

    // change directory to /tmp
    fs::current_path("/tmp/");
    write_mdin_heating(settings);

    // define necessary filenames
    GenerateFileNames(files, settings.NUM_COLD_STEPS + 1);
    std::stringstream lead_zero_number;

    // Run Amber Job
    slurm::update_job_name("Performing_Iterative_Heating");

    std::stringstream buffer;
    buffer.str("");
    buffer << "module load " << slurm.SLURM_amber_module << "; ";
    buffer << "$AMBERHOME/bin/pmemd.cuda -O -i mdin.in -o mdout.out -p job.prmtop -c last_step.rst7 -r current_step.rst7 -x trajectory.mdcrd -ref last_step.rst7";
    utils::silent_shell(buffer.str().c_str());

    // copy back from /tmp
    fs::copy("mdin.in",files.GetFile("mdin"));
    fs::remove("mdin.in");

    fs::copy("current_step.rst7",files.GetFile("restart"));
    fs::copy("current_step.rst7",(std::string)std::getenv("SLURM_SUBMIT_DIR")+"/current_step.rst7",fs::copy_options::update_existing);
    
    fs::copy("current_step.rst7","last_step.rst7",fs::copy_options::update_existing);
    fs::remove("current_step.rst7");

    fs::copy("mdout.out",files.GetFile("mdout"));
    fs::remove("mdout.out");
    
    fs::copy("trajectory.mdcrd",files.GetFile("trajectory"));
    fs::remove("trajectory.mdcrd");

    if (fs::exists("mdinfo"))
    {
        fs::remove("mdinfo");
    }

    // return to original directory when finished in /tmp
    fs::current_path(std::getenv("SLURM_SUBMIT_DIR"));

    // Parse output to CSV file.
    utils::mdout_to_csv(files.GetFile("mdout"),files.GetFile("csv"));

    // Generate Plots and Update Report
    GeneratePlotsAndReport(files);    

    // Compress 03_Heating/ to 03_Heating.tar.gz, then remove the folder
    utils::compress_and_delete("03_Heating");
    
    // Complete minimization job stage
    slurm::update_job_name("Completing_Iterative_Heating");
    
    // Compile Current Report
    latex::compile_report(settings);
    
    // Create .AMBER_HEATING_COMPLETE
    utils::write_to_file(".AMBER_HEATING_COMPLETE","");
    
    return 0;
}