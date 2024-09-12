#include "latex.h"


void latex::write_main_tex()
{
    // Get user's full name.
    std::stringstream dummy;
    dummy.str("");
    dummy << "finger " <<std::getenv("USER");
    std::string finger = utils::GetSysResponse(dummy.str().c_str());
    std::string::size_type sel_1 = finger.find("Name:")+5;
    std::string::size_type sel_2 = finger.find("Directory:");
    std::string username = finger.substr(sel_1,sel_2 - sel_1);
    
    // Create main.tex
    std::stringstream buffer;
    buffer.str("");
    buffer << "\\documentclass[letterpaper,11pt]{article}" << std::endl;
    buffer << "\\usepackage[margin=0.5in]{geometry}" << std::endl;
    buffer << "\\usepackage{graphicx}" << std::endl;
    buffer << "\\usepackage[T1]{fontenc}" << std::endl;
    buffer << "\\usepackage{minted}" << std::endl;
    buffer << "\\usepackage{tcolorbox}" << std::endl;
    buffer << "\\usepackage{etoolboox}" << std::endl;
    buffer << "\\usepackage{placeins}" << std::endl;
    buffer << "\\usepackage{longtable}" << std::endl;
    buffer << "\\title{Automatic Amber Molecular Dynamics Report}" << std::endl;
    buffer << "\\author{User: \\textbf{" << username <<"} \\\\ Wayne State University Grid}" << std::endl;
    buffer << "\\date{\\today}" << std::endl;
    buffer << "\\begin{document}" << std::endl;
    buffer << "\\maketitle" << std::endl;
    buffer << "\\clearpage" << std::endl;
    buffer << "\\input{timeline.tex}" << std::endl;
    buffer << "\\end{tabular}" << std::endl;
    buffer << "\\clearpage" << std::endl;
    buffer << "\\input{initialize.tex}" << std::endl;
    buffer << "\\clearpage" << std::endl;
    buffer << "\\input{minimization.tex}" << std::endl;
    buffer << "\\clearpage" << std::endl;
    buffer << "\\input{cold_equil.tex}" << std::endl;
    buffer << "\\clearpage" << std::endl;
    buffer << "\\input{heating.tex}" << std::endl;
    buffer << "\\clearpage" << std::endl;
    buffer << "\\input{hot_equil.tex}" << std::endl;
    buffer << "\\clearpage" << std::endl;
    buffer << "\\input{production.tex}" << std::endl;
    buffer << "\\clearpage" << std::endl;
    buffer << "\\input{analysis.tex}" << std::endl;
    buffer << "\\end{document}" << std::endl;
    utils::write_to_file("00_Report/main.tex",buffer.str());
}
void latex::write_timeline_tex()
{
    std::stringstream buffer;
    std::string date = utils::GetSysResponse("date");
    buffer.str("");
    buffer << "\\section*{Job Timeline}" << std::endl;
    buffer << "\\begin{tabular}{|p{0.3\\textwidth}|p{0.3\\textwidth}|p{0.3\\textwidth}|}" << std::endl;
    buffer << "\\hline" << std::endl;
    buffer << "Stage & Date/Time & Job ID Number \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    buffer << "Initialization & \\texttt{" << date << "} & \\textbf{";
    buffer << std::getenv("SLURM_JOB_ID") << "} \\\\ " << std::endl;
    buffer << "\\hline" << std::endl;
    utils::write_to_file("00_Report/timeline.tex",buffer.str());
}
void latex::write_initialize_tex(JobSettings settings)
{
    std::stringstream buffer;
// write initialize.tex
    buffer.str("");
    buffer << "\\section*{Initialization}" << std::endl;
    buffer << "\\subsection*{Job Settings}" << std::endl;
    buffer << "\\noindent\\begin{tabular}{|p{0.2\\textwidth}|p{0.7\\textwidth}|}" << std::endl;
    buffer << "\\hline" << std::endl;
    buffer << "Input Topology: & \\verb|" << settings.PRMTOP << "| \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    buffer << "Input Coordinates: & \\verb|" << settings.INPCRD << "| \\\\" << std::endl;
    buffer << "\\hline" << std::endl;
    buffer << "\\end{tabular}" << std::endl;
    buffer << "" << std::endl;
    utils::write_to_file("00_Report/initialize.tex",buffer.str());
}
void latex::write_minimization_tex()
{
    std::stringstream buffer;
    buffer << "\\section*{System Minimization}" << std::endl;
    buffer << "\\paragraph{} Minimization removes potential clashes from starting structures (such as from the addition of hydrogens to a crystal structure)." << std::endl;
    buffer << "" << std::endl;
    utils::write_to_file("00_Report/minimization.tex",buffer.str());
}
void latex::write_cold_equil_tex(JobSettings settings)
{
    std::stringstream buffer;
    buffer << "\\section*{Cold Density Equilibration}" << std::endl;
    buffer << "\\paragraph{}Cold density equilibration at 10K, holding pressure constant (NPT ensemble)." << std::endl;
    buffer << "Restraints are iteratively relaxed (linear descent over " << settings.NUM_COLD_STEPS << " steps) on " << settings.COMPLEX_MASK;
    buffer << ",beginning at " << settings.MAX_RESTRAINT << " $kcal\\cdot mol^{-1}$." << std::endl;
    buffer << "" << std::endl;
    utils::write_to_file("00_Report/cold_equil.tex",buffer.str());

}
void latex::write_heating_tex()
{
    std::stringstream buffer;
    buffer << "\\section*{System Heating}" << std::endl;
    buffer << "\\paragraph{}Heating through iterative steps at 10K, 20K, 50K, 100K, 150K, 200K, 250K, 300K, 325K, and 300K." << std::endl;
    buffer << "Each stage runs for 10000 timesteps (0.01 ns) for a total of 0.1ns of simulated heating to final temperature of 300K." << std::endl;
    buffer << "" << std::endl;
    utils::write_to_file("00_Report/heating.tex",buffer.str());

}
void latex::write_hot_equil_tex(JobSettings settings)
{
    std::stringstream buffer;
    buffer << "\\section*{Hot Density Equilibration}" << std::endl;
    buffer << "\\paragraph{}Hot density equilibration at 300K, holding pressure constant (NPT ensemble)." << std::endl;
    buffer << "Restraints are iteratively relaxed (linear descent over " << settings.NUM_HOT_STEPS << " steps) on " << settings.COMPLEX_MASK;
    buffer << ",beginning at " << settings.MAX_RESTRAINT << " $kcal\\cdot mol^{-1}$." << std::endl;
    buffer << "" << std::endl;
    utils::write_to_file("00_Report/hot_equil.tex",buffer.str());

}
void latex::write_production_tex(JobSettings settings)
{
    std::stringstream buffer;
    buffer << "\\section*{Production Molecular Dynamics}" << std::endl;
    buffer << "\\paragraph{}Production molecular dynamics will be run with a 1fs timestep for "<<settings.NUM_PROD_STEPS<< "ns with periodic boundary conditions at constant volume (NVT ensemble)." << std::endl;
    buffer << "" << std::endl;
    if (settings.RUN_MMPBSA)
    {
        buffer << "\\subsection*{MMPBSA Calculations}" << std::endl;
        buffer << "\\paragraph{} MMPBSA calculations will be performed alongside production.  These calculations are intended to calculate the binding energy for a given complex." << std::endl;
        buffer << "" << std::endl;
        buffer << "\\noindent \\begin{tabular}{|p{0.2\\textwidth}|p{0.7\\textwidth}|}" << std::endl;
        buffer << "\\hline" << std::endl;
        buffer << "Receptor mask:    & " << settings.RECEPTOR_MASK << " \\\\" << std::endl;
        buffer << "\\hline" << std::endl;
        buffer << "Ligand mask:      & " << settings.LIGAND_MASK << " \\\\" << std::endl;
        buffer << "\\hline" << std::endl;
        buffer << "Complex mask:     & " << settings.COMPLEX_MASK << " \\\\" << std::endl;
        buffer << "\\hline" << std::endl;
        buffer << "\\end{tabular}" << std::endl;
        buffer << "" << std::endl;
    }
    utils::write_to_file("00_Report/production.tex",buffer.str());

}

void latex::write_analysis_tex(JobSettings settings)
{
    std::stringstream buffer;
    buffer << "\\section*{Analysis}" << std::endl;
    buffer << "\\subsection*{Base Analyses}" << std::endl;
    buffer << "\\paragraph{} Basic analyses will be performed upon completion of production MD." << std::endl;
    buffer << "\\begin{itemize}" << std::endl;
    buffer << "\\item \\textbf{RMSD} - Root-mean-squared deviation is a measure of how far from a given reference structure (usually starting coordinates) a molecular dynamics system has changed.  Useful for monitoring conformational changes and system stability." << std::endl;
    buffer << "\\item \\textbf{RMSF} - The magnitude of fluctuation of amino acids in a protein.  High RMSF indicates a highly mobile amino acid." << std::endl;
    buffer << "\\item \\textbf{Hydrogen Bond Analysis} - Calculation of hydrogen bond interactions (HBI), including fraction of total simulation time for any given HBI." << std::endl;
    buffer << "\\item \\textbf{Normal Modes} - Top 100 vibrational modes of motion for the system.  Useful to identify the largest contributions to the overall dynamic motion of the protein." << std::endl;
    buffer << "\\item \\textbf{Correlated Motion} - A measure of how each amino acid moves with respect to other amino acids.  AAs which move together are correlated, those which move in direct opposition are anti-correlated, and those which move orthogonally to each other are non-correlated." << std::endl;
    if (settings.LIGAND_MASK != " " && settings.RECEPTOR_MASK != " ")
    {
    buffer << "\\item \\textbf{Ligand Interaction Energy} - Coulomb and van der Waals interaction energies between a ligand and receptor over time.  Lower (more negative) values indicate more favorable interactions between the ligand and receptor, and can suggest good binding conformations or receptor geometry changes.  Ligands with nonzero charges or highly hydrophilic ligands will generally be dominated by coulomb interactions, while neutral or hydrophobic ligands will often be dominated by van der Waals interactions." <<std::endl;
    }
    buffer << "\\end{itemize}" << std::endl;
    buffer << "\\noindent\\begin{tabular}{|p{0.2\\textwidth}|p{0.7\\textwidth}|}" << std::endl;
    // Add individual analysis masks
    if (settings.RMSD_MASK != " ")
    {
        buffer << "\\hline" << std::endl;
        buffer << "RMSD mask:     & "<< settings.RMSD_MASK <<" \\\\" << std::endl;
    }
    if (settings.RMSF_MASK != " ")
    {
        buffer << "\\hline" << std::endl;
        buffer << "RMSF mask:     & "<< settings.RMSF_MASK <<" \\\\" << std::endl;
    }
    if (settings.HBONDS_MASK != " ")
    {
        buffer << "\\hline" << std::endl;
        buffer << "Hydrogen bonds & "<< settings.HBONDS_MASK <<" \\\\" << std::endl;
    }
    if (settings.NORMAL_MODES_MASK != " ")
    {
        buffer << "\\hline" << std::endl;
        buffer << "Normal modes:  & "<< settings.NORMAL_MODES_MASK <<" \\\\" << std::endl;
    }
    if (settings.CORREL_MASK != " ")
    {
        buffer << "\\hline" << std::endl;
        buffer << "Correlation:   & "<< settings.CORREL_MASK <<" \\\\" << std::endl;
    }
    if (settings.SOLVENT_MASK != " ")
    {
        buffer << "\\hline" << std::endl;
        buffer << "Solvent:       & "<< settings.SOLVENT_MASK <<" \\\\" << std::endl;
    }
    if (settings.COUNTERIONS_MASK != " ")
    {
        buffer << "\\hline" << std::endl;
        buffer << "Counterions:   & "<< settings.COUNTERIONS_MASK <<" \\\\" << std::endl;
    }
    if (settings.LIGAND_MASK != " " && settings.RECEPTOR_MASK != " ")
    {
        buffer << "\\hline" << std::endl;
        buffer << "Receptor Mask: & "<< settings.RECEPTOR_MASK << " \\\\" << std::endl;
        buffer << "Ligand Mask: &" << settings.LIGAND_MASK << " \\\\" << std::endl;
    }
    buffer << "\\hline" << std::endl;
    buffer << "\\end{tabular}" << std::endl;
    buffer << "" << std::endl;
    // create individual analysis files, add as inputs to main analysis.tex
    if (settings.RMSD_MASK != " ")
    {
        utils::write_to_file("00_Report/analysis_RMSD.tex","");
        buffer << "\\input{analysis_RMSD.tex}" << std::endl;
    }
    if (settings.RMSF_MASK != " ")
    {
        utils::write_to_file("00_Report/analysis_RMSF.tex","");
        buffer << "\\input{analysis_RMSF.tex}" << std::endl;
    }
    if (settings.HBONDS_MASK != " ")
    {
        utils::write_to_file("00_Report/analysis_HBONDS.tex","");
        buffer << "\\input{analysis_HBONDS.tex}" << std::endl;
    }
    if (settings.NORMAL_MODES_MASK != " ")
    {
        utils::write_to_file("00_Report/analysis_NORMAL_MODES.tex","");
        buffer << "\\input{analysis_NORMAL_MODES.tex}" << std::endl;
    }
    if (settings.CORREL_MASK != " ")
    {
        utils::write_to_file("00_Report/analysis_CORREL.tex","");
        buffer << "\\input{analysis_CORREL.tex}" << std::endl;
    }
    if (settings.LIGAND_MASK != " " && settings.RECEPTOR_MASK != " ")
    {
        utils::write_to_file("00_Report/analysis_LIE.tex","");
        buffer << "\\input{analysis_LIE.tex}" << std::endl;
        buffer << "\\input{analysis_SASA.tex}" << std::endl;

    }

    buffer << std::endl << "\\subsection{Final Analysis}" << std::endl;
    utils::write_to_file("00_Report/analysis.tex",buffer.str());

}

void latex::initialize_report(JobSettings settings)
{
    // Create report directory
    std::experimental::filesystem::create_directory("00_Report/");
    write_main_tex();
    write_timeline_tex();
    write_initialize_tex(settings);
    write_minimization_tex();
    write_cold_equil_tex(settings);
    write_heating_tex();
    write_hot_equil_tex(settings);
    write_production_tex(settings);
    write_analysis_tex(settings);
    compile_report(settings);

}

void latex::compile_report(JobSettings settings)
{
    std::string curr_path = std::experimental::filesystem::current_path();
    std::experimental::filesystem::current_path("00_Report/");
    std::stringstream buffer;
    buffer.str("");
    buffer << "pdflatex main.tex -interaction=nonstopmode --shell-escape > /dev/null";
    utils::silent_shell(buffer.str().c_str());
    buffer.str("");
    buffer << "mv main.pdf ../" << settings.PRMTOP.substr(0,settings.PRMTOP.find(".prmtop")) << "_Report.pdf";
    utils::silent_shell(buffer.str().c_str());
    std::experimental::filesystem::current_path(curr_path);
}

void latex::figure_block_to_file(std::string imagebase, std::string caption, std::string filename)
{
    std::stringstream latex;
    latex << "\\begin{figure}[htbp] " << std::endl;
    latex << "\\centering" << std::endl;
    latex << "\\includegraphics[width=0.9\textwidth]{" << imagebase << ".png}" << std::endl;
    latex << "\\caption{ " << caption << "}" << std::endl;
    latex << "\\label{fig:" << imagebase << "}" << std::endl;
    latex << "\\end{figure}" << std::endl;
    latex << "" << std::endl;
    latex << "" << std::endl;

    utils::append_to_file(filename,latex.str());
    return;
}

void latex::python_block_to_file(std::string python, std::string filename)
{
    std::stringstream latex;
    latex << "\\begin{tcolorbox}[title=Code for Above Plot,colframe=green!50!black]" << std::endl;
    latex << "\\begin{minted}{python}" << std::endl;
    latex << python << std::endl;
    latex << "\\end{minted}" << std::endl;
    latex << "\\end{tcolorbox}" << std::endl;
    latex << "\\FloatBarrier" << std::endl;

    utils::append_to_file(filename,latex.str());
    return;
}