#include "utilities.h"
#include "classes.h"

SlurmSettings::SlurmSettings()
{
    char const* temp = std::getenv("SLURM_JOB_ID");
    if(temp != NULL)
    {
        SLURM_JOB_ID = std::string(temp);
    }
    else
    {
        SLURM_JOB_ID = " ";
    }
    SLURM_JOBNAME = "AmberMachine";
    SLURM_partition = "earwp";
    SLURM_queue = "express";
    SLURM_gpu = "gpu:1";
    SLURM_nodelist = " ";
    SLURM_exclude_nodes = " ";
    SLURM_amber_module = "Amber/20-cuda-11";
    SLURM_executable = "ambermachine";
}

SlurmSettings::~SlurmSettings()
{
}

JobSettings::JobSettings()
{
    // Input Filenames
    PRMTOP = "file.prmtop";
    INPCRD = "file.rst7";

    // Job Settings
    MAX_RESTRAINT = 500.00;
    NUM_COLD_STEPS   = 10;
    NUM_HOT_STEPS    = 10;
    NUM_PROD_STEPS   = 100;
    COMPRESS_STAGES = false;
    
    //MMPBSA settings
    RUN_MMPBSA = false;
    RECEPTOR_MASK=" ";
    LIGAND_MASK=" ";
    COMPLEX_MASK=" ";
    SALT_CONC = 0.00;

    //Analysis Settings
    RMSD_MASK=" ";
    RMSF_MASK=" ";
    HBONDS_MASK=" ";
    NORMAL_MODES_MASK=" ";
    CORREL_MASK=" " ;
    SOLVENT_MASK=" ";
    COUNTERIONS_MASK=" ";
    CPPTRAJ_EXTRA_COMMANDS=" ";
}

JobSettings::~JobSettings()
{
    
}
