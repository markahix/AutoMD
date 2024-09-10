#include "ambermachine.h"

// prepare cpptraj input, python post-processing, and latex for each section rather than trying to have all these separate things for each stage.  it's getting messy.

// void prepare_analysis_files(JobSettings settings)
// {
//     std::stringstream cpptraj;
//     std::stringstream python;
//     std::stringstream report;
//     cpptraj.str();
//     python.str();
//     report.str();

//     // create cpptraj.in, load prmtop and trajectories, autoimage
//     cpptraj << "parm " << settings.PRMTOP << std::endl;
//     for (std::experimental::filesystem::path p : std::experimental::filesystem::directory_iterator("05_Production/"))
//     {
//         if (p.extension() == ".mdcrd")
//         {
//         cpptraj << "trajin " << p << std::endl;
//         }
//     }
//     cpptraj << "reference " << settings.INPCRD << std::endl;
//     cpptraj << "autoimage" << std::endl;
//     cpptraj << "strip !" << settings.COMPLEX_MASK << std::endl;
//     utils::write_to_file("cpptraj.in",cpptraj.str());

//     // create analyse.py, load all necessary libraries.
//     python << "#!/usr/bin/env python3" << std::endl;
//     python << "import matplotlib" << std::endl;
//     python << "matplotlib.use('Agg')" << std::endl;
//     python << "import matplotlib.pyplot as plt" << std::endl;
//     python << "import matplotlib.cm as cm" << std::endl;
//     python << "from scipy.ndimage import gaussian_filter" << std::endl;
//     python << "import numpy as np" << std::endl;
//     python << "import pandas as pd" << std::endl;
//     python << "" << std::endl;
//     python << "" << std::endl;
//     python << "" << std::endl;
//     utils::write_to_file("analyse.py",python.str());
// }
// void prepare_rmsd(JobSettings settings)
// {
//     std::stringstream cpptraj;
//     std::stringstream python;
//     std::stringstream report;
//     cpptraj.str();
//     python.str();
//     report.str();

//     // append command to cpptraj.in
//     cpptraj << "rms reference perres out 06_Analysis/RMSD.dat " << settings.RMSD_MASK << std::endl;
//     utils::append_to_file("cpptraj.in",cpptraj.str());

//     // append plot to analyse.py
//     python << "data = np.genfromtxt('06_Analysis/RMSD.dat',skip_header=1,usecols=1)" << std::endl;
//     python <<  R"PYTHON(x = np.arange(0,len(data),dtype=float)/1000
// smoothed = gaussian_filter(data,sigma=100)
// fig = plt.figure(figsize=(6,4),dpi=300)
// ax = fig.add_subplot(1,1,1)
// ax.set_xlabel("Time (ns)")
// ax.set_ylabel(r"RMSD ($\AA$)")
// ax.set_xlim(0,x.max())
// ax.plot(x,data,color="grey",alpha=0.5)
// ax.plot(x,smoothed,color="blue",alpha=1.0)
// )PYTHON";
//     python << "fig.savefig('00_Report/RMSD.png',dpi=300,facecolor='white')" << std::endl << std::endl;
//     utils::append_to_file("analyse.py",python.str());
    
//     // append results to report latex.
//     std::stringstream caption;
//     caption.str();
//     caption << "Root-mean-squared deviation (RMSD) for all residues in " << settings.RMSD_MASK << ".";
//     latex::figure_block_to_file("RMSD.png",caption.str(),"00_Report/analysis_RMSD.tex");
//     latex::python_block_to_file(python.str(), "00_Report/analysis_RMSD.tex");
// }
// void prepare_rmsf(JobSettings settings)
// {
//     std::stringstream cpptraj;
//     std::stringstream python;
//     std::stringstream report;
//     cpptraj.str();
//     python.str();
//     report.str();
//     // append command to cpptraj.in
//     cpptraj << "atomicfluct out 06_Analysis/RMSF_ByRes.dat " << settings.RMSF_MASK << " byres " << std::endl;
//     utils::append_to_file("cpptraj.in",cpptraj.str());

