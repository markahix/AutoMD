#include "ambermachine.h"

// prepare cpptraj input, python post-processing, and latex for each section rather than trying to have all these separate things for each stage.  it's getting messy.

void prepare_clustering(JobSettings settings)
{
    std::stringstream cpptraj;
    std::stringstream python;
    std::stringstream report;
    cpptraj.str("");
    python.str("");
    report.str("");
    // append command to cpptraj.in
    // append plot to analyse.py
    // append results to report latex.

}

void parse_hbonds_avg(JobSettings settings)
{
    std::stringstream buffer;
    std::ifstream hbonds("06_Analysis/HBond_Table.dat");
    std::string line;
    std::stringstream dummy;
    getline(hbonds,line);

    // begin hbond table in LaTeX. "00_Report/analysis_HBONDS.tex"
    buffer.str("");
    buffer << "\\section{Hydrogen Bonding Analysis}" << std::endl;
    buffer << "\\paragraph{} Hydrogen bonding analysis was performed on atoms in " << settings.HBONDS_MASK << ". " << std::endl;
    if (settings.LIGAND_MASK != " " && settings.RECEPTOR_MASK != " ")
    {
        buffer << "This included a receptor and ligand, and the results of their intermolecular interactions are also included.  A secondary set of datafiles was generated which contains only the intermolecular interactions and does not contain any intramolecular interactions.  It may be found in the Analysis subdirectory." << std::endl;
    }
    buffer << "\\begin{longtable}{|rcl|r|}" << std::endl;
    buffer << "\\hline" << std::endl;
    buffer << " Acceptor &  & Donor & $\\%_{sim}$ \\\\" << std::endl;
    buffer << "\\hline" << std::endl;

    // obtain pairs of residues to get counts over time.
    int table_counter = 1;
    while (getline(hbonds,line))
    {
        std::string acceptor,donorH,donor,frames,frac,dist,angle;

        dummy.str(line);
        dummy >> acceptor >> donorH >> donor >> frames >> frac >> dist >> angle;
        int acc_num,don_num;
        acc_num = stoi(acceptor.substr(acceptor.find("_"),acceptor.find("@") - acceptor.find("_")));
        don_num = stoi(donor.substr(donor.find("_"),donor.find("@") - donor.find("_")));

        // Add HBond datatable to report as formatted LaTeX table.
        std::string acc_res_name = acceptor.substr(0,acceptor.find("_"));
        std::string don_res_name = donor.substr(0,donor.find("_"));
        std::string acc_atom_name = acceptor.substr(acceptor.find("@"),acceptor.size());
        std::string donh_atom_name = donorH.substr(donorH.find("@"),donorH.size());
        std::string don_atom_name = donor.substr(donor.find("@"),donor.size());
        buffer << acc_res_name << acc_num << "." << acc_atom_name << " & --> & " << donh_atom_name << "-" << don_atom_name << "." << don_num << don_res_name << " & " << stod(frac)*100 << "\\% \\\\" << std::endl;
        if (table_counter % 5 == 0)
        {
            buffer << "\\hline" << std::endl;
        }
        table_counter++;
    }
    buffer << "\\hline" << std::endl;
    buffer << "\\end{longtable}" << std::endl;
    utils::append_to_file("00_Report/analysis_HBONDS.tex",buffer.str());    
}

