#include "ambermachine.h"

namespace ambermachine
{
    void write_amberinput()
    {
        std::string input_file= R"(
#######################
##### Input Files #####
#######################
prmtop             file.prmtop
inpcrd             file.rst7

#######################
##### Job Settings ####
#######################
restraint          500
n_cold_steps       20
n_hot_steps        20
n_prod_steps       100
compress_stages    True # Compress trajectory fragments for each stage into one file.

#######################
### MMPBSA Settings ###
#######################
run_mmpbsa         True  #Delete this line if you don't want MMPBSA
receptor_mask      :1-99
ligand_mask        :100
complex_mask       :1-100
salt_concentration 0.00

#######################
## Analysis Settings ##
#######################
rmsd_mask          :1-100
rmsf_mask          :1-100@CA
hbonds_mask        :1-100
normal_modes       :1-100@CA
correl_mask        :1-100
solvent_mask       :WAT
counterions_mask   :K+,Cl-

#######################
#### Slurm Settings ###
#######################
slurm_partition    earwp
slurm_queue        express
slurm_gpu          gpu:nvidia_a30_1g.12gb:2
slurm_nodelist     arw4,arw5  #delete if you don't care what nodes are used.
slurm_exclude_nodes arw1,arw2,arw3  #delete if you don't care what nodes are used.
slurm_amber_module Amber/20-cuda-11

%BEGIN_CPPTRAJ
#Any additional or specific CPPTRAJ commands you want should go here.
#It is assumed that they will be performed on the entire production MD trajectory
%END_CPPTRAJ
)";
        utils::write_to_file("amberinput.in",input_file);
    }

    void read_amberinput(JobSettings &job_settings, SlurmSettings &slurm)
    {
        // if we're missing the amberinput.in, create the template, inform the user, and exit immediately.
        if ( !fs::exists("amberinput.in") )
        {        
            ambermachine::write_amberinput();
            error_log("Input file (amberinput.in) not found.  Edit the generated template for your specific job.",0);
        }

        std::ifstream file("amberinput.in");
        bool cpptraj_extra_commands = false;
        if (! file.is_open())
        {
            error_log("Unable to open amberinput.in for parsing.  Terminating.",1);
        }
        std::string line;
        std::stringstream dummy;
        while (std::getline(file, line)) 
        {
            std::string setting;
            std::string value;
            // Check for comments, clear them out
            std::string::size_type pos = line.find('#');
            if (pos != std::string::npos)
            {
                dummy.str(line.substr(0,pos));
            }
            else
            {
                dummy.str(line);
            }
            // Check if line is empty, skip if so.
            dummy.seekg(0, std::ios::end);
            int size = dummy.tellg();
            dummy.seekg(0, std::ios::beg);
            if (size == 0)
            {
                continue;
            }

            // Handle the %BEGIN_CPPTRAJ and %END_CPPTRAJ section here.
            // User-provided extra CPPTRAJ commands go in here and will be included in the final analysis job.
            pos = line.find("%END_CPPTRAJ");
            if (pos != std::string::npos)
            {
                cpptraj_extra_commands = false;
                continue;
            }
            if (cpptraj_extra_commands)
            {
                job_settings.CPPTRAJ_EXTRA_COMMANDS += line;
                job_settings.CPPTRAJ_EXTRA_COMMANDS += "\n";
                continue;
            }
            pos = line.find("%BEGIN_CPPTRAJ");
            if (pos != std::string::npos)
            {
                cpptraj_extra_commands = true;
                continue;
            }


            dummy >> setting >> value;
            if (setting == "prmtop")
                job_settings.PRMTOP = value;
            else if (setting == "inpcrd")
                job_settings.INPCRD = value;
            else if (setting == "restraint")
                job_settings.MAX_RESTRAINT = stod(value);
            else if (setting == "n_cold_steps")
                job_settings.NUM_COLD_STEPS = stoi(value);
            else if (setting == "n_hot_steps")
                job_settings.NUM_HOT_STEPS = stoi(value);
            else if (setting == "n_prod_steps")
                job_settings.NUM_PROD_STEPS = stoi(value);
            else if (setting == "run_mmpbsa")
                job_settings.RUN_MMPBSA = true;
            else if (setting == "receptor_mask")
                job_settings.RECEPTOR_MASK = value;
            else if (setting == "ligand_mask")
                job_settings.LIGAND_MASK = value;
            else if (setting == "complex_mask")
                job_settings.COMPLEX_MASK = value;
            else if (setting == "salt_concentration")
                job_settings.SALT_CONC = stod(value);
            else if (setting == "rmsd_mask")
                job_settings.RMSD_MASK = value;
            else if (setting == "rmsf_mask")
                job_settings.RMSF_MASK = value;
            else if (setting == "hbonds_mask")
                job_settings.HBONDS_MASK = value;
            else if (setting == "normal_modes")
                job_settings.NORMAL_MODES_MASK = value;
            else if (setting == "correl_mask")
                job_settings.CORREL_MASK = value;
            else if (setting == "solvent_mask")
                job_settings.SOLVENT_MASK = value;
            else if (setting == "counterions_mask")
                job_settings.COUNTERIONS_MASK = value;
            else if (setting == "slurm_partition")
                slurm.SLURM_partition = value;
            else if (setting == "slurm_queue")
                slurm.SLURM_queue = value;
            else if (setting == "slurm_gpu")
                slurm.SLURM_gpu = value;
            else if (setting == "slurm_amber_module")
                slurm.SLURM_amber_module = value;
            else if (setting == "slurm_nodelist")
                slurm.SLURM_nodelist = value;
            else if (setting == "slurm_exclude_nodes")
                slurm.SLURM_exclude_nodes = value;
        }
        file.close();

        // std::cout << "DEBUG: slurm_nodelist = " << slurm.SLURM_nodelist << std::endl;
        // std::cout << "DEBUG: slurm_exclude_nodes = " << slurm.SLURM_exclude_nodes << std::endl;
        // std::cout << "DEBUG: slurm_amber_module = " << slurm.SLURM_amber_module << std::endl;

        return;
    }

    void identify_current_state(JobSettings settings, SlurmSettings slurm)
    {
        /* When program is called without CLI flags, this section identifies what stage the job should be on and self-submits that stage as appropriate.*/
        std::stringstream sys_command;
        sys_command.str("");

        if ( !utils::CheckFileExists(".AMBER_INITIALIZE_COMPLETE") ) // amber_initialize.sh
        {
            std::cout << "Submitting AmberMachine Initialization." << std::endl;
            slurm::submit_initialize_job(settings,slurm);
            return;
        }
        if ( !utils::CheckFileExists(".AMBER_MINIMIZE_COMPLETE") ) // amber_minimize.sh
        {
            std::cout << "Submitting AmberMachine Minimization." << std::endl;
            slurm::submit_minimize_job(settings,slurm);
            return;
        }
        if ( !utils::CheckFileExists(".AMBER_COLD_RELAX_COMPLETE") ) // amber_cold_relax.sh
        {
            std::cout << "Submitting AmberMachine Cold Relaxation." << std::endl;
            slurm::submit_cold_equil_job(settings,slurm);
            return;
        }
        if ( !utils::CheckFileExists(".AMBER_HEATING_COMPLETE") ) // amber_heating.sh
        {
            std::cout << "Submitting AmberMachine Heating." << std::endl;
            slurm::submit_heating_job(settings,slurm);
            return;
        }
        if ( !utils::CheckFileExists(".AMBER_HOT_RELAX_COMPLETE") ) // amber_hot_relax.sh
        {
            std::cout << "Submitting AmberMachine Hot Relaxation." << std::endl;
            slurm::submit_hot_equil_job(settings,slurm);
            return;
        }
        if ( !utils::CheckFileExists(".AMBER_PRODUCTION_COMPLETE") ) // amber_production.sh
        {
            std::cout << "Submitting AmberMachine Production." << std::endl;
            slurm::submit_production_job(settings,slurm);
            return;
        }
        if ( !utils::CheckFileExists(".AMBER_ANALYSIS_COMPLETE") ) // amber_cpptraj.sh
        {
            std::cout << "Submitting AmberMachine Analysis." << std::endl;
            slurm::submit_analysis_job(settings,slurm);
            return;
        }
        if ( !utils::CheckFileExists(".AMBER_REPORT_COMPLETE"))
        {
            std::cout << "Submitting AmberMachine Reporting." << std::endl;
            slurm::submit_reporting_job(settings,slurm);
            return;
        }
        std::cout << "The Amber Machine ("<< slurm.SLURM_executable << ") reports that all stages of classical MD are complete according to the original input file.  Please check for the completed report PDF in this directory." << std::endl;
        std::cout << std::endl;
        std::cout << "If you wish to extend the simulation, edit the amberinput.in accordingly and delete the hidden file <.AMBER_PRODUCTION_COMPLETE>" << std::endl;
        return;
    }

    void AmberLoop(SlurmSettings slurm)
    {
        std::string curr_path = fs::current_path();
        fs::current_path("/tmp/");

        std::stringstream buffer;
        buffer.str("");
        buffer << "module load " << slurm.SLURM_amber_module << "; $AMBERHOME/bin/pmemd.cuda -O";
        buffer << " -i mdin.in";
        buffer << " -o mdout.out";
        buffer << " -p job.prmtop";
        buffer << " -c last_step.rst7";
        buffer << " -r current_step.rst7";
        buffer << " -x trajectory.mdcrd";
        buffer << " -ref last_step.rst7";
        utils::silent_shell(buffer.str().c_str());
        fs::current_path(curr_path);
    }

    void AmberCopyBack(std::string mdin_file,std::string restart_file, std::string mdout_file, std::string trajectory_file, std::string csv_file)
    {
        std::string curr_path = fs::current_path();
        fs::current_path("/tmp/");
        // copy back from /tmp
        fs::copy("mdin.in",mdin_file,fs::copy_options::update_existing);
        fs::remove("mdin.in");
        fs::copy("current_step.rst7",restart_file);
        fs::copy("current_step.rst7",(std::string)std::getenv("SLURM_SUBMIT_DIR")+"/current_step.rst7",fs::copy_options::update_existing);
        fs::copy("current_step.rst7","last_step.rst7",fs::copy_options::update_existing);
        fs::remove("current_step.rst7");
        fs::copy("mdout.out",mdout_file);
        fs::remove("mdout.out");
        fs::copy("trajectory.mdcrd",trajectory_file);
        fs::remove("trajectory.mdcrd");

        // Parse mdout into HotEquil.csv
        utils::mdout_to_csv(mdout_file,csv_file);
        fs::current_path(curr_path);

}
}
