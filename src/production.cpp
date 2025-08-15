#include "ambermachine.h"

void UpdateReport()
{
        slurm::update_job_name("Updating_Report_Timeline");
        std::stringstream buffer;
        buffer.str("");
        buffer << "Production Dynamics & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
        buffer << "\\hline" << std::endl;
        utils::append_to_file("00_Report/timeline.tex",buffer.str());
}

void write_mdin_production()
{
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
    utils::write_to_file("mdin.in",heat_script);
    return;
}

int GetStartBead()
{
    int startbead = 0;
    for (fs::path p : fs::directory_iterator("05_Production/"))
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
        std::string restart_file = "05_Production/prod." + lead_zero_number.str() + ".rst7";
        fs::copy(restart_file,"current_step.rst7");
    }
}

void GenerateFileNames(FileList &files, std::string filebasename, int step_num)
{
        std::stringstream lead_zero_number;
        lead_zero_number.str("");
        lead_zero_number << std::setw(4) << std::setfill('0') << step_num;
        files.AddFile("mdin",filebasename + lead_zero_number.str() + ".in");
        files.AddFile("mdout",filebasename + lead_zero_number.str() + ".out");
        files.AddFile("restart",filebasename + lead_zero_number.str() + ".rst7");
        files.AddFile("trajectory",filebasename + lead_zero_number.str() + ".mdcrd");
        std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
        csv_file += "/06_Analysis/Production.csv";
        files.AddFile("csv",csv_file);
}

void ProductionLoop(JobSettings settings, SlurmSettings slurm, int startbead, std::string filebasename, FileList &files)
{
    for (int i=startbead; i < settings.NUM_PROD_STEPS; i++)
    {
        // change directory to /tmp
        fs::current_path("/tmp/");

        // create mdin.in for production
        std::stringstream jobname;
        jobname.str("");
        jobname << "Running_Production_Step_" << i+1 << "_of_" << settings.NUM_PROD_STEPS;
        slurm::update_job_name(jobname.str());
        write_mdin_production();

        // set filenames
        GenerateFileNames(files,filebasename, i+1);

        // load amber module, then run Amber (pmemd.cuda)
        std::cout << "DEBUG: In Production Function, slurm_amber_module is:  " << slurm.SLURM_amber_module << std::endl;
        ambermachine::AmberLoop(slurm);

        // Error Check the output, terminate job if a step fails.
        std::ifstream outfile("mdout.out");
        std::string line;
        while (std::getline(outfile,line))
        {
            if (line.find("ERROR: Calculation halted.") != std::string::npos)
            {   
                std::stringstream buffer;
                std::cout << "Job Failed.  Terminating." << std::endl;
                buffer.str("");
                buffer << "scancel " << slurm.SLURM_JOB_ID;
                utils::silent_shell(buffer.str().c_str());
                return;
            }
        }
        ambermachine::AmberCopyBack(files.GetFile("mdin"),files.GetFile("restart"),files.GetFile("mdout"),files.GetFile("trajectory"),files.GetFile("csv"));
        fs::current_path(std::getenv("SLURM_SUBMIT_DIR"));
        // Check if MMPBSA job, spawn if necessary.
        if (settings.RUN_MMPBSA)
        {
            slurm::submit_mmpbsa_job(settings, slurm, files.GetFile("trajectory"));
        }          

        // Check if SASA job, spawn if necessary.
        if (settings.RECEPTOR_MASK != " " && settings.LIGAND_MASK != " " && settings.COMPLEX_MASK != " ")
        {
            slurm::submit_sasa_job(settings, slurm, files.GetFile("trajectory"));
        }
    }
}

void GeneratePlotsAndReport(FileList files)
{
    slurm::update_job_name("Generating_Plots_Production");
    python::plot_csv_data(files.GetFile("csv"));

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

}

