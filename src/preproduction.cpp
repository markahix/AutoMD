#include "ambermachine.h"

void write_mdin_minimize()
{
    std::string mdin_text = R"(Energy Minization To Eliminate Clashes
 &cntrl
  imin   = 1,
  maxcyc = 500,
  ncyc   = 50,
  ntc    = 2, 
  ntf    = 1,
  iwrap  = 1,
  ntb    = 1,
  cut    = 10
   /
END
)";
    utils::write_to_file("01_Minimization/init.0000.in", mdin_text);
    return;
}

void write_init_velocity_relax()
{
std::string mdin_text = R"(Velocity Initialization
 &cntrl
  ntx    = 1, 
  ntxo   = 1,       
  irest  = 0,
  ntpr   = 5,       
  ntwx   = 100,     
  ntwv   = 00,
  nstlim = 3000,   
  t      = 0.00,        
  dt     = 0.00100,
  ntc    = 2, 
  ntf    = 1,
  ntb    = 2, 
  ntp    = 2,
  iwrap  = 1,
  ig     = -1,
  ntt    = 1, 
  temp0  = 010.0, 
  tempi  = 010.0, 
  tautp  = 0.5,
  nmropt = 1,
  cut    = 10.0,
 /
 &wt type='REST', istep1=000000,istep2=5000, &end
 &wt type='END'  &end /
END
)";
    utils::write_to_file("/tmp/mdin.in", mdin_text);
}

void write_mdin_density_equil(double temperature)
{
std::string mdin_text = R"(Density Equilibration at )" + std::to_string(temperature) +  R"(K
 &cntrl
  ntx    = 5, 
  ntxo   = 1,       
  irest  = 1,
  ntpr   = 5,       
  ntwx   = 100,     
  ntwv   = 00,
  nstlim = 3000,   
  t      = 0.00,        
  dt     = 0.00100,
  ntc    = 2, 
  ntf    = 1,
  ntb    = 2, 
  ntp    = 2,
  iwrap  = 1,
  ig     = -1,
  ntt    = 1, 
  temp0  = )" + std::to_string(temperature) +  R"(, 
  tempi  = )" + std::to_string(temperature) +  R"(, 
  tautp  = 0.5,
  nmropt = 1,
  cut    = 10.0,
 /
 &wt type='REST', istep1=000000,istep2=5000, &end
 &wt type='END'  &end /
END
)";
    utils::write_to_file("/tmp/mdin.in", mdin_text);
}

void update_report()
{
    slurm::update_job_name("Updating_Report_Timeline");
    std::stringstream buffer;
    buffer.str("");
    buffer << "Cold Density Equilibration & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    utils::append_to_file("00_Report/timeline.tex",buffer.str());
    normal_log("Updated report with cold equilibration data.");
}

void GenerateFiguresAndReport()
{
    // Plot all CSVs using python
    slurm::update_job_name("Generating_Preproduction_Plots");
    python::plot_csv_data("06_Analysis/ColdEquil.csv");
    python::plot_csv_data("06_Analysis/Heating.csv");
    python::plot_csv_data("06_Analysis/HotEquil.csv");

    // Cold Equilibration Figures.
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
        
    // Heating Figures
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


    // Hot Equilibration Figures
    if (utils::CheckFileExists("00_Report/HotEquil_Figure_01.png"))
    {
    std::string report_update = R"LATEX(
\begin{figure}[!htbp]
\centering
\includegraphics[width=0.9\textwidth]{HotEquil_Figure_01.png}
\caption{Temperature (K), pressure (bar), volume ($\AA$), and density ($g\cdot mL^{-1}$) during hot density equilibration stage.}
\label{fig:hot_equil_fig_01}
\end{figure}

)LATEX";
    utils::append_to_file("00_Report/hot_equil.tex",report_update);
    }
    if (utils::CheckFileExists("00_Report/HotEquil_Figure_02.png"))
    {
    std::string report_update = R"LATEX(
\begin{figure}[!htbp]
\centering
\includegraphics[width=0.9\textwidth]{HotEquil_Figure_02.png}
\caption{Total, kinetic, and potential energies ($kcal\cdot mol^{-1}$) during hot density equilibration stage.}
\label{fig:hot_equil_fig_02}
\end{figure}

)LATEX";
    utils::append_to_file("00_Report/hot_equil.tex",report_update);
    }
    if (utils::CheckFileExists("00_Report/HotEquil_Figure_03.png"))
    {
    std::string report_update = R"LATEX(
\begin{figure}[!htbp]
\centering
\includegraphics[width=0.9\textwidth]{HotEquil_Figure_03.png}
\caption{Bond, angle, dihedral, van der Waals, and electrostatic energies ($kcal\cdot mol^{-1}$) during hot density equilibration stage.}
\label{fig:hot_equil_fig_03}
\end{figure}

)LATEX";
    utils::append_to_file("00_Report/hot_equil.tex",report_update);
    }
}

