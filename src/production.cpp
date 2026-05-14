#include "constantphdynamics.h"
#include "classicaldynamics.h"

int main(int argc, char** argv)
{
    // Make sure I can actually RUN the classical dynamics simulations.
    if (! utils::CheckProgAvailable("pmemd.cuda"))
    {
        error_log("Unable to locate pmemd.cuda.  Make sure you have provided the correct Amber module.",1);
    }
    normal_log("Located pmemd.cuda at: ");
    std::string pmemd_loc = utils::GetSysResponse("which pmemd");
    normal_log("\t" + pmemd_loc);
    // Variable Declarations.
    JobSettings settings;
    SlurmSettings slurm;
    slurm.SLURM_executable = argv[0];
    FileList files;
    ambermachine::read_amberinput(settings,slurm);
    if (!settings.RUN_CONSTANT_PH)
    {
        classical::RunClassicalDynamics(settings,slurm,files);
    }
    else
    {
        // check command line for current value of pH.  if not present, run generative process for folders and submit subjobs with command line argument.
        if (argc > 2)
        {
            double ph_level = stod((std::string)argv[2]);
            constph::RunConstantpHDynamics(settings,slurm,files,ph_level);
        }
        else
        {
            constph::PrepareConstantpHDynamics(settings,slurm,files);
        }      
    }
    return 0;
}