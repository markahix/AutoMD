#include "utilities.h"
#include "classes.h"

SlurmSettings::SlurmSettings()
{
    char const* temp = std::getenv("SLURM_JOB_ID");
    if(temp != NULL)
    {
        std::string SLURM_JOB_ID = std::string(temp);
    }
    else
    {
        std::string SLURM_JOB_ID = " ";
    }
    std::string SLURM_JOBNAME = "AmberMachine";
    std::string SLURM_partition = "earwp";
    std::string SLURM_queue = "express";
    std::string SLURM_gpu = "gpu:1";
    std::string SLURM_nodelist = " ";
    std::string SLURM_exclude_nodes = " ";
    std::string SLURM_amber_module = "Amber/20-cuda-11";
    std::string SLURM_executable = "ambermachine";
}

SlurmSettings::~SlurmSettings()
{
}

JobSettings::JobSettings()
{
    // Input Filenames
    std::string PRMTOP = "file.prmtop";
    std::string INPCRD = "file.rst7";

    // Job Settings
    double MAX_RESTRAINT = 500.00;
    int NUM_COLD_STEPS   = 10;
    int NUM_HOT_STEPS    = 10;
    int NUM_PROD_STEPS   = 100;
    bool COMPRESS_STAGES = false;
    
    //MMPBSA settings
    bool RUN_MMPBSA = false;
    std::string RECEPTOR_MASK=" ";
    std::string LIGAND_MASK=" ";
    std::string COMPLEX_MASK=" ";
    double SALT_CONC = 0.00;

    //Analysis Settings
    std::string RMSD_MASK=" ";
    std::string RMSF_MASK=" ";
    std::string HBONDS_MASK=" ";
    std::string NORMAL_MODES_MASK=" ";
    std::string CORREL_MASK=" " ;
    std::string SOLVENT_MASK=" ";
    std::string COUNTERIONS_MASK=" ";
    std::string CPPTRAJ_EXTRA_COMMANDS=" ";
}



JobSettings::~JobSettings()
{
    
}
