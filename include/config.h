#ifndef CONFIG_H
#define CONFIG_H

#define DEFAULT_TERACHEM_MODULE "Terachem/1.9.3-2023"
#define DEFAULT_PDFLATEX_MODULE "texlive/2020"
#define DEFAULT_AMBER_MODULE "Amber/20-cuda-11"

// SLURM GPU Job Settings
#define SLURM_GPU_JOB_QUEUE "express"
#define SLURM_GPU_JOB_PARTITION "earwp"
#define SLURM_GPU_JOB_INCLUDE_NODES = "arw4,arw5"
#define SLURM_GPU_JOB_EXCLUDE_NODES = "arw1,arw2,arw3"
#define SLURM_GPU_JOB_GPUNAME = "gpu:nvidia_a30_1g.12gb:2"
#define SLURM_GPU_JOB_MAX_MEMORY = "20GB"

// SLURM CPU Job Settings
#define SLURM_CPU_JOB_QUEUE "primary"


#endif