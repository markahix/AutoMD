#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <experimental/filesystem>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <iomanip>
#include "utilities.h"
#include "slurm.h"

namespace slurm
{
    void update_job_name(std::string jobname)
    {
        std::string SLURM_JOB_ID=std::getenv("SLURM_JOB_ID");
        std::stringstream buffer;
        buffer.str("");
        buffer << "scontrol update JobID=" << SLURM_JOB_ID << " jobname=\"" << jobname <<"\"";
        utils::silent_shell(buffer.str().c_str());
    }

    void submit_initialize_job(JobSettings settings,SlurmSettings slurm)
    {
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AmberMachineInitialize -p " << slurm.SLURM_partition;
        sys_command << " -q "<< slurm.SLURM_queue;
        sys_command << " -t 1:00:00 -N 1 -n 1";
        sys_command << " --gres=" << slurm.SLURM_gpu;
        if (slurm.SLURM_nodelist != " ")
            sys_command << " --nodelist=" << slurm.SLURM_nodelist;
        else if (slurm.SLURM_exclude_nodes != " ")
            sys_command << " --exclude=" << slurm.SLURM_exclude_nodes;
        sys_command << " --mem=20GB";
        sys_command << " -o AMBER_%j.out -e AMBER_%j.err --wrap \"" << slurm.SLURM_executable << " --initialize\"";
        std::cout << sys_command.str() << std::endl;
        utils::silent_shell(sys_command.str().c_str());
    }

    void submit_minimize_job(JobSettings settings, SlurmSettings slurm)
    {
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AmberMachineMinimize -p " << slurm.SLURM_partition;
        sys_command << " -q "<< slurm.SLURM_queue;
        sys_command << " -t 24:00:00 -N 1 -n 1";
        sys_command << " --gres=" << slurm.SLURM_gpu;
        if (slurm.SLURM_nodelist != " ")
            sys_command << " --nodelist=" << slurm.SLURM_nodelist;
        else if (slurm.SLURM_exclude_nodes != " ")
            sys_command << " --exclude=" << slurm.SLURM_exclude_nodes;
        sys_command << " --mem=20GB";
        sys_command << " -o AMBER_%j.out -e AMBER_%j.err --wrap \"" << slurm.SLURM_executable << " --minimize\"";
        std::cout << sys_command.str() << std::endl;
        utils::silent_shell(sys_command.str().c_str());
    }

    void submit_cold_equil_job(JobSettings settings,SlurmSettings slurm)
    {
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AmberMachineColdEquil -p " << slurm.SLURM_partition;
        sys_command << " -q "<< slurm.SLURM_queue;
        sys_command << " -t 24:00:00 -N 1 -n 1";
        sys_command << " --gres=" << slurm.SLURM_gpu;
        if (slurm.SLURM_nodelist != " ")
            sys_command << " --nodelist=" << slurm.SLURM_nodelist;
        else if (slurm.SLURM_exclude_nodes != " ")
            sys_command << " --exclude=" << slurm.SLURM_exclude_nodes;
        sys_command << " --mem=20GB";
        sys_command << " -o AMBER_%j.out -e AMBER_%j.err --wrap \"" << slurm.SLURM_executable << " --coldrelax\"";
        utils::silent_shell(sys_command.str().c_str());

    }

    void submit_heating_job(JobSettings settings,SlurmSettings slurm)
    {
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AmberMachineHeating -p " << slurm.SLURM_partition;
        sys_command << " -q "<< slurm.SLURM_queue;
        sys_command << " -t 24:00:00 -N 1 -n 1";
        sys_command << " --gres=" << slurm.SLURM_gpu;
        if (slurm.SLURM_nodelist != " ")
            sys_command << " --nodelist=" << slurm.SLURM_nodelist;
        else if (slurm.SLURM_exclude_nodes != " ")
            sys_command << " --exclude=" << slurm.SLURM_exclude_nodes;
        sys_command << " --mem=20GB";
        sys_command << " -o AMBER_%j.out -e AMBER_%j.err --wrap \"" << slurm.SLURM_executable << " --heating\"";
        utils::silent_shell(sys_command.str().c_str());
    }

