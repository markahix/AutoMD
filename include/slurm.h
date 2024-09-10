#ifndef slurm_h
#define slurm_h
#include "utilities.h"
#include "classes.h"

namespace slurm
{
    void update_job_name(std::string jobname);
    void submit_initialize_job(JobSettings settings,SlurmSettings slurm);
    void submit_minimize_job(JobSettings settings,SlurmSettings slurm);
    void submit_cold_equil_job(JobSettings settings,SlurmSettings slurm);
    void submit_heating_job(JobSettings settings,SlurmSettings slurm);
    void submit_hot_equil_job(JobSettings settings,SlurmSettings slurm);
    void submit_production_job(JobSettings settings,SlurmSettings slurm);
    void submit_mmpbsa_job(JobSettings settings, SlurmSettings slurm, std::string trajectory);
    void submit_sasa_job(JobSettings settings, SlurmSettings slurm, std::string trajectory);
    void submit_analysis_job(JobSettings settings,SlurmSettings slurm);
    void submit_reporting_job(JobSettings settings,SlurmSettings slurm);
    void cleanup_out_err(SlurmSettings slurm);
    void remove_dependency_from_list(SlurmSettings slurm);
}


#endif