void parse_cpptraj_results(JobSettings settings, SlurmSettings slurm)
{
    slurm::update_job_name("Parsing_Primary_CPPTRAJ_Results");
    
    // start next cpptraj input file
    std::stringstream buffer;
    buffer.str("");
    buffer << "parm " << settings.PRMTOP << std::endl;
    
    for (std::experimental::filesystem::path p : std::experimental::filesystem::directory_iterator("05_Production/"))
    {
        if (p.extension() == ".mdcrd")
        {
        buffer << "trajin " << p << std::endl;
        }
    }
    buffer << "reference " << settings.INPCRD << std::endl;
    buffer << "autoimage " << std::endl;
    buffer << "strip !" << settings.COMPLEX_MASK << std::endl;
    utils::write_to_file("cpptraj_002.in",buffer.str());

    // Parse HBond_Avg.dat, figure out combinations of residues that have any HBonds, rerun cpptraj to obtain the COUNT of each hbond-pair over time.
    if (!utils::CheckFileExists("HBond_Table.dat"))
    {
        parse_hbonds_avg(settings);
    }

    // Parse primary output files, obtain combinations of time-dependent files which can be used for clustering analyses.
    std::stringstream clustering;
    clustering.str("");
    clustering << "cluster data ";
    int i = 1;
    int time_line_files = utils::count_lines_in_file("RMSD.dat");
    for (std::experimental::filesystem::path p : std::experimental::filesystem::directory_iterator("./"))
    {
        if (p.extension() == ".dat")
        {
            if (time_line_files == utils::count_lines_in_file(p))
            {
                buffer << "readdata " << p << " name fds_" << i << std::endl; 
                clustering << "fds_" << i << ",";
                i++;
            }
        }
    }
    clustering.seekp(-1,clustering.cur);
    clustering << " heiragglo epsilon 1.0 clusters 10 linkage epsilonplot epsilonplot.dat";
    clustering << " bestrep centroid";
    clustering << " out cnumvtime.dat";
    clustering << " summary cluster_summary.dat";
    clustering << " info clustinfo.dat";
    clustering << " repout clust";
    clustering << " repfmt restart";
    clustering << " cpopvtime cpopvtime.dat";
    clustering << " savenreps 1";
    clustering << " clustersvtime clustvtime.dat";
    clustering << " cvtwindow 50";
    clustering << std::endl;
    if (i > 1)
    {
        buffer << clustering.str();
    }

    // Parse primary output files, obtain residue-specific datasets which can be used for scatters.
    buffer << "run" << std::endl << "quit" << std::endl;
    utils::write_to_file("cpptraj_002.in",buffer.str());
    std::cout << "DEBUG: In Analysis Function, slurm_amber_module is:  " << slurm.SLURM_amber_module << std::endl;

    buffer.str("");
    buffer << "module load " << slurm.SLURM_amber_module << "; cpptraj < cpptraj_002.in > cpptraj_002.out";
    utils::silent_shell(buffer.str().c_str());
}

void parse_nmd_to_mode_csv()
{
    std::ifstream ifile;
    std::ofstream ofile;
    ifile.open("06_Analysis/nma_first_100_modes.nmd");
    ofile.open("06_Analysis/normal_modes.csv",std::ios::app);
    std::string line;
    while (getline(ifile,line))
    {
        std::stringstream line_buf;
        line_buf.str(line);
        std::string dummy;
        line_buf >> dummy;
        if (dummy != "mode")
        {
            continue;
        }            
        int mode_num;
        double eigenval;
        line_buf >> mode_num >> eigenval;
        while (line_buf)
        {
            double x,y,z;
            line_buf >> x >> y >> z;
            double norm;
            norm = sqrt(x*x + y*y + z*z)/eigenval;
            ofile << norm << ", ";
        }
        ofile <<  std::endl;
    }
    
    return;
}

