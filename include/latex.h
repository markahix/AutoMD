#ifndef latex_h
#define latex_h

#include "utilities.h"
#include "classes.h"

namespace latex
{
    void write_main_tex();
    void write_timeline_tex();
    void write_initialize_tex(JobSettings settings);
    void write_minimization_tex();
    void write_cold_equil_tex(JobSettings settings);
    void write_heating_tex();
    void write_hot_equil_tex(JobSettings settings);
    void write_production_tex(JobSettings settings);
    void write_analysis_tex(JobSettings settings);
    void initialize_report(JobSettings settings);
    void compile_report(JobSettings settings);
    void figure_block_to_file(std::string image, std::string caption,std::string filename);
    void python_block_to_file(std::string python, std::string filename);
}

#endif