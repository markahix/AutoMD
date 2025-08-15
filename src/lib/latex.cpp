#include "latex.h"

std::string GetUserFullName()
{
    std::stringstream dummy;
    dummy.str("");
    dummy << "finger " <<std::getenv("USER");
    std::string finger = utils::GetSysResponse(dummy.str().c_str());
    std::string::size_type sel_1 = finger.find("Name:")+5;
    std::string::size_type sel_2 = finger.find("Directory:");
    std::string username = finger.substr(sel_1,sel_2 - sel_1);
    return username;
}
std::string ligand_analysis_text(JobSettings settings)
{
    if (settings.LIGAND_MASK != " " && settings.RECEPTOR_MASK != " ")
    {
        return R"(\item \textbf{Ligand Interaction Energy} - Coulomb and van der Waals interaction energies between a ligand and receptor over time.  Lower (more negative) values indicate more favorable interactions between the ligand and receptor, and can suggest good binding conformations or receptor geometry changes.  Ligands with nonzero charges or highly hydrophilic ligands will generally be dominated by coulomb interactions, while neutral or hydrophobic ligands will often be dominated by van der Waals interactions.)";
    }
    return " ";
}
void latex::write_main_tex()
{
    // Get user's full name.
    std::string username = GetUserFullName();

    // Create main.tex
    std::string preamble = R"(
\documentclass[letterpaper,11pt]{article}
\usepackage[margin=0.5in]{geometry}
\usepackage{graphicx}
\usepackage[T1]{fontenc}
\usepackage{minted}
\usepackage{tcolorbox}
\usepackage{etoolbox}
\usepackage{placeins}
\usepackage{longtable}
\title{Automatic Amber Molecular Dynamics Report}
\author{User: \textbf{)" + username + R"(} \\ Wayne State University Grid}
\date{\today}
\begin{document}
\maketitle
\clearpage
\input{timeline.tex}
\end{tabular}
\clearpage
\input{initialize.tex}
\clearpage
\input{minimization.tex}
\clearpage
\input{cold_equil.tex}
\clearpage
\input{heating.tex}
\clearpage
\input{hot_equil.tex}
\clearpage
\input{production.tex}
\clearpage
\input{analysis.tex}
\end{document}
)";
    utils::write_to_file("00_Report/main.tex",preamble);
}
void latex::write_timeline_tex()
{
    std::string rawtext = R"(
\section*{Job Timeline}
\begin{tabular}{|p{0.3\textwidth}|p{0.3\textwidth}|p{0.3\textwidth}|}
\hline
Stage & Date/Time & Job ID Number \\
\hline
Initialization & \texttt{)" + utils::GetSysResponse("date") + R"(} & \textbf{)" +  std::getenv("SLURM_JOB_ID") + R"( << "} \\
\hline" << std::endl;
)";
    utils::write_to_file("00_Report/timeline.tex",rawtext);
}
void latex::write_initialize_tex(JobSettings settings)
{
    std::string rawtext = R"(
\section*{Initialization}
\subsection*{Job Settings}
\noindent\begin{tabular}{|p{0.2\textwidth}|p{0.7\textwidth}|}
\hline
Input Topology: & \verb|)" + settings.PRMTOP + R"(\\
\hline
Input Coordinates: & \verb|)" + settings.INPCRD + R"(\\
\hline
\end{tabular}

)";
    // write initialize.tex
    utils::write_to_file("00_Report/initialize.tex",rawtext);
}
void latex::write_minimization_tex()
{
    std::string rawtext=R"(
\section*{System Minimization}
\paragraph{} Minimization removes potential clashes from starting structures (such as from the addition of hydrogens to a crystal structure).

)";
    utils::write_to_file("00_Report/minimization.tex",rawtext);
}
void latex::write_cold_equil_tex(JobSettings settings)
{
    std::string rawtext=R"(
\section*{Cold Density Equilibration}
\paragraph{}Cold density equilibration at 10K, holding pressure constant (NPT ensemble).
Restraints are iteratively relaxed (linear descent over)" 
+ std::to_string(settings.NUM_COLD_STEPS) + 
R"( steps) on )" 
+ settings.COMPLEX_MASK + 
R"(, beginning at )" 
+ std::to_string(settings.MAX_RESTRAINT) + 
R"($kcal\\cdot mol^{-1}$.
)";
    utils::write_to_file("00_Report/cold_equil.tex",rawtext);

}
void latex::write_heating_tex()
{
    std::string rawtext = R"(
\section*{System Heating}
\paragraph{}Heating through iterative steps at 10K, 20K, 50K, 100K, 150K, 200K, 250K, 300K, 325K, and 300K.
Each stage runs for 10000 timesteps (0.01 ns) for a total of 0.1ns of simulated heating to final temperature of 300K.
)";
    utils::write_to_file("00_Report/heating.tex",rawtext);

}
void latex::write_hot_equil_tex(JobSettings settings)
{
    std::string rawtext = R"(
\section*{Hot Density Equilibration}
\paragraph{}Hot density equilibration at 300K, holding pressure constant (NPT ensemble).
Restraints are iteratively relaxed (linear descent over)" 
+ std::to_string(settings.NUM_HOT_STEPS) + 
R"( steps) on )" 
+ settings.COMPLEX_MASK + 
R"(, beginning at )" 
+ std::to_string(settings.MAX_RESTRAINT) + 
R"($kcal\\cdot mol^{-1}$.
)";
    utils::write_to_file("00_Report/hot_equil.tex",rawtext);

}
void latex::write_production_tex(JobSettings settings)
{
    std::string rawtext = R"(
\section*{Production Molecular Dynamics}
\paragraph{}Production molecular dynamics will be run with a 1fs timestep for)" + std::to_string(settings.NUM_PROD_STEPS) 
+ R"(ns with periodic boundary conditions at constant volume (NVT ensemble).
)";
    std::string mmpbsa_text = R"(
