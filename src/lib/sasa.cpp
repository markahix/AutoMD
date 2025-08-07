#include "ambermachine.h"

namespace ambermachine
{
    void sasa(JobSettings settings,SlurmSettings slurm, std::string trajectory)
    {
        // Update SLURM jobname
        std::stringstream buffer;
        buffer.str("");
        buffer << "SASA_for_" << trajectory;
        slurm::update_job_name(buffer.str());

        // copy necessary files into /tmp/
        std::experimental::filesystem::copy(settings.PRMTOP, "/tmp/solvated.prmtop");
        std::experimental::filesystem::copy(trajectory, "/tmp/trajectory.mdcrd");
        
        // move to /tmp/
        std::experimental::filesystem::current_path("/tmp/");

        // write cpptraj in file.
        buffer.str("");
        buffer << "parm solvated.prmtop" << std::endl;
        buffer << "trajin trajectory.mdcrd" << std::endl;
        buffer << "molsurf Receptor_SASA " << settings.RECEPTOR_MASK << " out molsurf.dat" <<std::endl;
        buffer << "molsurf Ligand_SASA " << settings.LIGAND_MASK << " out molsurf.dat" <<std::endl;
        buffer << "molsurf Complex_SASA " << settings.COMPLEX_MASK << " out molsurf.dat" <<std::endl;
        buffer << "run" << std::endl;
        buffer << "quit" << std::endl;
        utils::write_to_file("cpptraj.in",buffer.str());

        // run cpptraj
        utils::silent_shell((const char*)"cpptraj < cpptraj.in > cpptraj.out");

        // append data to main SASA.dat file.
        std::string sasa_dat = std::getenv("SLURM_SUBMIT_DIR");
        sasa_dat += "/SASA.dat";
        if (!utils::CheckFileExists(sasa_dat))
        {
            utils::write_to_file(sasa_dat,"#Frame    Receptor    Ligand  Complex\n");
        }
        buffer.str("");
        buffer << "tail -n +2 molsurf.dat >> " << sasa_dat;
        utils::silent_shell(buffer.str().c_str());

        slurm::remove_dependency_from_list(slurm);
        // remove SASA_*.*
        buffer.str("");
        buffer << "rm " << std::getenv("SLURM_SUBMIT_DIR") << "/SASA_*"<< slurm.SLURM_JOB_ID << ".out " << std::getenv("SLURM_SUBMIT_DIR") << "/SASA_*" << slurm.SLURM_JOB_ID << ".err";
        utils::silent_shell(buffer.str().c_str());
    }
}
