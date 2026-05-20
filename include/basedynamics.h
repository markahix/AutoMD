#ifndef BASEDYNAMICS_H
#define BASEDYNAMICS_H
#include "ambermachine.h"


int GetStartBead(std::string directory);
void UpdateReport();
void GenerateFileNames(FileList &files, std::string filebasename, int step_num);
void SetRestartFile(int startbead, std::string production_directory);
void CompressProductionFolder(JobSettings settings);
std::string GetCompressedProductionFilename(JobSettings settings);
void GeneratePlotsAndReport(FileList files);
void CallByNanosecondAnalytics(JobSettings settings, SlurmSettings slurm, FileList &files);


#endif