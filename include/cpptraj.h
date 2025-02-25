#ifndef cpptraj_h
#define cpptraj_h

#include "utilities.h"
#include "classes.h"

namespace cpptraj
{
    std::string preamble(JobSettings settings);
    std::string rmsd(JobSettings settings);
    std::string rmsf(JobSettings settings);
    std::string correl(JobSettings settings);
    std::string ligand_interaction_energy(JobSettings settings);
    std::string hbonds(JobSettings settings);
    std::string normal_modes(JobSettings settings);
}

#endif