//     // append plot to analyse.py
//     python << "" << std::endl;
//     python <<  R"PYTHON(data = np.genfromtxt('06_Analysis/RMSF_ByRes.dat',skip_header=1,usecols=1)
// fig = plt.figure(figsize=(6,4),dpi=300)
// ax = fig.add_subplot(1,1,1)
// ax.set_xlim(0,len(data)+1)
// ax.bar(np.arange(1,len(data)+1,1),data,align="center")
// fig.savefig('00_Report/RMSF.png',dpi=300,facecolor='white')
// )PYTHON";
//     // figure out how to get VMD to do residue color-by-RMSF via script.  If able, add this as a function (color by array so it can be used in multiple ways easily)
//     utils::append_to_file("analyse.py",python.str());
    
//     // append results to report latex.
//     std::stringstream caption;
//     caption.str();
//     caption << "Root-mean-squared fluctuation (RMSF) for all residues in " << settings.RMSF_MASK << ".";
//     latex::figure_block_to_file("RMSD.png",caption.str(),"00_Report/analysis_RMSF.tex");
//     latex::python_block_to_file(python.str(), "00_Report/analysis_RMSF.tex");

// }
// void prepare_correlation(JobSettings settings)
// {
//     std::stringstream cpptraj;
//     std::stringstream python;
//     std::stringstream report;
//     cpptraj.str();
//     python.str();
//     report.str();
//     // append command to cpptraj.in
//     cpptraj << "matrix correl name corrpath out 06_Analysis/Correlation.dat " << settings.CORREL_MASK << " byres" << std::endl;
//     utils::append_to_file("cpptraj.in",cpptraj.str());

//     // append plot to analyse.py
//     python <<  R"PYTHON(data = np.genfromtxt('06_Analysis/Correlation.dat')
// fig = plt.figure(figsize=(7,6),dpi=300)
// dims = data.shape[0]
// X, Y = np.mgrid[0:dims:complex(0, dims), 0:dims:complex(0, dims)]
// ax = fig.add_subplot(1,1,1)
// im = ax.pcolormesh(X,Y,data,cmap="RdBu",vmin=-1.,vmax=1.)
// fig.colorbar(im,ticks=[-1,0,1],ax=ax)
// ax.xaxis.tick_bottom()
// plt.xticks(rotation=90)
// fig.savefig('00_Report/Correlated_Motion.png',dpi=300,facecolor='white')
// )PYTHON";
//     utils::append_to_file("analyse.py",python.str());
    
//     // append results to report latex.
//     std::stringstream caption;
//     caption.str();
//     caption << "Correlated motion for all residue pairs in " << settings.CORREL_MASK << ".  Regions in blue indicate strong positive correlation (moving together) and regions in red indicate strong anticorrelation (moving opposite).";
//     latex::figure_block_to_file("Correlated_Motion.png",caption.str(),"00_Report/analysis_CORREL.tex");
//     latex::python_block_to_file(python.str(), "00_Report/analysis_CORREL.tex");

// }
// void prepare_lie(JobSettings settings)
// {
//     std::stringstream cpptraj;
//     std::stringstream python;
//     std::stringstream report;
//     cpptraj.str();
//     python.str();
//     report.str();

//     // append command to cpptraj.in
//     cpptraj << "lie LigandInteractionEnergy " << settings.RECEPTOR_MASK << " " << settings.LIGAND_MASK << " out 06_Analysis/LigInterEnergy.dat" << std::endl;
//     utils::append_to_file("cpptraj.in",cpptraj.str());

//     // append plot to analyse.py
//     python <<  R"PYTHON(data = np.genfromtxt('06_Analysis/LigInterEnergy.dat',usecols=(1,2),skip_header=1)
//     coulomb = data[:,0]
// vdw = data[:,1]
// fig = plt.figure(figsize=(6,8),dpi=300)
// ax = fig.add_subplot(2,1,1)
// x = np.arange(0,len(coulomb),dtype=float)/1000
// smoothed = gaussian_filter(coulomb,sigma=100)
// ax.set_xlabel("Time (ns)")
// ax.set_ylabel(r"Coulomb Energy ($kcal\cdot mol^{-1}$)")
// ax.set_xlim(0,x.max())
// ax.plot(x,coulomb,color="grey",alpha=0.5)
// ax.plot(x,smoothed,color="blue",alpha=1.0)
// ax = fig.add_subplot(2,1,2)
// x = np.arange(0,len(vdw),dtype=float)/1000
// smoothed = gaussian_filter(vdw,sigma=100)
// ax.set_xlabel("Time (ns)")
// ax.set_ylabel(r"van der Waals Energy ($kcal\cdot mol^{-1}$)")
// ax.set_xlim(0,x.max())
// ax.plot(x,coulomb,color="grey",alpha=0.5)
// ax.plot(x,smoothed,color="blue",alpha=1.0)
// fig.savefig('00_Report/Ligand_Interaction_Energy.png',dpi=300,facecolor='white')
// )PYTHON";
//     utils::append_to_file("analyse.py",python.str());