bool is_density_equilibrated(std::string mdout)
{
    std::ifstream ifile(mdout,std::ios::in);
    std::string line;
    while (getline(ifile,line))
    {
        if (line.find("R M S  F L U C T U A T I O N S") != std::string::npos)
        {
            break;
        }
    }
    while (getline(ifile,line))
    {
        if (line.find("Density") != std::string::npos)
        {
            double curr_density = stof(utils::string_between(line,"=","?"));
            if (curr_density < 0.01)
            {
                return false;
            }
            return true;
        }
    }
    error_log("Did not find final density values.  Terminating run", 1);
    return false;
}

void minimize_job(SlurmSettings slurm, JobSettings settings)
{
    if (!fs::create_directory("01_Minimization"))
    {
        error_log("Unable to create 01_Minimization/ directory! Terminating!",1);
    }
    // create mdin.in for minimization
    write_mdin_minimize();
    // set filenames
    std::string mdout_file = "01_Minimization/init.0000.out";
    std::string restart_file = "01_Minimization/init.0000.rst7";
    std::string trajectory_file = "01_Minimization/init.0000.mdcrd";
    normal_log("MDOUT:      " + mdout_file);
    normal_log("RESTART:    " + restart_file);
    normal_log("TRAJECTORY: " + trajectory_file);

    std::stringstream buffer;
    buffer.str("");
    buffer << "pmemd.cuda -O";
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
    // fs::remove("mdinfo");
    buffer.str("");
    if (! fs::exists(mdout_file))
    {
        error_log("Minimization failure.  Missing mdout file.",1);
    }
    // Compress 01_Minimization/ to 01_Minimization.tar.gz, then remove the folder
    slurm::update_job_name("Compressing_Minimization_Data");
    normal_log("Compressing AMBER minimization data...");
    utils::compress_and_delete("01_Minimization");
    normal_log("Compression complete.");
}

void cold_equilibration_job(SlurmSettings slurm, JobSettings settings)
{
    fs::create_directory("02_ColdEquilibration");
    fs::copy(settings.PRMTOP,"/tmp/job.prmtop");
    fs::copy("current_step.rst7","/tmp/last_step.rst7");
    fs::copy(settings.INPCRD,"/tmp/start_coords.rst7");

    std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
    csv_file += "/06_Analysis/ColdEquil.csv";
    std::string filebasename = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/02_ColdEquilibration/cold_equil.";
    
    // change directory to /tmp
    std::string orig_path = fs::current_path();
    fs::current_path("/tmp/");

    int n_steps=0;
    std::string mdin_file;
    std::string mdout_file;
    std::string restart_file;
    std::string trajectory_file;

    do
    {
        // create mdin.in for cold equilibration
        std::stringstream jobname;
        jobname.str("");
        jobname << "Cold_Equilibration_Step_" << n_steps+1;
        slurm::update_job_name(jobname.str());
        normal_log("Cold Equilibration: Step " + std::to_string(n_steps+1));
        if (n_steps == 0)
        {
            write_init_velocity_relax();
        }
        else
        {
            write_mdin_density_equil(10.0);
        }

        // set filenames
        std::stringstream lead_zero_number;
        lead_zero_number.str("");
        lead_zero_number << std::setw(4) << std::setfill('0') << n_steps+1;
        mdin_file = filebasename+ lead_zero_number.str() + ".in";
        mdout_file = filebasename + lead_zero_number.str() + ".out";
        restart_file = filebasename + lead_zero_number.str() + ".rst7";
        trajectory_file = filebasename + lead_zero_number.str() + ".mdcrd";
        
        std::string mdin_path_name = fs::path(mdin_file).filename();
        std::string mdout_path_name = fs::path(mdout_file).filename();
        std::string restart_path_name = fs::path(restart_file).filename();
        std::string traj_path_name = fs::path(trajectory_file).filename();
                
        normal_log("MDIN:       " + mdin_path_name);
        normal_log("MDOUT:      " + mdout_path_name);
        normal_log("RESTART:    " + restart_path_name);
        normal_log("TRAJECTORY: " + traj_path_name);

        // load amber module, then run Amber (pmemd.cuda)
        ambermachine::AmberLoopCUDA(slurm);
        ambermachine::AmberCopyBack(mdin_file,restart_file,mdout_file,trajectory_file,csv_file);
        n_steps++;
    } while (is_density_equilibrated(mdout_file));
    
    fs::current_path(orig_path);
}

