// #include "ambermachine.h"

// void write_mdin_hot_relax(JobSettings settings, double restraint_value)
// {
//     int colpos = settings.COMPLEX_MASK.find(":");
//     int dashpos = settings.COMPLEX_MASK.find("-");
//     std::string startres,endres;
//     if (dashpos != std::string::npos)
//     {
//         startres = settings.COMPLEX_MASK.substr(colpos + 1, dashpos - colpos);
//         endres = settings.COMPLEX_MASK.substr(dashpos + 1, settings.COMPLEX_MASK.length());
//     }
//     else
//     {
//         startres = settings.COMPLEX_MASK.substr(colpos + 1, settings.COMPLEX_MASK.length());
//         endres = settings.COMPLEX_MASK.substr(colpos + 1, settings.COMPLEX_MASK.length());
//     } 
//     std::stringstream buffer;
//     std::string heat_script = R"HEATSCRIPT(
// Hot Density Equilibration
//   &cntrl
//   ntx      = 7, 
//   irest    = 1,
//   nsnb     = 1,
//   ntpr     = 1000, 
//   ntwx     = 1000, 
//   ntwv     = 0,
//   nstlim   = 1000, 
//   t        = 0.00, 
//   dt       = 0.00100,
//   ntc      = 2, 
//   ntf      = 1,
//   iwrap    = 1,
//   ntb      = 2, 
//   ntp      = 2,
//   ntt      = 3, 
//   temp0    = 300.0, 
//   tempi    = 300.0, 
//   tautp    = 1.0, 
//   gamma_ln = 5.0,
//   ntr      = 1,
//   cut      = 10.0,
//   vlimit   = -1,
//  /
// Hold molecule fixed
// )HEATSCRIPT";
//     buffer.str("");
//     buffer << heat_script << std::endl;
//     buffer << restraint_value << std::endl;
//     buffer << "RES " << startres << " " << endres << std::endl;
//     buffer << "END" << std::endl;
//     buffer << "END" << std::endl;
//     utils::write_to_file("mdin.in",buffer.str());
//     return;
// }

// namespace ambermachine
// {
//     void hot_equilibration(JobSettings settings,SlurmSettings slurm)
//     {
//         double restraint_value;
//         double rest_interval = settings.MAX_RESTRAINT / (settings.NUM_HOT_STEPS - 1);

//         // Update report
//         slurm::update_job_name("Updating_Report_Timeline");
//         std::stringstream buffer;
//         buffer.str("");
//         buffer << "Hot Density Equilibration & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
//         buffer << "\\hline" << std::endl;
//         utils::append_to_file("00_Report/timeline.tex",buffer.str());

//         // create job subdirectory.
//         fs::create_directory("04_HotDensityEquilibration/");

//         //identify current bead
//         int startbead = 0;
//         for (fs::path p : fs::directory_iterator("04_HotDensityEquilibration/"))
//         {
//             if (p.extension() == ".mdcrd")
//             {
//                 startbead++;
//             }
//         }

//         //ensure that matching rst7 is in the "current_step.rst7" position in the main directory.
//         if (startbead > 0)
//         {
//             std::stringstream lead_zero_number;
//             lead_zero_number.str("");
//             lead_zero_number << std::setw(4) << std::setfill('0') << startbead;
//             std::string restart_file = "04_HotDensityEquilibration/hot_equil." + lead_zero_number.str() + ".rst7";
//             fs::copy(restart_file,"current_step.rst7");
//         }
        
//         // copy to /tmp
//         fs::copy(settings.PRMTOP,"/tmp/job.prmtop");
//         fs::copy("current_step.rst7","/tmp/last_step.rst7");


//         std::string filebasename = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/04_HotDensityEquilibration/hot_equil.";
//         std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
//         csv_file += "/06_Analysis/HotEquil.csv";
//         // Loop over all hot steps
//         for (int i=startbead; i < settings.NUM_HOT_STEPS; i++)
//         {

//             // change directory to /tmp
//             fs::current_path("/tmp/");

//             // set current restraint value
//             restraint_value = settings.MAX_RESTRAINT - (i * rest_interval);
//             if (restraint_value < 0.0)
//             {
//                 restraint_value = 0.0;
//             }

//             // create mdin.in for hot equilibration
//             std::stringstream jobname;
//             jobname.str("");
//             jobname << "Running_Hot_Equilibration_Step_" << i+1 << "_of_" << settings.NUM_HOT_STEPS;
//             slurm::update_job_name(jobname.str());
//             write_mdin_hot_relax(settings,restraint_value);

