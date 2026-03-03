#include "ambermachine.h"

namespace ambermachine
{
    void report(JobSettings settings,SlurmSettings slurm)
    {
        latex::compile_report(settings);
        slurm::cleanup_out_err(slurm);
    }
}