    void submit_hot_equil_job(JobSettings settings,SlurmSettings slurm)
    {
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AmberMachineHotEquil -p " << slurm.SLURM_partition;
        sys_command << " -q "<< slurm.SLURM_queue;
        sys_command << " -t 24:00:00 -N 1 -n 1";
        sys_command << " --gres=" << slurm.SLURM_gpu;
        if (slurm.SLURM_nodelist != " ")
            sys_command << " --nodelist=" << slurm.SLURM_nodelist;
        else if (slurm.SLURM_exclude_nodes != " ")
            sys_command << " --exclude=" << slurm.SLURM_exclude_nodes;
        sys_command << " --mem=20GB";
        sys_command << " -o AMBER_%j.out -e AMBER_%j.err --wrap \"" << slurm.SLURM_executable << " --hotrelax\"";
        utils::silent_shell(sys_command.str().c_str());
    }

    void submit_production_job(JobSettings settings,SlurmSettings slurm)
    {
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AmberMachineProduction -p " << slurm.SLURM_partition;
        sys_command << " -q "<< slurm.SLURM_queue;
        sys_command << " -t 5-0:00:00 -N 1 -n 1";
        sys_command << " --gres=" << slurm.SLURM_gpu;
        if (slurm.SLURM_nodelist != " ")
            sys_command << " --nodelist=" << slurm.SLURM_nodelist;
        else if (slurm.SLURM_exclude_nodes != " ")
            sys_command << " --exclude=" << slurm.SLURM_exclude_nodes;
        sys_command << " --mem=20GB";
        sys_command << " -o AMBER_%j.out -e AMBER_%j.err --wrap \"" << slurm.SLURM_executable << " --production\"";
        utils::silent_shell(sys_command.str().c_str());
    }

    void submit_mmpbsa_job(JobSettings settings, SlurmSettings slurm, std::string trajectory)
    {
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AutoMMPBSA";
        sys_command << " -q primary";
        sys_command << " -t 5-0:00:00 -N 1 -n 20";
        sys_command << " --mem=20GB";
        sys_command << " -o MMPBSA_%j.out -e MMPBSA_%j.err --wrap \"autommpbsa -s" << settings.PRMTOP << " -c MMPBSA_Inputs/complex.prmtop -r MMPBSA_Inputs/receptor.prmtop -l MMPBSA_Inputs/ligand.prmtop -m MMPBSA_Inputs/mmpbsa.in -t " << trajectory << "\"";
        std::string jobid = utils::GetSysResponse(sys_command.str().c_str());
        std::stringstream line;
        line.str(jobid);
        std::string dummy;
        line >> dummy >> dummy >> dummy >> dummy; // Submitted batch job 123456789
        std::stringstream buffer;
        buffer.str("");
        buffer << dummy << std::endl;
        std::string job_dep_file = std::getenv("SLURM_SUBMIT_DIR");
        job_dep_file += "/.JOBDEPENDENCIES";
        utils::append_to_file(job_dep_file,buffer.str());
    }

    void submit_sasa_job(JobSettings settings, SlurmSettings slurm, std::string trajectory)
    {
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AmberMachineSASA";
        sys_command << " -q primary";
        sys_command << " -t 5-0:00:00 -N 1 -n 1";
        sys_command << " --mem=5GB";
        sys_command << " -o SASA_%j.out -e SASA_%j.err --wrap \"" << slurm.SLURM_executable << " --sasa " << trajectory << "\"";
        std::string jobid = utils::GetSysResponse(sys_command.str().c_str());
        std::stringstream line;
        line.str(jobid);
        std::string dummy;
        line >> dummy >> dummy >> dummy >> dummy; // Submitted batch job 123456789
        std::stringstream buffer;
        buffer.str("");
        buffer << dummy << std::endl;
        std::string job_dep_file = std::getenv("SLURM_SUBMIT_DIR");
        job_dep_file += "/.JOBDEPENDENCIES";
        utils::append_to_file(job_dep_file,buffer.str());
    }