//     // append results to report latex.
//     std::stringstream caption;
//     caption.str();
//     caption << "Ligand interaction energy between all residues in " << settings.RECEPTOR_MASK << " and residues in " << settings.LIGAND_MASK << ".";
//     latex::figure_block_to_file("Ligand_Interaction_Energy.png",caption.str(),"00_Report/analysis_LIE.tex");
//     latex::python_block_to_file(python.str(), "00_Report/analysis_LIE.tex");

// }
// void prepare_hbonds(JobSettings settings)
// {
//     std::stringstream cpptraj;
//     std::stringstream python;
//     std::stringstream report;
//     cpptraj.str();
//     python.str();
//     report.str();
//     // append command to cpptraj.in
//     cpptraj << "hbond " << settings.HBONDS_MASK << " out 06_Analysis/HBonds_v_Time.dat avgout 06_Analysis/HBond_Table.dat" << std::endl;
//     if (settings.LIGAND_MASK != " " && settings.RECEPTOR_MASK != " ")
//     {
//         cpptraj << "hbond " << settings.HBONDS_MASK << " out 06_Analysis/LIG_HBonds_v_Time.dat avgout 06_Analysis/LIG_HBond_Table.dat nointramol" << std::endl;
//     }
//     // Have to parse the results to generate the LaTeX table afterwards.

// }
// void prepare_nma(JobSettings settings)
// {
//     std::stringstream cpptraj;
//     std::stringstream python;
//     std::stringstream report;
//     cpptraj.str();
//     python.str();
//     report.str();
//     // append command to cpptraj.in
//     cpptraj << "matrix covar name nma_covar " << settings.NORMAL_MODES_MASK << std::endl;
//     cpptraj << "diagmatrix nma_covar out nma_covar.mat vecs 100 reduce nmwiz nmwizvecs 100 nmwizfile 06_Analysis/nma_first_100_modes.nmd nmwizmask " << settings.NORMAL_MODES_MASK << std::endl;

//     // append plot to analyse.py
//     // append results to report latex.

// }
// void prepare_clustering(JobSettings settings)
// {
//     std::stringstream cpptraj;
//     std::stringstream python;
//     std::stringstream report;
//     cpptraj.str();
//     python.str();
//     report.str();
//     // append command to cpptraj.in
//     // append plot to analyse.py
//     // append results to report latex.

// }
// void prepare_extra_cpptraj_commands(JobSettings settings)
// {
//     std::stringstream cpptraj;
//     std::stringstream python;
//     std::stringstream report;
//     cpptraj.str();
//     python.str();
//     report.str();
//     // append command to cpptraj.in
//     // append plot to analyse.py
//     // append results to report latex.

// }

