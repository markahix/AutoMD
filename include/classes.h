#ifndef classes_h
#define classes_h

#include "utilities.h"

class SlurmSettings
{
    public:
        std::string SLURM_JOB_ID;
        std::string SLURM_JOBNAME;
        std::string SLURM_partition;
        std::string SLURM_queue;
        std::string SLURM_gpu;
        std::string SLURM_nodelist;
        std::string SLURM_exclude_nodes;
        std::string SLURM_amber_module;
        std::string SLURM_executable;

        SlurmSettings();
        ~SlurmSettings();
};

class JobSettings
{
    public:
        // Input Filenames
        std::string PRMTOP;
        std::string INPCRD;

        // Job Settings
        double MAX_RESTRAINT;
        int NUM_COLD_STEPS;
        int NUM_HOT_STEPS;
        int NUM_PROD_STEPS;
        bool COMPRESS_STAGES;
        
        //MMPBSA settings
        bool RUN_MMPBSA;
        std::string RECEPTOR_MASK;
        std::string LIGAND_MASK;
        std::string COMPLEX_MASK;
        double SALT_CONC;

        //Analysis Settings
        std::string RMSD_MASK;
        std::string RMSF_MASK;
        std::string HBONDS_MASK;
        std::string NORMAL_MODES_MASK;
        std::string CORREL_MASK;
        std::string SOLVENT_MASK;
        std::string COUNTERIONS_MASK;
        std::string CPPTRAJ_EXTRA_COMMANDS;

        // Constructor/Destructor
        JobSettings();
        ~JobSettings();
};

#endif