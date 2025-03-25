# AmberMachine

This codebase is currently built to work on Wayne State University's Grid HPC.  
It is recommended that anyone wishing to use this code be sure to edit the defaults to align with their own HPC environment to streamline end-user friendliness.

#### What does this do?

Simply put, I got tired to having to manually manage each and every classical molecular dynamics simulation I run, including keeping track of replicate trajectories, what files correspond to what stage of the simulation, and what analyses I need to run for a given system.
I also found myself in situations where I'd run up against a wall-clock limitations.
So I wrote a program to automate and streamline as much of the process as I could.  Now, with a single input file, I can perform a complete MD simulation with an arbitrary number of replicates, each one spawning their own stage-specific SLURM jobs to avoid wall-clock limits while also ensuring that I'm not just holding up resources for long periods of time at a stretch.  

#### The Goals of The Amber Machine
1. Be a good citizen of the HPC
2. Maintain consistency in filenames and directory heirarchies to streamline later analyses.
3. Achieve high reproducibility in pursuit of good science.
4. Generate automatic reports for the end user with as much *useful* analysis as possible
5. Include error checking and handling wherever possible to reduce the need for human involvement.
6. Free up mental bandwidth for users to spend on the deeper analysis rather than the technical processes.

#### The Current Workflow
1. **Initialize** - Parse the input file for all options, ensure all necessary files (prmtop, restart, etc.) exist, prepare necessary inputs for concurrent analyses such as MMPBSA or SASA calculations during later stages, prepare report templates and job directory heirarchy including replicates.
2. **Minimize** - Classical MD simulations usually require initial minimizations to remove clashes and ensure reasonable starting positions. (NPT)
3. **Cold Density Equilibration** - Restraints are slowly lifted from the main solute (usually a protein/ligand complex or other molecule more complex than the surrounding solvent) as the solvent equilibrates in density at 10K.  (NPT ensemble)
4. **Iterative Heating** - The temperature of the system is increased in 15K increments from 10K to 325K, then back down to 300K, with each increment running for 10ps.  This is done to allow solvent to smoothly accelerate to velocities appropriate for the desired temperature.
5. **Hot Density Equilibration**  - Restraints are again slowly lifted from the main solute to allow solvent to equilibrate in density, this time at 300K.  (NPT ensemble)
6. **Production** - Production MD stage.  Each nanosecond of simulation is recorded as a separate numbered file.  (NVT ensemble)
7. **Analysis** - Analysis of completed production MD.  Base analyses include RMSD, RMSF, correlation, hydrogen bond analysis, and normal mode/principal component analysis.  The input file can include additional `cpptraj` commands which will be performed after the base analyses.
8. **Report** - The report is updated at each stage of the workflow, so this stage primarily ensures that everything is properly compiled and the PDF made available to the end user.