bool run_cpptraj(JobSettings settings, SlurmSettings slurm)
{
    std::stringstream buffer;
    bool should_cpptraj_run = false;
    slurm::update_job_name("Generating_CPPTRAJ_Input");
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
    buffer << "autoimage" << std::endl;
    buffer << "strip !" << settings.COMPLEX_MASK << std::endl;
    int data_count = 1;
    std::stringstream lead_zero_number;
    if (settings.RMSD_MASK != " ")
    {
        buffer << "rms reference perres out RMSD.dat " << settings.RMSD_MASK << std::endl;
        should_cpptraj_run = true;
    }
    if (settings.RMSF_MASK != " ")
    {
        buffer << "atomicfluct out RMSF_ByRes.dat " << settings.RMSF_MASK << " byres" << std::endl;
        should_cpptraj_run = true;
    }
    if (settings.HBONDS_MASK != " ")
    {
        lead_zero_number.str("");
        lead_zero_number << std::setw(3) << std::setfill('0') << data_count;
        buffer << "hbond " << settings.HBONDS_MASK << " out " << lead_zero_number.str() << "_HBonds_v_Time.dat avgout HBond_Table.dat" << std::endl;
        should_cpptraj_run = true;
        data_count++;
    }    
    if (settings.CORREL_MASK != " ")
    {
        buffer << "matrix correl name corrpath out correlation.dat " << settings.CORREL_MASK << " byres" << std::endl;
        should_cpptraj_run = true;
    }
    if (settings.NORMAL_MODES_MASK != " ")
    {
        buffer << "matrix covar name nma_covar " << settings.NORMAL_MODES_MASK << std::endl;
        buffer << "diagmatrix nma_covar out nma_covar.mat vecs 100 reduce nmwiz nmwizvecs 100 nmwizfile nma_first_100_modes.nmd nmwizmask " << settings.NORMAL_MODES_MASK << std::endl;
        should_cpptraj_run = true;
    }
    if (settings.RECEPTOR_MASK != " " && settings.LIGAND_MASK != " ")
    {
        lead_zero_number.str("");
        lead_zero_number << std::setw(3) << std::setfill('0') << data_count;
        buffer << "lie " << lead_zero_number.str() << "_LigandInteractionEnergy " << settings.RECEPTOR_MASK << " " << settings.LIGAND_MASK << " out LigInterEnergy.dat" << std::endl;
        should_cpptraj_run = true;
        data_count++;
    }
    if (settings.CPPTRAJ_EXTRA_COMMANDS != " ")
    {
        lead_zero_number.str("");
        lead_zero_number << std::setw(3) << std::setfill('0') << data_count;
        buffer << settings.CPPTRAJ_EXTRA_COMMANDS << std::endl;
        should_cpptraj_run = true;
        data_count++;
    }
    buffer << "run" << std::endl;
    buffer << "quit" << std::endl;
    if (should_cpptraj_run)
    {
        utils::write_to_file("cpptraj_001.in",buffer.str());
        std::cout << "DEBUG: In Heating Function, slurm_amber_module is:  " << slurm.SLURM_amber_module << std::endl;

        buffer.str("");
        buffer << "module load " << slurm.SLURM_amber_module << "; cpptraj < cpptraj_001.in > cpptraj_001.out";
        utils::silent_shell(buffer.str().c_str());
        return true;
    }
    std::cout << "No analyses requested." << std::endl;
    return false;
}

void parse_hbonds_avg(JobSettings settings)
{
    std::stringstream buffer;
    std::vector<std::string> hbond_pairs;
    std::ifstream hbonds("HBond_Table.dat");
    std::string line;
    std::stringstream dummy;
    getline(hbonds,line);

    // begin hbond table in LaTeX. "00_Report/analysis_HBONDS.tex"
    buffer.str("");
    buffer << "\\section{Hydrogen Bonding Analysis}" << std::endl;
    buffer << "\\paragraph{} Hydrogen bonding analysis was performed on atoms in " << settings.HBONDS_MASK << ". " << std::endl;
    if (settings.LIGAND_MASK != " " && settings.RECEPTOR_MASK != " ")
    {
        buffer << "This included a receptor and ligand, and the results their intermolecular interactions are also included." << std::endl;
    }
    buffer << "\\begin{tabular}{|rcl|r|}" << std::endl;
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

        // Add HBond command to list.
        if (acc_num < don_num)
        {
            std::stringstream tmp;
            tmp.str("");
            tmp << "hbond HBI_" << acc_num << "_" << don_num << " :" << acc_num << "," << don_num << " out HBIResPairs_v_Time.dat" << std::endl;
            hbond_pairs.push_back(tmp.str());
        }

        else
        {
            std::stringstream tmp;
            tmp.str("");
            tmp << "hbond HBI_" << don_num << "_" << acc_num << " :" << don_num << "," << acc_num << " out HBIResPairs_v_Time.dat" << std::endl;
            hbond_pairs.push_back(tmp.str());
        }
    }

    buffer << "\\hline" << std::endl;
    buffer << "\\end{tabular}" << std::endl;
    utils::append_to_file("00_Report/analysis_HBONDS.tex",buffer.str());
    // convert hbond_pairs vector to unique set:
    std::set<std::string> s( hbond_pairs.begin(), hbond_pairs.end() );
    hbond_pairs.assign( s.begin(), s.end() );

    // write each residue pair to the cpptraj_002.in file for hbonds
    for (int i = 0; i < hbond_pairs.size(); i++) 
    {
        utils::append_to_file("cpptraj_002.in",hbond_pairs[i]);
    }
    
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

