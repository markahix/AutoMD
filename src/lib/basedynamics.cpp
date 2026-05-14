#include "basedynamics.h"

void UpdateReport()
{
        slurm::update_job_name("Updating_Report_Timeline");
        std::stringstream buffer;
        buffer.str("");
        buffer << "Production Dynamics & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
        buffer << "\\hline" << std::endl;
        utils::append_to_file("00_Report/timeline.tex",buffer.str());
}

int GetStartBead(std::string directory)
{
    int startbead = 0;
    for (fs::path p : fs::directory_iterator(directory))
    {
        if (p.extension() == ".mdcrd")
        {
            startbead++;
        }
    }
    return startbead;
}

void GenerateFileNames(FileList &files, std::string filebasename, int step_num)
{
        std::stringstream lead_zero_number;
        lead_zero_number.str("");
        lead_zero_number << std::setw(4) << std::setfill('0') << step_num;
        files.AddFile("mdin",filebasename + ".in");
        files.AddFile("mdout",filebasename + lead_zero_number.str() + ".out");
        files.AddFile("restart",filebasename + lead_zero_number.str() + ".rst7");
        files.AddFile("trajectory",filebasename + lead_zero_number.str() + ".mdcrd");
        files.AddFile("cpin",filebasename + lead_zero_number.str() + ".cpin");
        files.AddFile("cpout",filebasename + lead_zero_number.str() + ".cpout");
        files.AddFile("cprst",filebasename + lead_zero_number.str() + ".cprst");
        std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
        csv_file += "/06_Analysis/Production.csv";
        files.AddFile("csv",csv_file);
}

void SetRestartFile(int startbead, std::string production_directory)
{
    //ensure that matching rst7 is in the "current_step.rst7" position in the main directory.
    if (startbead > 0)
    {
        std::stringstream lead_zero_number;
        lead_zero_number.str("");
        lead_zero_number << std::setw(4) << std::setfill('0') << startbead;
        std::string restart_file = production_directory + "prod." + lead_zero_number.str() + ".rst7";
        fs::copy(restart_file,"current_step.rst7");
    }
    else
    {
        std::stringstream lead_zero_number;
        lead_zero_number.str("");
        lead_zero_number << std::setw(4) << std::setfill('0') << startbead;
        std::string restart_file = production_directory + "prod." + lead_zero_number.str() + ".rst7";
        fs::copy("current_step.rst7",restart_file);
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

void CallByNanosecondAnalytics(JobSettings settings, SlurmSettings slurm, FileList &files)
{
        // #####################################################
        // # REPORT BACK TO AGIMUS FOR BY-NANOSECOND FUNCTIONS #
        // #   AutoMMPBSA, AutoAnalytics(Time-Dependent), etc  #
        // #####################################################


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

void CompressProductionFolder(JobSettings settings, std::string production_folder)
{
    std::string current_dir=fs::current_path();
    fs::current_path(production_folder);
    std::stringstream buffer;
    // Move restarts into RESTARTS folder
    buffer.str("");
    buffer << "mkdir RESTARTS/; mv *.rst7 RESTARTS/";
    utils::silent_shell(buffer.str().c_str());

    // Move outs into OUTPUTS folder
    buffer.str("");
    buffer << "mkdir OUTPUTS/; mv *.out OUTPUTS/";
    utils::silent_shell(buffer.str().c_str());

    fs::current_path(current_dir);

    if (settings.COMPRESS_STAGES)
    {
        fs::current_path(production_folder);
        buffer.str("");
        buffer << "tar -czvf RESTARTS.tar.gz RESTARTS/ && rm -r RESTARTS/; tar -czvf OUTPUTS.tar.gz OUTPUTS/ && rm -r OUTPUTS/; cd -";
        utils::silent_shell(buffer.str().c_str());
        fs::current_path(current_dir);
        // Compress trajectories to one file.
        std::stringstream cpptraj;
        cpptraj.str("");
        cpptraj << "parm " << settings.PRMTOP << std::endl;
        std::vector<std::string> traj_file_list = utils::sort_files_by_timestamp(production_folder,".mdcrd");
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
            buffer.str("");
            buffer << "rm " << production_folder << "/prod.*.mdcrd compress_prod.in compress_prod.out";
            utils::silent_shell(buffer.str().c_str());
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