namespace ambermachine 
{
    void analysis(JobSettings settings,SlurmSettings slurm)
    {
        std::stringstream buffer;
        // Update report timeline
        slurm::update_job_name("Updating_Report_Timeline");
        buffer.str("");
        buffer << "Final Analysis & \\texttt{" << utils::GetTimeAndDate()<< "} & \\textbf{" << std::getenv("SLURM_JOB_ID") << "} \\\\" << std::endl;
        buffer << "\\hline" << std::endl;
        utils::append_to_file("00_Report/timeline.tex",buffer.str());

        // make 06_Analysis/ subdirectory
        std::experimental::filesystem::create_directory("06_Analysis/");

        // prepare cpptraj.in, analyse.py, and report latex
        utils::write_to_file("cpptraj.in",cpptraj::preamble(settings));
        utils::write_to_file("analyse.py",python::preamble());

        if (settings.RMSD_MASK != " ")  // Handle RMSD   
        {
            utils::append_to_file("cpptraj.in",cpptraj::rmsd(settings));
            utils::append_to_file("analyse.py",python::plot_rmsd());
            std::stringstream caption;
            caption.str("");
            caption << "Root-mean-squared deviation (RMSD) for all residues in " << settings.RMSD_MASK << ".";
            latex::figure_block_to_file("RMSD",caption.str(),"00_Report/analysis_RMSD.tex");
            latex::python_block_to_file(python::plot_rmsd(), "00_Report/analysis_RMSD.tex");
        }

        if (settings.RMSF_MASK != " ")  // Handle RMSF
        {
            utils::append_to_file("cpptraj.in",cpptraj::rmsf(settings));
            utils::append_to_file("analyse.py",python::plot_rmsf());
            std::stringstream caption;
            caption.str("");
            caption << "Root-mean-squared fluctuation (RMSF) for all residues in " << settings.RMSF_MASK << ".";
            latex::figure_block_to_file("RMSF",caption.str(),"00_Report/analysis_RMSF.tex");
            latex::python_block_to_file(python::plot_rmsf(), "00_Report/analysis_RMSF.tex");
        }

        if (settings.CORREL_MASK != " ") // Handle Correlated Motion
        {
            utils::append_to_file("cpptraj.in",cpptraj::correl(settings));
            utils::append_to_file("analyse.py",python::plot_correl());
            std::stringstream caption;
            caption.str("");
            caption << "Correlated motion for all residue pairs in " << settings.CORREL_MASK << ".  Regions in blue indicate strong positive correlation (moving together) and regions in red indicate strong anticorrelation (moving opposite).";
            latex::figure_block_to_file("Correlated_Motion",caption.str(),"00_Report/analysis_CORREL.tex");
            latex::python_block_to_file(python::plot_correl(), "00_Report/analysis_CORREL.tex");
        }

        if (settings.HBONDS_MASK != " ") // Handle Hydrogen Bonding
        {
            utils::append_to_file("cpptraj.in",cpptraj::hbonds(settings));
        }

        if (settings.RECEPTOR_MASK != " " && settings.LIGAND_MASK != " ") // Handle Ligand Interaction Energy & SASA
        {
            // Ligand Interaction Energy
            utils::append_to_file("cpptraj.in",cpptraj::ligand_interaction_energy(settings));
            utils::append_to_file("analyse.py",python::plot_lie());
            std::stringstream caption;
            caption.str("");
            caption << "Ligand interaction energy between all residues in " << settings.RECEPTOR_MASK << " and residues in " << settings.LIGAND_MASK << ".";
            latex::figure_block_to_file("Ligand_Interaction_Energy",caption.str(),"00_Report/analysis_LIE.tex");
            latex::python_block_to_file(python::plot_lie(), "00_Report/analysis_LIE.tex");
            // SASA
            utils::append_to_file("analyse.py",python::plot_sasa());
            caption.str("");
            caption << "Solvent accessible surface areas (SASA) for receptor (" << settings.RECEPTOR_MASK << "), ligand (" << settings.LIGAND_MASK << "), and complex (" << settings.COMPLEX_MASK << "), plus buried interface (\\$A_{receptor} + A_{ligand} - A_{complex}\\$)";
            latex::figure_block_to_file("SASA",caption.str(),"00_Report/analysis_SASA.tex");
            latex::python_block_to_file(python::plot_sasa(), "00_Report/analysis_SASA.tex");
        }

        if (settings.NORMAL_MODES_MASK != " ") // Handle Normal Modes
        {
            utils::append_to_file("cpptraj.in",cpptraj::normal_modes(settings));
            utils::append_to_file("analyse.py",python::plot_normal_modes());
            utils::append_to_file("analyse.py",python::plot_pca());

            std::stringstream caption;
            
            caption.str("");
            caption << "Largest vibrational modes by residue in" << settings.NORMAL_MODES_MASK << ".";
            latex::figure_block_to_file("Largest_Normal_Modes", caption.str(), "00_Report/analysis_NORMAL_MODES.tex");
            latex::python_block_to_file(python::plot_normal_modes(), "00_Report/analysis_NORMAL_MODES.tex");
                        
            caption.str("");
            caption << "Principle component analysis comparing the two largest vibrational modes in " << settings.NORMAL_MODES_MASK << ".";
            latex::figure_block_to_file("PrincipleComponentAnalysis", caption.str(), "00_Report/analysis_NORMAL_MODES.tex");
            latex::python_block_to_file(python::plot_pca(), "00_Report/analysis_NORMAL_MODES.tex");
        }

        prepare_clustering(settings);  // Handle Clustering (based on what...?)

        utils::append_to_file("cpptraj.in",settings.CPPTRAJ_EXTRA_COMMANDS);

        // run cpptraj.in
        utils::silent_shell("cpptraj < cpptraj.in >> cpptraj.out");

        // parse .nmd to .csv with parse_nmd_to_mode_csv(std::string nmdfile, std::string csvfile)
        parse_nmd_to_mode_csv();

        // run analyse.py
        utils::silent_shell("python analyse.py");
        
        // Parse the HBond results into a nice table.
        parse_hbonds_avg(settings);

        // MMPBSA processing to plots!

        // generate ambermachine.vmd and run for each: RMSF, Normal Modes (first 4 modes), correl (if ligand + receptor mask)
        // RMSF
        // 1.  get array of values from RMSF.dat
        // 2.  call vmdwriter functions
        // 3.  run VMD command.

        // Normal Modes
        // 1.  get array(s) of modes from normal_modes.csv
        // 2.  call vmdwriter function
        // 3.  run vmd command.

        // If ligand mask + receptor mask:
        // 1.  get correlation array based on ligand mask?
        // 2.  call vmdwriter functions
        // 3.  run vmd command.

        // If ligand mask + receptor mask:
        // Parse LIG_Hbond_table for intermolecular pairs, draw arrows from donorHs to acceptors
        // ... run vmd command for this.

        // compile the final report
        latex::compile_report(settings);

        // Create .AMBER_ANALYSIS_COMPLETE
        utils::write_to_file(".AMBER_ANALYSIS_COMPLETE","");
        // Cleanup
        slurm::cleanup_out_err(slurm);
        return;
    }
}