void parse_nmd_to_mode_csv(std::string nmdfile, std::string csvfile)
{
    std::ifstream ifile;
    std::ofstream ofile;
    ifile.open(nmdfile);
    ofile.open(csvfile,std::ios::app);
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

std::string Plot_First_Normal_Modes(std::string csvfile, std::string outbase, JobSettings settings)
{
    std::stringstream latex;
    std::stringstream pyscript;
    pyscript << "data = np.genfromtxt('" << csvfile << "',delimiter=',')" << std::endl;
    pyscript << "num_of_modes = 6" << std::endl;
    pyscript << R"PYTHON(fig = plt.figure(figsize=[6,4],dpi=300)
ax = fig.add_subplot(1,1,1)
cmap = cm.get_cmap("viridis")
color_range = np.linspace(0,1,num_of_modes)
for i in range(num_of_modes):
    ax.bar(np.arange(1, len(data[i])+1), data[i], width=1, align="center", color=cmap(color_range[i]), label = f"Mode {i+1}")
ax.set_title("First 4 Vibrational Modes")
ax.legend()
ax.set_xlim(0,len(data[0]]))
plt.xticks(rotation = 90)
fig.tight_layout())PYTHON";
    pyscript << "fig.savefig('00_Report/" << outbase << ".png',dpi=300,facecolor='white')" << std::endl;

    std::stringstream caption;
    caption.str();
    caption << "Largest vibrational modes by residue in" << settings.NORMAL_MODES_MASK << ".";
    latex::figure_block_to_file(outbase, caption.str(), "00_Report/analysis_NORMAL_MODES.tex");

    std::stringstream buffer;
    buffer.str();
    buffer << "rm " << csvfile;
    utils::silent_shell(buffer.str().c_str());
    return pyscript.str();
}

std::string Plot_PCA(std::string nmd_file, std::string outbase, JobSettings settings)
{
    std::stringstream latex;
    std::stringstream pyscript;
    pyscript << "data = np.genfromtxt('" << nmd_file << "',delimiter = None,skip_header = 9)" << std::endl;
    pyscript <<  R"PYTHON(area = np.pi * (2)**2
x = data[0,2] * data[0,3:]
y = data[1,2] * data[1,3:]
z = -(-x**2 - y**2)
fig = plt.figure(figsize=(6,6))
ax = fig.add_subplot(1,1,1)
ax.set_xlabel("Mode 1",fontsize=16)
ax.set_ylabel("Mode 2",fontsize=16)
ax.scatter(x,y,marker='o', s=area, zorder=10, alpha=0.4, c=z, edgecolors = 'black', cmap='viridis')
plt.xticks(fontsize=14,rotation=90)
plt.yticks(fontsize=14)
plt.tight_layout()
)PYTHON";
    pyscript << "fig.savefig('00_Report/" << outbase << ".png',dpi=300,facecolor='white')" << std::endl;

    latex << R"LATEX(
\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{)LATEX" << outbase << R"LATEX2(.png}
\caption{Principle component analysis comparing the two largest vibrational modes in )LATEX2" << settings.NORMAL_MODES_MASK << R"LATEX3(.}
\label{fig:)LATEX3" << outbase << R"LATEX4(}
\end{figure}
)LATEX4";
    utils::append_to_file("00_Report/analysis_NORMAL_MODES.tex",latex.str());
    return pyscript.str();

}

std::string Plot_Correlation(std::string correl_file, std::string outbase, JobSettings settings)
{
    std::stringstream latex;
    std::stringstream pyscript;
    pyscript << "data = np.genfromtxt('" << correl_file << "')" << std::endl;
    pyscript <<  R"PYTHON(fig = plt.figure(figsize=(7,6),dpi=300)
dims = data.shape[0]
X, Y = np.mgrid[0:dims:complex(0, dims), 0:dims:complex(0, dims)]
ax = fig.add_subplot(1,1,1)
im = ax.pcolormesh(X,Y,data,cmap="RdBu",vmin=-1.,vmax=1.)
fig.colorbar(im,ticks=[-1,0,1],ax=ax)
ax.xaxis.tick_bottom()
plt.xticks(rotation=90)
)PYTHON";
    pyscript << "fig.savefig('00_Report/" << outbase << ".png',dpi=300,facecolor='white')" << std::endl;

    latex << R"LATEX(
\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{)LATEX" << outbase << R"LATEX2(.png}
\caption{Correlated motion between all residues in )LATEX2" << settings.CORREL_MASK << R"LATEX3(.}
\label{fig:)LATEX3" << outbase << R"LATEX4(}
\end{figure}
)LATEX4";
    utils::append_to_file("00_Report/analysis_CORREL.tex",latex.str());
    return pyscript.str();
}