void heating_job(SlurmSettings slurm, JobSettings settings)
{
std::string mdintext = R"(Iterative Heating to )" + std::to_string(settings.TEMPERATURE) +  R"(K
 &cntrl
  ntx    = 7,        
  irest  = 1,
  nsnb   = 1,
  ntpr   = 100,       
  ntwx   = 1000,     
  ntwv   = 00,
  nstlim = 100000,   
  t      = 0.00,        
  dt     = 0.00100,
  ntc    = 2, 
  ntf    = 1,
  ntb    = 1,
  iwrap  = 1,
  ntt    = 1, 
  temp0  = 010.0, 
  tempi  = 010.0, 
  tautp  = 0.001, 
  vlimit = 20.0,
  cut    = 8.0,
  nmropt=1
 /
)";
    std::stringstream jobname;
    jobname.str("");
    jobname << "Iterative_Heating";
    slurm::update_job_name(jobname.str());
    double max_temp = settings.TEMPERATURE + 30;
    int temp_interval = max_temp / 9;
    int step_1 = 0;
    int step_2 = 10000;
    int temp_1 = 10;
    int temp_2 = temp_1 + temp_interval;
    std::stringstream buffer;
    buffer.str("");
    for (int i = 0; i < 9; i ++)
    {
        buffer << " &wt type='TEMP0', istep1=" << step_1 << ", istep2=" << step_2 << ", value1=" << temp_1 << ", value2=" << temp_2 << ",  &end" << std::endl;
        step_1 = step_2 + 1;
        step_2 = step_2 + 10000;
        temp_1 = temp_2;
        temp_2 = temp_2 + temp_interval;
    }
    buffer <<  " &wt type='TEMP0', istep1=" << step_1 << ", istep2=" << step_2 << ", value1=" << temp_1 << ", value2=" << settings.TEMPERATURE << ",  &end" << std::endl;
    mdintext += buffer.str();
    mdintext += R"( &wt type='END'  &end /
END
)";
    utils::write_to_file("/tmp/mdin.in", mdintext);
    fs::create_directory("03_Heating");

    // change directory to /tmp
    std::string orig_path = fs::current_path();
    fs::current_path("/tmp/");

    int n_steps=1;
    std::stringstream lead_zero_number;
    lead_zero_number.str("");
    lead_zero_number << std::setw(4) << std::setfill('0') << n_steps;
    std::string filebase = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/03_Heating/heating." + lead_zero_number.str();

    std::string mdin_file = filebase + ".in";
    std::string mdout_file = filebase + ".out";
    std::string restart_file = filebase + ".rst7";
    std::string trajectory_file = filebase + ".mdcrd";
    std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
    csv_file += "/06_Analysis/Heating.csv";

    ambermachine::AmberLoopCUDA(slurm);
    ambermachine::AmberCopyBack(mdin_file,restart_file,mdout_file,trajectory_file,csv_file);
    fs::current_path(orig_path);

}

