#ifndef VMD_H
#define VMD_H

#include "utilities.h"

namespace vmd
{
    void prepare_workspace();
    void new_molecule(std::string prmtop,std::string restart);
    void new_trajectory(std::string prmtop);
    void display_NewCartoon(std::string atom_selection, int rep_num);
    void display_Licorice(std::string atom_selection, int rep_num);
    // void display_CPK(std::string atom_selection, int rep_num);
    // void display_QuickSurf(std::string atom_selection, int rep_num);
    void color_residue_by_beta(std::vector<double> array);
    void color_atom_by_beta(std::vector<double> array);
    void render_images(std::string basename, int num_rotations);
    void trajectory_animation();
    void exit();
    void run_vmd();
    void ffmpeg_video_grid();

}

#endif