std::string Plot_RMSD(std::string rmsd_file, std::string outbase, JobSettings settings)
{
    std::stringstream latex;
    std::stringstream pyscript;
    pyscript << "data = np.genfromtxt('" << rmsd_file << "',skip_header=1,usecols=1)" << std::endl;
    pyscript <<  R"PYTHON(x = np.arange(0,len(data),dtype=float)/1000
smoothed = gaussian_filter(data,sigma=100)
fig = plt.figure(figsize=(6,4),dpi=300)
ax = fig.add_subplot(1,1,1)
ax.set_xlabel("Time (ns)")
ax.set_ylabel(r"RMSD ($\AA$)")
ax.set_xlim(0,x.max())
ax.plot(x,data,color="grey",alpha=0.5)
ax.plot(x,smoothed,color="blue",alpha=1.0)
)PYTHON";
    pyscript << "fig.savefig('00_Report/" << outbase << ".png',dpi=300,facecolor='white')" << std::endl;

    latex << R"LATEX(
\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{)LATEX" << outbase << R"LATEX2(.png}
\caption{Root-mean-squared deviation for all residues in )LATEX2" << settings.RMSD_MASK << R"LATEX3(.}
\label{fig:)LATEX3" << outbase << R"LATEX4(}
\end{figure}
)LATEX4";
    utils::append_to_file("00_Report/analysis_RMSD.tex",latex.str());
    return pyscript.str();
}

std::string Plot_RMSF(std::string rmsf_file, std::string outbase, JobSettings settings)
{
    std::stringstream latex;
    std::stringstream pyscript;
    pyscript << "data = np.genfromtxt('" << rmsf_file << "',skip_header=1,usecols=1)" << std::endl;
    pyscript <<  R"PYTHON(fig = plt.figure(figsize=(6,4),dpi=300)
ax = fig.add_subplot(1,1,1)
ax.set_xlim(0,len(data)+1)
ax.bar(np.arange(1,len(data)+1,1),data,align="center")
)PYTHON";
    pyscript << "fig.savefig('00_Report/" << outbase << ".png',dpi=300,facecolor='white')" << std::endl;

    latex << R"LATEX(
\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{)LATEX" << outbase << R"LATEX2(.png}
\caption{Root-mean-squared fluctuation for all residues in )LATEX2" << settings.RMSF_MASK << R"LATEX3(.}
\label{fig:)LATEX3" << outbase << R"LATEX4(}
\end{figure}
)LATEX4";
    utils::append_to_file("00_Report/analysis_RMSF.tex",latex.str());
    return pyscript.str();
}

std::string Plot_L_I_E(std::string lie_file, std::string outbase, JobSettings settings)
{
    std::stringstream latex;
    std::stringstream pyscript;
    pyscript << "data = np.genfromtxt('" << lie_file << "',usecols=(1,2),skip_header=1)" << std::endl;
    pyscript <<  R"PYTHON(coulomb = data[:,0]
vdw = data[:,1]
fig = plt.figure(figsize=(6,8),dpi=300)
ax = fig.add_subplot(2,1,1)
x = np.arange(0,len(coulomb),dtype=float)/1000
smoothed = gaussian_filter(coulomb,sigma=100)
ax.set_xlabel("Time (ns)")
ax.set_ylabel(r"Coulomb Energy ($kcal\cdot mol^{-1}$)")
ax.set_xlim(0,x.max())
ax.plot(x,coulomb,color="grey",alpha=0.5)
ax.plot(x,smoothed,color="blue",alpha=1.0)
ax = fig.add_subplot(2,1,2)
x = np.arange(0,len(vdw),dtype=float)/1000
smoothed = gaussian_filter(vdw,sigma=100)
ax.set_xlabel("Time (ns)")
ax.set_ylabel(r"van der Waals Energy ($kcal\cdot mol^{-1}$)")
ax.set_xlim(0,x.max())
ax.plot(x,coulomb,color="grey",alpha=0.5)
ax.plot(x,smoothed,color="blue",alpha=1.0)
)PYTHON";
    pyscript << "fig.savefig('00_Report/" << outbase << ".png',dpi=300,facecolor='white')" << std::endl;

    latex << R"LATEX(
