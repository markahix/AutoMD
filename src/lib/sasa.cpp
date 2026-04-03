#include "ambermachine.h"

void write_sasa_cpptraj_input(JobSettings settings)
{
    std::stringstream buffer;
    buffer.str("");
    buffer << "parm solvated.prmtop" << std::endl;
    buffer << "trajin trajectory.mdcrd" << std::endl;
    buffer << "molsurf Receptor_SASA " << settings.RECEPTOR_MASK << " out molsurf.dat" <<std::endl;
    buffer << "molsurf Ligand_SASA " << settings.LIGAND_MASK << " out molsurf.dat" <<std::endl;
    buffer << "molsurf Complex_SASA " << settings.COMPLEX_MASK << " out molsurf.dat" <<std::endl;
    buffer << "run" << std::endl;
    buffer << "quit" << std::endl;
    utils::write_to_file("cpptraj.in",buffer.str());
}

namespace ambermachine
{
    void sasa(JobSettings settings, SlurmSettings slurm, std::string trajectory)
    {
        // Update SLURM jobname
        std::stringstream buffer;
        buffer.str("");
        buffer << "SASA_for_" << fs::absolute(trajectory).filename();
        slurm::update_job_name(buffer.str());

        // copy necessary files into /tmp/
        std::experimental::filesystem::copy(settings.PRMTOP, "/tmp/solvated.prmtop");
        std::experimental::filesystem::copy(trajectory, "/tmp/trajectory.mdcrd");
        
        // move to /tmp/
        std::experimental::filesystem::current_path("/tmp/");

        // write cpptraj in file.
        write_sasa_cpptraj_input(settings);

        // run cpptraj
        utils::silent_shell((const char*)"cpptraj < cpptraj.in > cpptraj.out");

        // append data to main SASA.dat file.
        std::string sasa_dat = std::getenv("SLURM_SUBMIT_DIR");
        sasa_dat += "/SASA.dat";
        if (!fs::exists(sasa_dat))
        {
            buffer.str("");
            buffer << std::setw(14) << "TimeIndex";
            buffer << std::setw(14) << "Receptor";
            buffer << std::setw(14) << "Ligand";
            buffer << std::setw(14) << "Complex";
            buffer << std::endl;

            utils::write_to_file(sasa_dat,buffer.str());
        }
        
        double basetimevalue = stoi(trajectory.substr(5,4));
        std::string line;
        std::ifstream ifile("molsurf.dat",std::ios::in);
        getline(ifile,line);
        getline(ifile,line);
        double time, receptor, ligand, complex;
        while (getline(ifile,line))
        {
            std::stringstream dummy;
            dummy.str(line);
            dummy >> time >> receptor >> ligand >> complex;
            time = time/settings.FRAMES_PER_NS;
            time +=(basetimevalue - 1);
            buffer << std::setw(14) << time;
            buffer << std::setw(14) << receptor;
            buffer << std::setw(14) << ligand;
            buffer << std::setw(14) << complex;
            buffer << std::endl;
        }
        
        utils::append_to_file(sasa_dat, buffer.str());
        // buffer.str("");
        // buffer << "tail -n +2 molsurf.dat >> " << sasa_dat;
        // utils::silent_shell(buffer.str().c_str());

        slurm::remove_dependency_from_list(slurm);
        // remove SASA_*.*
        buffer.str("");
        buffer << "rm " << std::getenv("SLURM_SUBMIT_DIR") << "/SASA_*"<< slurm.SLURM_JOB_ID << ".out " << std::getenv("SLURM_SUBMIT_DIR") << "/SASA_*" << slurm.SLURM_JOB_ID << ".err";
        utils::silent_shell(buffer.str().c_str());
    }
}
