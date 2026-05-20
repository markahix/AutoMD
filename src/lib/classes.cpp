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
    // Absolute Paths
    std::cout << "Got Job Submission Directory." << std::endl;
    if (std::getenv("SLURM_SUBMIT_DIR")==NULL)
    {
        SUBMISSION_DIRECTORY = fs::absolute("./");
    }
    else
    {
        SUBMISSION_DIRECTORY = std::getenv("SLURM_SUBMIT_DIR");
    }
    PRODUCTION_DIRECTORY = fs::absolute(SUBMISSION_DIRECTORY + "/05_Production/");
    normal_log("Job Submitted From: " + SUBMISSION_DIRECTORY);
    if (fs::is_directory(PRODUCTION_DIRECTORY))
    {
        normal_log("Current Production Directory: " + PRODUCTION_DIRECTORY);
    }
    

    // Input Filenames
    PRMTOP = "file.prmtop";
    INPCRD = "file.rst7";

    // Job Settings
    NUM_PROD_STEPS   = 100;
    FRAMES_PER_NS    = 1000;
    TRAJ_WRITE_FREQ  = 1000000/FRAMES_PER_NS;
    COMPRESS_STAGES  = false;

    // Environmental Settings
    TEMPERATURE = 300;

    // Constant pH Settings
    RUN_CONSTANT_PH = false;
    MIN_PH_LEVEL = 4.0;
    MAX_PH_LEVEL = 9.0;
    PH_INCREMENT = 0.5;

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

void JobSettings::UpdateProductionDirectory(std::string new_directory)
{
    std::string tmp_new_dir = fs::absolute(new_directory);
    if (fs::is_directory(tmp_new_dir))
    {
        PRODUCTION_DIRECTORY = tmp_new_dir;
        normal_log("Current Production Directory: " + PRODUCTION_DIRECTORY);
    }
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