#include "ambermachine.h"

namespace ambermachine
{
    void mmpbsa(JobSettings settings,SlurmSettings slurm, std::string trajectory)
    {
        // Update SLURM jobname
        std::stringstream buffer;
        buffer.str("");
        buffer << "MMPBSA_for_" << trajectory;
        slurm::update_job_name(buffer.str());

        // copy necessary files into /tmp/
        std::experimental::filesystem::copy(settings.PRMTOP, "/tmp/solvated.prmtop");
        std::experimental::filesystem::copy(trajectory, "/tmp/trajectory.mdcrd");
        std::experimental::filesystem::copy("MMPBSA_Inputs/mmpbsa.in", "/tmp/mmpbsa.in");
        std::experimental::filesystem::copy("MMPBSA_Inputs/complex.prmtop", "/tmp/complex.prmtop");
        std::experimental::filesystem::copy("MMPBSA_Inputs/receptor.prmtop", "/tmp/receptor.prmtop");
        std::experimental::filesystem::copy("MMPBSA_Inputs/ligand.prmtop", "/tmp/ligand.prmtop");

        // move to /tmp/
        std::experimental::filesystem::current_path("/tmp/");

        // run mmpbsa.py
        buffer.str("");
        buffer << "module load " << slurm.SLURM_amber_module << "; ";
        buffer << "MMPBSA.py -O -i mmpbsa.in -o FINAL_RESULTS_MMPBSA.dat -sp solvated.prmtop -cp complex.prmtop -rp receptor.prmtop -lp ligand.prmtop -y trajectory.mdcrd";
        utils::silent_shell(buffer.str().c_str());

        // copy results back to appropriate filenames
        std::string mmpbsa_result = std::experimental::filesystem::path(trajectory).parent_path() / std::experimental::filesystem::path(trajectory).stem();
        mmpbsa_result += ".mmpbsa.dat";
        std::experimental::filesystem::copy("FINAL_RESULTS_MMPBSA.dat",mmpbsa_result);

        // append results to CSV
        std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
        csv_file += "/MMPBSA_data.csv";
        double prodstep = stod(utils::GetSysResponse("echo $TRAJECTORY | cut -d. -f2"));
        double delta_avg = stod(utils::GetSysResponse("grep 'DELTA TOTAL' FINAL_RESULTS_MMPBSA.dat | awk '{print $3}'"));
        double delta_std = stod(utils::GetSysResponse("grep 'DELTA TOTAL' FINAL_RESULTS_MMPBSA.dat | awk '{print $4}'"));

        if (!utils::CheckFileExists(csv_file))
        {
            buffer.str("");
            buffer << "ProdStep, Delta_Avg, Delta_StDev" << std::endl;
            utils::write_to_file(csv_file,buffer.str());
        }
        buffer.str("");
        buffer << prodstep << ", " << delta_avg << ", " << delta_std << std::endl;

        slurm::remove_dependency_from_list(slurm);
        // remove MMPBSA_*.*
        buffer.str("");
        buffer << "rm " << std::getenv("SLURM_SUBMIT_DIR") << "/MMPBSA_*"<< slurm.SLURM_JOB_ID << ".out " << std::getenv("SLURM_SUBMIT_DIR") << "/MMPBSA_*" << slurm.SLURM_JOB_ID << ".err";
        utils::silent_shell(buffer.str().c_str());
        slurm::cleanup_out_err(slurm);
        return;
    }
}