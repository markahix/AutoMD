#ifndef CONSTANTPHDYNAMICS_H
#define CONSTANTPHDYNAMICS_H
#include "basedynamics.h"

namespace constph
{
    void PrepareConstantpHDynamics(JobSettings settings, SlurmSettings slurm, FileList files);
    void RunConstantpHDynamics(JobSettings settings, SlurmSettings slurm, FileList files, double ph_level);    
}

#endif