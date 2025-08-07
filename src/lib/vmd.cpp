#include "vmd.h"

void vmd::prepare_workspace()
{
    std::string vmd = R"""(display update on
color add item Display Background white
color Display Background white
display projection orthographic
display culling off
axes location off
display rendermode Normal
display depthcue off
display resize 1920 1080
)""";
    utils::write_to_file("ambermachine.vmd",vmd);
}

void vmd::new_molecule(std::string prmtop,std::string restart)
{
    std::stringstream vmd;
    vmd.str("");
    vmd << "mol new " << prmtop << " type parm7 waitfor all" << std::endl;
    vmd << "mol addfile " << restart << " type rst7 waitfor all" << std::endl;
    vmd << "mol delrep 0 top" << std::endl;
    utils::append_to_file("ambermachine.vmd",vmd.str());
}

void new_trajectory(std::string prmtop)
{
    std::stringstream vmd;
    vmd.str("");
    vmd << "mol new " << prmtop << " type parm7 waitfor all" << std::endl;
    for (std::experimental::filesystem::path p : std::experimental::filesystem::directory_iterator("05_Production/"))
    {
        if (p.extension() == ".mdcrd")
        {
        vmd << "mol addfile " << p << "type mdcrd first 0 last -1 step 1 filebonds 1 autobonds 1 waitfor all" << std::endl;
        }
    }
    vmd << "mol delrep 0 top" << std::endl;
    utils::append_to_file("ambermachine.vmd",vmd.str());
}

void vmd::display_NewCartoon(std::string atom_selection, int rep_num)
{
    std::stringstream vmd;
    vmd.str("");
    vmd << "mol representation NewCartoon 0.300000 50.000000 4.500000 0" << std::endl;
    vmd << "mol color Beta" << std::endl;
    vmd << "mol selection " << atom_selection << std::endl;
    vmd << "mol material Glossy" << std::endl;
    vmd << "mol addrep top" << std::endl;
    vmd << "mol selupdate " << rep_num << " top 0" << std::endl;
    vmd << "mol colupdate " << rep_num << " top 0" << std::endl;
    vmd << "mol scaleminmax top " << rep_num << " 0.000000 0.000000" << std::endl;
    vmd << "mol smoothrep top " << rep_num << " 0" << std::endl;
    vmd << "mol drawframes top " << rep_num << " {now}" << std::endl;
    utils::append_to_file("ambermachine.vmd",vmd.str());
}

void vmd::display_Licorice(std::string atom_selection, int rep_num)
{
    std::stringstream vmd;
    vmd.str("");
    vmd << "mol representation Licorice 0.100000 100.000000 100.000000" << std::endl;
    vmd << "mol color Element" << std::endl;
    vmd << "mol selection " << atom_selection << std::endl;
    vmd << "mol material Glossy" << std::endl;
    vmd << "mol addrep top" << std::endl;
    vmd << "mol selupdate " << rep_num << " top 0" << std::endl;
    vmd << "mol colupdate " << rep_num << " top 0" << std::endl;
    vmd << "mol scaleminmax top " << rep_num << " 0.000000 0.000000" << std::endl;
    vmd << "mol smoothrep top " << rep_num << " 0" << std::endl;
    vmd << "mol drawframes top " << rep_num << " {now}" << std::endl;
    utils::append_to_file("ambermachine.vmd",vmd.str());
        
}

void vmd::color_residue_by_beta(std::vector<double> array)
{
    std::stringstream vmd;
    vmd.str("");
    for(unsigned int i=0; i < array.size(); i++) 
    {
        vmd << "[atomselect top \"resid " << i+1 << "\"] set beta " << array[i] << std::endl;
    }
    utils::append_to_file("ambermachine.vmd",vmd.str());
}

void vmd::color_atom_by_beta(std::vector<double> array)
{
    std::stringstream vmd;
    vmd.str("");
    for(unsigned int i=0; i < array.size(); i++) 
    {
        vmd << "[atomselect top \"index " << i << "\"] set beta " << array[i] << std::endl;
    }
    utils::append_to_file("ambermachine.vmd",vmd.str());
      
}