    void submit_analysis_job(JobSettings settings,SlurmSettings slurm)
    {
        // get all dependencies in .JOBDEPENDENCIES file, join with commas, submit slurm job with dependency=afterok:<jobids>
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AmberMachineAnalysis -p " << slurm.SLURM_partition;
        sys_command << " -q "<< slurm.SLURM_queue;
        sys_command << " -t 5-0:00:00 -N 1 -n 1";
        sys_command << " --gres=" << slurm.SLURM_gpu;
        if (slurm.SLURM_nodelist != " ")
            sys_command << " --nodelist=" << slurm.SLURM_nodelist;
        else if (slurm.SLURM_exclude_nodes != " ")
            sys_command << " --exclude=" << slurm.SLURM_exclude_nodes;
        sys_command << " --mem=20GB";
        sys_command << " -o AMBER_%j.out -e AMBER_%j.err --wrap \"" << slurm.SLURM_executable << " --analysis\"";
        utils::silent_shell(sys_command.str().c_str());
    }

    void submit_reporting_job(JobSettings settings,SlurmSettings slurm)
    {
        std::stringstream sys_command;
        sys_command.str("");
        sys_command << "sbatch -J AmberMachineReporting -p " << slurm.SLURM_partition;
        sys_command << " -q "<< slurm.SLURM_queue;
        sys_command << " -t 24:00:00 -N 1 -n 1";
        sys_command << " --gres=" << slurm.SLURM_gpu;
        if (slurm.SLURM_nodelist != " ")
            sys_command << " --nodelist=" << slurm.SLURM_nodelist;
        if (slurm.SLURM_exclude_nodes != " ")
            sys_command << " --exclude=" << slurm.SLURM_exclude_nodes;
        sys_command << " --mem=20GB";
        sys_command << " -o AMBER_%j.out -e AMBER_%j.err --wrap \"" << slurm.SLURM_executable << " --report\"";
        utils::silent_shell(sys_command.str().c_str());

    }
    void delete_if_exists(std::string filename)
    {
        if (fs::exists(fs::absolute(filename)))
        {
            fs::remove(fs::absolute(filename));
        }
    }

    void cleanup_out_err(SlurmSettings slurm)
    {
        
        std::stringstream buffer;
        std::stringstream filename;
        //remove empty AMBER_*.out and *.err files.
        
        filename.str("");
        filename << std::getenv("SLURM_SUBMIT_DIR") << "/AMBER_*"<< slurm.SLURM_JOB_ID << ".out";
        delete_if_exists(filename.str());
        filename.str("");
        filename << std::getenv("SLURM_SUBMIT_DIR") << "/AMBER_*"<< slurm.SLURM_JOB_ID << ".err";
        delete_if_exists(filename.str());

        filename.str("");
        filename << std::getenv("SLURM_SUBMIT_DIR") << "/MMPBSA_*"<< slurm.SLURM_JOB_ID << ".out";
        delete_if_exists(filename.str());
        filename.str("");
        filename << std::getenv("SLURM_SUBMIT_DIR") << "/MMPBSA_*"<< slurm.SLURM_JOB_ID << ".err";
        delete_if_exists(filename.str());

        filename.str("");
        filename << std::getenv("SLURM_SUBMIT_DIR") << "/SASA_*"<< slurm.SLURM_JOB_ID << ".out";
        delete_if_exists(filename.str());
        filename.str("");
        filename << std::getenv("SLURM_SUBMIT_DIR") << "/SASA_*"<< slurm.SLURM_JOB_ID << ".err";
        delete_if_exists(filename.str());
    }

    void remove_dependency_from_list(SlurmSettings slurm)
    {
        // JOBDEPENDENCIES
        std::ifstream infile;
        std::stringstream lines;
        std::string line;
        infile.open(".JOBDEPENDENCIES");
        while (getline(infile,line))
        {
            if (line.find(slurm.SLURM_JOB_ID) == std::string::npos)
            {
                lines << line;
            }
        }
        infile.close();
        utils::write_to_file(".JOBDEPENDENCIES",lines.str());
    }

}