std::string GetCompressedProductionFilename(JobSettings settings)
{
    int startstep = 1;
    int n_steps = settings.NUM_PROD_STEPS;
    std::stringstream newprodfile;
    newprodfile.str("");
    newprodfile << "05_Production/Production_" << std::setw(3) << std::setfill('0') << startstep;
    newprodfile << "_" << std::setw(3) << std::setfill('0') << n_steps << ".mdcrd";
    while (fs::exists(newprodfile.str()))
    {
        std::string old_file = newprodfile.str();
        startstep = stoi(old_file.substr(old_file.find_last_of("_")+1,old_file.size()-old_file.find_last_of("_")-7));
        n_steps = startstep + settings.NUM_COLD_STEPS;
        startstep++;
        newprodfile.str("");
        newprodfile << "05_Production/Production_" << std::setw(3) << std::setfill('0') << startstep;
        newprodfile << "_" << std::setw(3) << std::setfill('0') << n_steps << ".mdcrd";
    }
    return newprodfile.str();
}

void CompressProductionFolder(JobSettings settings)
{
    std::stringstream buffer;
    // Move restarts into RESTARTS folder
    buffer.str("");
    buffer << "mkdir 05_Production/RESTARTS/; mv 05_Production/*.rst7 05_Production/RESTARTS/";
    utils::silent_shell(buffer.str().c_str());

    // Move outs into OUTPUTS folder
    buffer.str("");
    buffer << "mkdir 05_Production/OUTPUTS/; mv 05_Production/*.out 05_Production/OUTPUTS/";
    utils::silent_shell(buffer.str().c_str());

    if (settings.COMPRESS_STAGES)
    {
        buffer.str("");
        buffer << "cd 05_Production; tar -czvf RESTARTS.tar.gz RESTARTS/ && rm -R RESTARTS/; tar -czvf OUTPUTS.tar.gz OUTPUTS/ && rm -R OUTPUTS/; cd ../";
        utils::silent_shell(buffer.str().c_str());
        // Compress trajectories to one file.
        std::stringstream cpptraj;
        cpptraj.str("");
        cpptraj << "parm " << settings.PRMTOP << std::endl;
        std::vector<std::string> traj_file_list = utils::sort_files_by_timestamp("05_Production/",".mdcrd");
        for (unsigned int i=0; i < traj_file_list.size(); i++)
        {
            cpptraj << "trajin " << traj_file_list[i] << std::endl;
        }
        cpptraj << "reference " << settings.INPCRD << std::endl;
        cpptraj << "autoimage" << std::endl;
        std::string newprodfile = GetCompressedProductionFilename(settings);
        cpptraj << "trajout " << newprodfile << std::endl;
        cpptraj << "run" << std::endl;
        cpptraj << "quit" << std::endl;
        utils::write_to_file("compress_prod.in",cpptraj.str());
        utils::silent_shell("cpptraj < compress_prod.in >> compress_prod.out");
        if (fs::exists(newprodfile))
        {
            utils::silent_shell("rm 05_Production/prod.*.mdcrd compress_prod.in compress_prod.out");
        }
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
    std::string filebasename = (std::string)std::getenv("SLURM_SUBMIT_DIR") + "/05_Production/prod.";
    
    UpdateReport();

    // create job subdirectory.
    fs::create_directory("05_Production/");

    //identify current bead
    int startbead = GetStartBead();

    //ensure that matching rst7 is in the "current_step.rst7" position in the main directory.
    SetRestartFile(startbead);
    
    // copy to /tmp
    fs::copy(settings.PRMTOP,"/tmp/job.prmtop");
    fs::copy("current_step.rst7","/tmp/last_step.rst7");

    // Loop over all production steps
    
    ProductionLoop(settings, slurm, startbead, filebasename, files);
    
    fs::remove("mdinfo");
    
    // Error Checking after finishing loop
    startbead = GetStartBead();
    if (startbead != settings.NUM_PROD_STEPS)
    {
        error_log("ERROR:  Number of production dynamics steps does not match expectations.",1);
    }

    // Plot the Production.csv using python ... 
    GeneratePlotsAndReport(files);

    CompressProductionFolder(settings);

    // Complete Production job stage
    slurm::update_job_name("Completing_Production");

    // Compile Current Report
    latex::compile_report(settings);
    
    // Create .AMBER_PRODUCTION_COMPLETE
    utils::write_to_file(".AMBER_PRODUCTION_COMPLETE","");

    return 0;
}