void vmd::render_images(std::string basename, int num_rotations)
{
    std::stringstream vmd;
    // set window scaling
    vmd.str("");
    vmd << R""""(
proc get_scale_factor {molid selec} {
  set sel [atomselect $molid all]
  set coords [$sel get {x y z}]
  set coord [lvarpop coords]
  lassign $coord minx miny minz
  lassign $coord maxx maxy maxz
  foreach coord $coords {
    lassign $coord x y z
    if {$x < $minx} {set minx $x} else {if {$x > $maxx} {set maxx $x}}
    if {$y < $miny} {set miny $y} else {if {$y > $maxy} {set maxy $y}}
    if {$z < $minz} {set minz $z} else {if {$z > $maxz} {set maxz $z}}
  }
  set rx [expr $maxx - $minx]
  set ry [expr $maxy - $miny]
  set rz [expr $maxz - $minz]
  set sel [atomselect $molid $selec]
  set coords [$sel get {x y z}]
  set coord [lvarpop coords]
  lassign $coord minx miny minz
  lassign $coord maxx maxy maxz
  foreach coord $coords {
    lassign $coord x y z
    if {$x < $minx} {set minx $x} else {if {$x > $maxx} {set maxx $x}}
    if {$y < $miny} {set miny $y} else {if {$y > $maxy} {set maxy $y}}
    if {$z < $minz} {set minz $z} else {if {$z > $maxz} {set maxz $z}}
  }
  set x_r [expr $maxx - $minx]
  set y_r [expr $maxy - $miny]
  set z_r [expr $maxz - $minz]
  set scale_x [expr $rx / $x_r]
  set scale_y [expr $ry / $y_r]
  set scale_z [expr $rz / $z_r]
  set s_factor 10
  if {$s_factor > $scale_x} {set s_factor $scale_x}
  if {$s_factor > $scale_y} {set s_factor $scale_y}
  if {$s_factor > $scale_z} {set s_factor $scale_z}    
  return $s_factor
}
set scale_by [get_scale_factor 0 "resid 1 to 460"]
scale by $scale_by
)"""";
    utils::append_to_file("ambermachine.vmd",vmd.str());

    int rotation_degrees = 360/num_rotations;
    for (int i =0; i < num_rotations; i++)
    {
        vmd.str("");
        vmd << "set filename " << basename << "_rotation." << std::setw(4) << std::setfill('0') << i+1 << ".tga " <<std::endl;
        vmd << "render TachyonLOptiXInternal $filename" << std::endl;
        vmd << "rotate y by " << rotation_degrees << std::endl;
        utils::append_to_file("ambermachine.vmd",vmd.str());
    }
    // After this function is called, there should be a call to imagemagick to create a 1-by-X array of the produced images with a provided filename ending in png.
}

void vmd::trajectory_animation()
{
std::string vmd = R""""(
proc make_trajectory_movie {} {
    # get the number of frames in the movie
    set num [molinfo top get numframes]
    # loop through the frames
    for {set i 0} {$i < $num} {incr i 1} {
        # go to the given frame
        animate goto $i
                # for the display to update
                display update
        # take the picture
        set filename VMD_Images/temp.[format \"%05d\" [expr $i/1]].tga
        render TachyonLOptiXInternal $filename
    }
}
make_trajectory_movie
)"""";
    utils::append_to_file("ambermachine.vmd",vmd);
}

void vmd::exit()
{
    std::stringstream vmd;
    vmd.str("");
    vmd << "quit" << std::endl;
    utils::append_to_file("ambermachine.vmd",vmd.str());
}

void vmd::run_vmd()
{
    std::string command="module load vmd/1.9.3; vmd -dispdev text < ambermachine.vmd > vmd.log";
    utils::silent_shell(command.c_str());
}

void vmd::ffmpeg_video_grid()
{
    std::string ffmpeg = R""""(ffmpeg -i vmd1.avi -i vmd2.avi -i vmd3.avi -i vmd4.avi -filter_complex "nullsrc=size=1920x1080 [base]; [0:v] setpts=PTS-STARTPTS, scale=960x540 [upperleft]; [1:v] setpts=PTS-STARTPTS, scale=960x540 [upperright]; [2:v] setpts=PTS-STARTPTS, scale=960x540 [lowerleft]; [3:v] setpts=PTS-STARTPTS, scale=960x540 [lowerright]; [base][upperleft] overlay=shortest=1 [tmp1]; [tmp1][upperright] overlay=shortest=1:x=960 [tmp2]; [tmp2][lowerleft] overlay=shortest=1:y=540 [tmp3]; [tmp3][lowerright] overlay=shortest=1:x=960:y=540" -c:v libx264 VMD_Trajectory.mkv)"""";
    utils::silent_shell(ffmpeg.c_str());
}
