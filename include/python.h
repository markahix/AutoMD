#ifndef python_h
#define python_h

#include "utilities.h"
#include "classes.h"

namespace python
{
    void plot_csv_data(std::string filename);
    void plot_cpptraj_results();
    std::string preamble();
    std::string plot_rmsd();
    std::string plot_rmsf();
    std::string plot_correl();
    std::string plot_lie();
    std::string plot_normal_modes();
    std::string plot_pca();
    std::string plot_sasa();
}


#endif