void hot_equilibration_job(SlurmSettings slurm, JobSettings settings)
{
    fs::create_directory("04_HotEquilibration");    

    std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
    csv_file += "/06_Analysis/HotEquil.csv";
    std::string filebasename = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/04_HotEquilibration/hot_equil.";
    
    // change directory to /tmp
    std::string orig_path = fs::current_path();
    fs::current_path("/tmp/");

    int n_steps=0;
    std::string mdin_file;
    std::string mdout_file;
    std::string restart_file;
    std::string trajectory_file;

    do
    {
        // create mdin.in for cold equilibration
        std::stringstream jobname;
        jobname.str("");
        jobname << "Hot_Equilibration_Step_" << n_steps+1;
        slurm::update_job_name(jobname.str());
        normal_log("Hot_Equilibration: Step " + std::to_string(n_steps+1));
        write_mdin_density_equil(settings.TEMPERATURE);

        // set filenames
        std::stringstream lead_zero_number;
        lead_zero_number.str("");
        lead_zero_number << std::setw(4) << std::setfill('0') << n_steps+1;
        mdin_file = filebasename+ lead_zero_number.str() + ".in";
        mdout_file = filebasename + lead_zero_number.str() + ".out";
        restart_file = filebasename + lead_zero_number.str() + ".rst7";
        trajectory_file = filebasename + lead_zero_number.str() + ".mdcrd";
        
        std::string mdin_path_name = fs::path(mdin_file).filename();
        std::string mdout_path_name = fs::path(mdout_file).filename();
        std::string restart_path_name = fs::path(restart_file).filename();
        std::string traj_path_name = fs::path(trajectory_file).filename();
                
        normal_log("MDIN:       " + mdin_path_name);
        normal_log("MDOUT:      " + mdout_path_name);
        normal_log("RESTART:    " + restart_path_name);
        normal_log("TRAJECTORY: " + traj_path_name);

        // load amber module, then run Amber (pmemd.cuda)
        ambermachine::AmberLoopCUDA(slurm);
        ambermachine::AmberCopyBack(mdin_file,restart_file,mdout_file,trajectory_file,csv_file);
        n_steps++;
    } while (is_density_equilibrated(mdout_file));

    fs::current_path(orig_path);
}

int main(int argc, char** argv)
{
    // Make sure I can actually RUN the classical dynamics simulations.
    if (! utils::CheckProgAvailable("pmemd"))
    {
        error_log("Unable to locate pmemd.  Make sure you have provided the correct Amber module.",1);
    }
    normal_log("Located pmemd.cuda at: ");
    std::string pmemd_loc = utils::GetSysResponse("which pmemd");
    normal_log("\t" + pmemd_loc);

    // Variable Declarations.
    JobSettings settings;
    SlurmSettings slurm;
    slurm.SLURM_executable = argv[0];
    ambermachine::read_amberinput(settings,slurm);
    std::stringstream buffer;
    
    // Update report
    buffer.str("");
    buffer << "Preproduction & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    utils::append_to_file("00_Report/timeline.tex",buffer.str());
    normal_log("Updated report for preproduction.");
    
    // Run minimization
    minimize_job(slurm,settings);
    buffer.str("");
    buffer << "Minimization Complete & \\texttt{" << utils::GetTimeAndDate()<< "} & \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    utils::append_to_file("00_Report/timeline.tex",buffer.str());
    normal_log("Minimization complete.");

    // Run low temperature density equilibration
    cold_equilibration_job(slurm, settings);
    utils::compress_and_delete("02_ColdEquilibration");
    buffer.str("");
    buffer << "Cold Density Equilibration Complete & \\texttt{" << utils::GetTimeAndDate()<< "} & \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    utils::append_to_file("00_Report/timeline.tex",buffer.str());
    normal_log("Low temperature density equilibration complete.");
    
    // Run iterative heating
    heating_job(slurm,settings);
    utils::compress_and_delete("03_Heating");
    buffer.str("");
    buffer << "Iterative Heating Complete & \\texttt{" << utils::GetTimeAndDate()<< "} & \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    utils::append_to_file("00_Report/timeline.tex",buffer.str());
    normal_log("Iterative heating complete.");

    // Run high temperature density equilibration
    hot_equilibration_job(slurm, settings);
    utils::compress_and_delete("04_HotEquilibration");
    buffer.str("");
    buffer << "Hot Density Equilibration Complete & \\texttt{" << utils::GetTimeAndDate()<< "} & \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    utils::append_to_file("00_Report/timeline.tex",buffer.str());
    normal_log("Target temperature density equilibration complete.");
    
    // Plot the coldequil.csv to figures and append the latex report.
    GenerateFiguresAndReport();

    // Compile Current Report
    latex::compile_report(settings);
    
    // Create .AMBER_COLD_RELAX_COMPLETE
    utils::write_to_file(".AMBER_PREPRODUCTION_COMPLETE","");
    return 0;
}
