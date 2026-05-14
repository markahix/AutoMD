#ifndef CLASSICALDYNAMICS_H
#define CLASSICALDYNAMICS_H
#include "basedynamics.h"

namespace classical
{
    void RunClassicalDynamics(JobSettings settings, SlurmSettings slurm, FileList files);
}

#endif