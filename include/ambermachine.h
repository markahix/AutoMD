#ifndef ambermachine_h
#define ambermachine_h

#include "utilities.h"
#include "classes.h"
#include "slurm.h"
#include "latex.h"
#include "python.h"
#include "cpptraj.h"
#include "vmd.h"

namespace ambermachine
{
    void write_amberinput();
    bool read_amberinput(JobSettings &settings,SlurmSettings &slurm);
    void identify_current_state(JobSettings job_settings, SlurmSettings slurm);
    
    // Main Job Stages (only called by flags on command line)
    void initialize(JobSettings settings,SlurmSettings slurm);
    void minimize(JobSettings settings,SlurmSettings slurm);
    void cold_equilibration(JobSettings settings,SlurmSettings slurm);
    void heating(JobSettings settings,SlurmSettings slurm);
    void hot_equilibration(JobSettings settings,SlurmSettings slurm);
    void production(JobSettings settings,SlurmSettings slurm);
    void mmpbsa(JobSettings settings,SlurmSettings slurm, std::string trajectory);
    void sasa(JobSettings settings,SlurmSettings slurm, std::string trajectory);
    void analysis(JobSettings settings,SlurmSettings slurm);
    void report(JobSettings settings,SlurmSettings slurm);
    void AmberLoop(SlurmSettings slurm);
    void AmberCopyBack(std::string mdin_file,std::string restart_file, std::string mdout_file, std::string trajectory_file, std::string csv_file);

}

#endif