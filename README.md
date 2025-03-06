# AmberMachine

This codebase is currently built to work on Wayne State University's Grid HPC.  
It is recommended that anyone wishing to use this code be sure to edit the defaults to align with their own HPC environment to streamline end-user friendliness.

### What does this do?

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