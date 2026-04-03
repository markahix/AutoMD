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
    SLURM_partition = SLURM_DEFAULT_GPU_JOB_PARTITION;
    SLURM_queue = SLURM_DEFAULT_GPU_JOB_QUEUE;
    SLURM_gpu = SLURM_DEFAULT_GPU_JOB_GPUNAME;
    SLURM_nodelist = SLURM_DEFAULT_GPU_JOB_INCLUDE_NODES;
    SLURM_exclude_nodes = SLURM_DEFAULT_GPU_JOB_EXCLUDE_NODES;
    SLURM_amber_module = DEFAULT_AMBER_MODULE;
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
    // MAX_RESTRAINT    = 500.00;
    // NUM_COLD_STEPS   = 10;
    // NUM_HOT_STEPS    = 10;
    NUM_PROD_STEPS   = 100;
    FRAMES_PER_NS    = 1000;
    TRAJ_WRITE_FREQ  = 1000000/FRAMES_PER_NS;
    COMPRESS_STAGES  = false;

    // Environmental Settings
    TEMPERATURE = 300;

    // MMPBSA settings
    RUN_MMPBSA = false;
    RECEPTOR_MASK=" ";
    LIGAND_MASK=" ";
    COMPLEX_MASK=" ";
    SALT_CONC = 0.00;

    // Analysis Settings
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

FileList::FileList()
{
    files = {};
}
FileList::~FileList()
{
    
}

void FileList::AddFile(std::string key, std::string filename)
{
    files[key] = fs::absolute(filename);
}

std::string FileList::GetFile(std::string key)
{
    return files[key];
}