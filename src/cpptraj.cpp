#include "cpptraj.h"

// Prepare input with parm, trajin, autoimage, and reference.
// check against JobSettings for what specific tasks can be done.

void PrepareCPPTRAJInput(JobSettings settings)
{
    std::stringstream cpptraj;
    cpptraj.str();

    // Load trajectory, autoimage, strip solvent
    cpptraj << "parm " << settings.PRMTOP << std::endl;
    for (fs::path p : fs::directory_iterator("05_Production/"))
    {
        if (p.extension() == ".mdcrd")
        {
        cpptraj << "trajin " << p << std::endl;
        }
    }
    cpptraj << "reference " << settings.INPCRD << std::endl;
    cpptraj << "autoimage" << std::endl;
    cpptraj << "strip " << settings.SOLVENT_MASK << std::endl;
    cpptraj << "strip " << settings.COUNTERIONS_MASK << std::endl;

    // Base Analyses for any/all systems.
    // Expand "COMPLEX_MASK" to other masks?

    if (settings.RMSD_MASK != " ")
    {
        cpptraj << "rms reference perres out 06_Analysis/RMSD.dat " << settings.RMSD_MASK << std::endl;
    }
    if (settings.RMSF_MASK != " ")
    {
        cpptraj << "atomicfluct out 06_Analysis/RMSF_ByRes.dat " << settings.RMSF_MASK << " byres " << std::endl;
    }
    if (settings.CORREL_MASK != " ")
    {
        cpptraj << "matrix correl name corrpath out 06_Analysis/Correlation.dat " << settings.CORREL_MASK << " byres" << std::endl;
    }
    if (settings.HBONDS_MASK != " ")
    {
        cpptraj << "hbond out 06_Analysis/HBonds_v_Time.dat " << settings.HBONDS_MASK << " avgout 06_Analysis/HBond_Table.dat" << std::endl;
    }
    if (settings.NORMAL_MODES_MASK != " ")
    {
        cpptraj << "matrix covar name nma_covar " << settings.NORMAL_MODES_MASK << std::endl;
        cpptraj << "diagmatrix nma_covar out 06_Analysis/nma_covar.mat vecs 100 reduce nmwiz nmwizvecs 100 nmwizfile 06_Analysis/nma_first_100_modes.nmd nmwizmask " << settings.NORMAL_MODES_MASK << std::endl;
    }
    if (settings.LIGAND_MASK != " " && settings.RECEPTOR_MASK != " ")
    {
        cpptraj << "lie LigandInteractionEnergy " << settings.RECEPTOR_MASK << " " << settings.LIGAND_MASK << " out 06_Analysis/LigInterEnergy.dat" << std::endl;
        cpptraj << "hbond " << settings.HBONDS_MASK << " out 06_Analysis/LIG_HBonds_v_Time.dat avgout 06_Analysis/LIG_HBond_Table.dat nointramol" << std::endl;
    }
    if (settings.CPPTRAJ_EXTRA_COMMANDS != " ")
    {
        cpptraj << settings.CPPTRAJ_EXTRA_COMMANDS << std::endl;
    }
    // Run analyses and exit CPPTRAJ.
    cpptraj << "run" << std::endl;
    cpptraj << "quit" << std::endl;

    utils::write_to_file("cpptraj.in",cpptraj.str());
}

void RunCPPTRAJ(JobSettings settings)
{

}

namespace cpptraj
{
    std::string preamble(JobSettings settings){
        std::stringstream cpptraj;
        cpptraj.str("");
        cpptraj << "parm " << settings.PRMTOP << std::endl;
        for (fs::path p : fs::directory_iterator("05_Production/"))
        {
            if (p.extension() == ".mdcrd")
            {
            cpptraj << "trajin " << p << std::endl;
            }
        }
        cpptraj << "reference " << settings.INPCRD << std::endl;
        cpptraj << "autoimage" << std::endl;
        cpptraj << "strip !" << settings.COMPLEX_MASK << std::endl;
        return cpptraj.str();
    }

    std::string rmsd(JobSettings settings){
        std::stringstream cpptraj;
        cpptraj.str("");
        cpptraj << "rms reference perres out 06_Analysis/RMSD.dat " << settings.RMSD_MASK << std::endl;
        return cpptraj.str();
    }

    std::string rmsf(JobSettings settings){
        std::stringstream cpptraj;
        cpptraj.str("");
        cpptraj << "atomicfluct out 06_Analysis/RMSF_ByRes.dat " << settings.RMSF_MASK << " byres " << std::endl;
        return cpptraj.str();
    }

    std::string correl(JobSettings settings){
        std::stringstream cpptraj;
        cpptraj.str("");
        cpptraj << "matrix correl name corrpath out 06_Analysis/Correlation.dat " << settings.CORREL_MASK << " byres" << std::endl;
        return cpptraj.str();
    }

    std::string ligand_interaction_energy(JobSettings settings){
        std::stringstream cpptraj;
        cpptraj.str("");
        cpptraj << "lie LigandInteractionEnergy " << settings.RECEPTOR_MASK << " " << settings.LIGAND_MASK << " out 06_Analysis/LigInterEnergy.dat" << std::endl;
        return cpptraj.str();
    }

    std::string hbonds(JobSettings settings){
        std::stringstream cpptraj;
        cpptraj.str("");
        // append command to cpptraj.in
        cpptraj << "hbond " << settings.HBONDS_MASK << " out 06_Analysis/HBonds_v_Time.dat avgout 06_Analysis/HBond_Table.dat" << std::endl;
        if (settings.LIGAND_MASK != " " && settings.RECEPTOR_MASK != " ")
        {
            cpptraj << "hbond " << settings.HBONDS_MASK << " out 06_Analysis/LIG_HBonds_v_Time.dat avgout 06_Analysis/LIG_HBond_Table.dat nointramol" << std::endl;
        }
        return cpptraj.str();
    }

    std::string normal_modes(JobSettings settings){
        std::stringstream cpptraj;
        cpptraj.str("");
        cpptraj << "matrix covar name nma_covar " << settings.NORMAL_MODES_MASK << std::endl;
        cpptraj << "diagmatrix nma_covar out 06_Analysis/nma_covar.mat vecs 100 reduce nmwiz nmwizvecs 100 nmwizfile 06_Analysis/nma_first_100_modes.nmd nmwizmask " << settings.NORMAL_MODES_MASK << std::endl;
        return cpptraj.str();
    }
}