\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{)LATEX" << outbase << R"LATEX2(.png}
\caption{Ligand interaction energies between receptor ()LATEX2" << settings.RECEPTOR_MASK << R"LATEX3( and ligand ()LATEX3" << settings.LIGAND_MASK << R"LATEX4(.}
\label{fig:)LATEX4" << outbase << R"LATEX5(}
\end{figure}
)LATEX5";
    utils::append_to_file("00_Report/analysis.tex",latex.str());
    return pyscript.str();
}

std::string Plot_SASA(std::string sasa_file, std::string outbase, JobSettings settings)
{
    std::stringstream latex;
    std::stringstream pyscript;
    pyscript << "df = pd.read_csv('" << sasa_file << "',delim_whitespace=True)" << std::endl;
    pyscript <<  R"PYTHON(
df["Time (ns)"] = np.arange(0,len(df["#Frame"]),dtype=float)/1000
df = df[df["Receptor"] != -1]
df = df[df["Ligand"] != -1]
df = df[df["Complex"] != -1]
df["Interface"] = df["Receptor"] + df["Ligand"] - df["Complex"]
fig = plt.figure(figsize=(7.5,10),dpi=300)
ax = fig.add_subplot(4,1,1)
data = df["Receptor"]
smoothed = gaussian_filter(data,sigma=100)
ax.plot(df["Time (ns)"],data,color="grey",alpha=0.5)
ax.plot(df["Time (ns)"],smoothed,color="blue",alpha=1.0)
ax.set_ylabel(r"Receptor SASA ($\AA^2$)")
ax.set_xticks([])
ax = fig.add_subplot(4,1,2)
data = df["Ligand"]
smoothed = gaussian_filter(data,sigma=100)
ax.plot(df["Time (ns)"],data,color="grey",alpha=0.5)
ax.plot(df["Time (ns)"],smoothed,color="blue",alpha=1.0)
ax.set_ylabel(r"Ligand SASA ($\AA^2$)")
ax.set_xticks([])
ax = fig.add_subplot(4,1,3)
data = df["Complex"]
smoothed = gaussian_filter(data,sigma=100)
ax.plot(df["Time (ns)"],data,color="grey",alpha=0.5)
ax.plot(df["Time (ns)"],smoothed,color="blue",alpha=1.0)
ax.set_ylabel(r"Complex SASA ($\AA^2$)")
ax.set_xticks([])
ax = fig.add_subplot(4,1,4)
data = df["Interface"]
smoothed = gaussian_filter(data,sigma=100)
ax.plot(df["Time (ns)"],data,color="grey",alpha=0.5)
ax.plot(df["Time (ns)"],smoothed,color="blue",alpha=1.0)
ax.set_ylabel(r"Buried Interface ($\AA^2$)")
ax.set_xlabel("Time (ns)")
ax.set_xlim(0,max(df["Time (ns)"]))
fig.subplots_adjust(hspace=0.0)
)PYTHON";
    pyscript << "fig.savefig('00_Report/" << outbase << ".png',dpi=300,facecolor='white')" << std::endl;

    latex << R"LATEX(
\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{)LATEX" << outbase << R"LATEX2(.png}
\caption{Solvent accessible surface areas (SASA) for receptor ( )LATEX2" <<
    settings.RECEPTOR_MASK << R"LATEX3(), ligand ( )LATEX3" << settings.LIGAND_MASK << R"LATEX4(), and complex ()LATEX4" << settings.COMPLEX_MASK << R"LATEX5(), plus buried interface (\$A_{receptor} + A_{ligand} - A_{complex}\$)}
\label{fig: )LATEX5" << outbase << R"LATEX6(}
\end{figure}
)LATEX6";
    utils::append_to_file("00_Report/analysis.tex",latex.str());
    return pyscript.str();
}