\subsection*{MMPBSA Calculations}
\paragraph{} MMPBSA calculations will be performed alongside production.  These calculations are intended to calculate the binding energy for a given complex.

\noindent \begin{tabular}{|p{0.2\textwidth}|p{0.7\textwidth}|}
\hline
Receptor mask:    & )" + settings.RECEPTOR_MASK + R"( \\
\hline
Ligand mask:      & )" + settings.LIGAND_MASK + R"( \\
\hline
Complex mask:     & )" + settings.COMPLEX_MASK + R"( \\
\hline
\end{tabular}
)";
    utils::write_to_file("00_Report/production.tex",rawtext);
    if (settings.RUN_MMPBSA)
    {
        utils::append_to_file("00_Report/production.tex",mmpbsa_text);
    }
}
void latex::write_analysis_tex(JobSettings settings)
{
    std::string rawtext = R"(
\section*{Analysis}
\subsection*{Base Analyses}
\paragraph{} Basic analyses will be performed upon completion of production MD.
\begin{itemize}
\item \textbf{RMSD} - Root-mean-squared deviation is a measure of how far from a given reference structure (usually starting coordinates) a molecular dynamics system has changed.  Useful for monitoring conformational changes and system stability.
\item \textbf{RMSF} - The magnitude of fluctuation of amino acids in a protein.  High RMSF indicates a highly mobile amino acid.
\item \textbf{Hydrogen Bond Analysis} - Calculation of hydrogen bond interactions (HBI), including fraction of total simulation time for any given HBI.
\item \textbf{Normal Modes} - Top 100 vibrational modes of motion for the system.  Useful to identify the largest contributions to the overall dynamic motion of the protein.
\item \textbf{Correlated Motion} - A measure of how each amino acid moves with respect to other amino acids.  AAs which move together are correlated, those which move in direct opposition are anti-correlated, and those which move orthogonally to each other are non-correlated.
)" + ligand_analysis_text(settings) + R"(
\end{itemize}
\noindent\begin{tabular}{|p{0.2\textwidth}|p{0.7\textwidth}|}
)";
    utils::write_to_file("00_Report/analysis.tex",rawtext);

    // Add individual analysis masks
    std::stringstream buffer;
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
    utils::append_to_file("00_Report/analysis.tex",buffer.str());
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
    buffer << "module load texlive/2020; pdflatex -interaction=nonstopmode --shell-escape main.tex > /dev/null";
    utils::silent_shell(buffer.str().c_str());
    if (fs::exists("main.pdf"))
    {
        buffer.str("");
        buffer << "mv main.pdf ../" << settings.PRMTOP.substr(0,settings.PRMTOP.find(".prmtop")) << "_Report.pdf";
        utils::silent_shell(buffer.str().c_str());
    }    
    std::experimental::filesystem::current_path(curr_path);
}
void latex::figure_block_to_file(std::string imagebase, std::string caption, std::string filename)
{
    std::stringstream latex;
    latex << "\\begin{figure}[htbp] " << std::endl;
    latex << "\\centering" << std::endl;
    latex << "\\includegraphics[width=0.9\\textwidth]{" << imagebase << ".png}" << std::endl;
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