//             // set filenames
//             std::stringstream lead_zero_number;
//             lead_zero_number.str("");
//             lead_zero_number << std::setw(4) << std::setfill('0') << i+1;
//             std::string mdin_file = filebasename+ lead_zero_number.str() + ".in";
//             std::string mdout_file = filebasename + lead_zero_number.str() + ".out";
//             std::string restart_file = filebasename + lead_zero_number.str() + ".rst7";
//             std::string trajectory_file = filebasename + lead_zero_number.str() + ".mdcrd";

//             // load amber module, then run Amber (pmemd.cuda)
//             std::cout << "DEBUG: In Hot Equilibration Function, slurm_amber_module is:  " << slurm.SLURM_amber_module << std::endl;

//             AmberLoop(slurm);
//             AmberCopyBack(mdin_file, restart_file, mdout_file, trajectory_file, csv_file);

//         }
//         if (utils::CheckFileExists("mdinfo"))
//         {
//             fs::remove("mdinfo");
//         }
        
//         // return to original directory when finished in /tmp
//         fs::current_path(std::getenv("SLURM_SUBMIT_DIR"));

//         // Error Checking after finishing loop
//         startbead = 0;
//         for (fs::path p : fs::directory_iterator("04_HotDensityEquilibration/"))
//         {
//             if (p.extension() == ".mdcrd")
//             {
//                 startbead++;
//             }
//         }
//         if (startbead != settings.NUM_HOT_STEPS)
//         {
//             std::cout << "ERROR:  Number of hot density equilibration steps does not match expectations." << std::endl;
//             return;
//         }

//         // Plot the ColdEquil.csv using python ... 
//         slurm::update_job_name("Generating_Plots_Hot_Equilibration");
//         python::plot_csv_data("06_Analysis/HotEquil.csv");

//         // Update report with completed hot equilibration figures, checking if each one exists as we go.
//         if (utils::CheckFileExists("00_Report/HotEquil_Figure_01.png"))
//         {
//         std::string report_update = R"LATEX(
//     \begin{figure}[!htbp]
//     \centering
//     \includegraphics[width=0.9\textwidth]{HotEquil_Figure_01.png}
//     \caption{Temperature (K), pressure (bar), volume ($\AA$), and density ($g\cdot mL^{-1}$) during hot density equilibration stage.}
//     \label{fig:hot_equil_fig_01}
//     \end{figure}

//     )LATEX";
//         utils::append_to_file("00_Report/hot_equil.tex",report_update);
//         }
//         if (utils::CheckFileExists("00_Report/HotEquil_Figure_02.png"))
//         {
//         std::string report_update = R"LATEX(
//     \begin{figure}[!htbp]
//     \centering
//     \includegraphics[width=0.9\textwidth]{HotEquil_Figure_02.png}
//     \caption{Total, kinetic, and potential energies ($kcal\cdot mol^{-1}$) during hot density equilibration stage.}
//     \label{fig:hot_equil_fig_02}
//     \end{figure}

//     )LATEX";
//         utils::append_to_file("00_Report/hot_equil.tex",report_update);
//         }
//         if (utils::CheckFileExists("00_Report/HotEquil_Figure_03.png"))
//         {
//         std::string report_update = R"LATEX(
//     \begin{figure}[!htbp]
//     \centering
//     \includegraphics[width=0.9\textwidth]{HotEquil_Figure_03.png}
//     \caption{Bond, angle, dihedral, van der Waals, and electrostatic energies ($kcal\cdot mol^{-1}$) during hot density equilibration stage.}
//     \label{fig:hot_equil_fig_03}
//     \end{figure}

//     )LATEX";
//         utils::append_to_file("00_Report/hot_equil.tex",report_update);
//         }

//         // Compress 04_HotDensityEquilibration/ to 04_HotDensityEquilibration.tar.gz, then remove the folder
//         utils::compress_and_delete("04_HotDensityEquilibration");
//         // Complete hot equilibration job stage
//         slurm::update_job_name("Completing_Hot_Equilibration");
//         // Compile Current Report
//         latex::compile_report(settings);
//         // Create .AMBER_HOT_RELAX_COMPLETE
//         utils::write_to_file(".AMBER_HOT_RELAX_COMPLETE","");
//         // Submit the next stage of the job: submit_production_job()
//         slurm::submit_production_job(settings,slurm);
//         // Cleanup
//         slurm::cleanup_out_err(slurm);
//         return;
//     }
// }