void plot_time_dependent_datafiles(JobSettings settings, SlurmSettings slurm)
{
    std::stringstream pyscript;
    pyscript << R"PREAMBLE(#!/usr/bin/env python3
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.ndimage import gaussian_filter
import numpy as np
import pandas as pd
)PREAMBLE";
    pyscript << std::endl;
    if (utils::CheckFileExists("RMSD.dat"))
    {
        pyscript << Plot_RMSD("RMSD.dat", "RMSD", settings) << std::endl;
    }

    if (utils::CheckFileExists("RMSF_ByRes.dat"))
    {
        pyscript << Plot_RMSF("RMSF_ByRes.dat", "RMSF", settings) << std::endl;
    }

    if (utils::CheckFileExists("nma_first_100_modes.nmd"))
    {
        pyscript << Plot_PCA("nma_first_100_modes.nmd", "PCA_first_two_modes", settings) << std::endl;
        parse_nmd_to_mode_csv("nma_first_100_modes.nmd", "nma_first_100_modes.csv");
        pyscript << Plot_First_Normal_Modes("nma_first_100_modes.csv","NMA_largest_6_modes",settings) << std::endl;
    }

    if (utils::CheckFileExists("LigInterEnergy.dat"))
    {
        pyscript << Plot_L_I_E("LigInterenergy.dat", "Ligand_Interaction_Energy", settings) << std::endl;
    }
    
    // if (utils::CheckFileExists(""))
    //{
    //
    //}

    // if (utils::CheckFileExists(""))
    //{
    //
    //}

    // if (utils::CheckFileExists(""))
    //{
    //
    //}

    // if (utils::CheckFileExists(""))
    //{
    //
    //}

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
        // run cpptraj.in
        // run analyse.py
        // compile the final report


        // run analyses.
        if (run_cpptraj(settings,slurm))
        {
            parse_cpptraj_results(settings,slurm);
        }

        // plot generated data -- find datasets of matching lengths to compare with scatter plots, clustering, etc.

        // Okay, we can't use chimera, so we've gotta use VMD if VMD is available.  So that should be contained in an if statement for whether or not VMD is avaiable.
                // if ligand mask, generate 3D structure of receptor colored by correlation?
                // color 3D structure by RMSF
                // color 3D structure by normal mode contributions.

        // Add plots to report, including explanations where possible.


        /*

scontrol update JobId=$SLURM_JOB_ID jobname=$(printf "Running_CPPTRAJ")
#############################
###### Modules to load ######
#############################
module load Amber/20-cuda-11

cpptraj < cpptraj.in >> cpptraj.out


### Compile information to plots, include in 00_Report/analysis.tex
python ~/bin/AmberUtilities/plot_cpptraj_results.py

mkdir -p 06_Analysis
mv *.dat 06_Analysis/
mv *.nmd 06_Analysis/
rm cpptraj.*

cat << EOF >> 00_Report/analysis.tex
\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{PrincipleComponentAnalysis.png}
\caption{Principle component analysis comparing the two largest vibrational modes in $_NORMAL_MODES_MASK.}
\label{fig:pca}
\end{figure}

\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{Correlation.png}
\caption{Correlated motion between all residues in $_CORREL_MASK.}
\label{fig:correl}
\end{figure}

\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{RMSD.png}
\caption{Root-mean-squared deviation for all residues in $_RMSD_MASK.}
\label{fig:rmsd}
\end{figure}

\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{RMSF_ByRes.png}
\caption{Root-mean-squared fluctuation for all residues in $_RMSF_MASK.}
\label{fig:rmsf}
\end{figure}
EOF

if [ -f 00_Report/Ligand_Interaction_Energy.png ]
then
cat << EOF >> 00_Report/analysis.tex

\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{Ligand_Interaction_Energy.png}
\caption{Ligand interaction energies between receptor ($_RECEPTOR_MASK) and ligand ($_LIGAND_MASK).}
\label{fig:lig_inter_energy}
\end{figure}

EOF
fi

if [ -f 00_Report/SolventAccessibleSurfaceArea.png ]
then
cat << EOF >> 00_Report/analysis.tex

\begin{figure}[htbp]
\centering
\includegraphics[width=0.9\textwidth]{SolventAccessibleSurfaceArea.png}
\caption{Solvent accessible surface areas (SASA) for receptor ($_RECEPTOR_MASK), ligand ($_LIGAND_MASK), and complex ($_COMPLEX_MASK), plus buried interface (\$A_{receptor} + A_{ligand} - A_{complex}\$).}
\label{fig:sasa_data}
\end{figure}

EOF
fi
### If MMPBSA, compile to plots


### Compile Current Report
cd 00_Report/
pdflatex main.tex --interaction=non-stop-mode --escape-shell > /dev/null
mv main.pdf ../${_PRMTOP%%.prmtop}_Report.pdf
cd ../

rm AMBER_$SLURM_JOB_ID.*
touch .AMBER_ANALYSIS_COMPLETE
        */
    }
}
