#ifndef PDB_H
#define PDB_H

#include "utilities.h"

class PDBAtom
{
    private:
        int index;
        std::string atom_name;
        std::string residue_name;
        std::string chain_id;
        int residue_number;
        double x;
        double y;
        double z;
        std::string element;
        int charge;
    public:
        PDBAtom(std::string line);
        ~PDBAtom();
        void set_index(int idx);
        void set_atom_name(std::string aname);
        void set_residue_name(std::string rname);
        void set_chain_id(std::string chid);
        void set_residue_number(int rnum);
        void set_xyz(double newx, double newy, double newz);
        void set_element(std::string new_element);
        void set_charge(int new_charge);
        std::string get_atom_name();
};

class PDBResidue
{
    private:
        std::vector<PDBAtom> atoms;
        int resid;
        std::string resname;
        std::string chain_id;

    public:
        void AddAtom(std::string line);
        void CheckForErrors();
        void set_resid(int new_resid);
        void set_resname(std::string new_resname);
        void set_chain_id(std::string new_chid);
        PDBResidue(int id, std::string name, std::string chain);
        ~PDBResidue();
};

class PDBMolecule
{
    private:
        std::vector<PDBResidue> residues;
        int molecule_number;
    public:
        void AddResidue(int id, std::string name, std::string chain);
        PDBMolecule(int number);
        ~PDBMolecule();
};


class PDB
{
    private:
        std::string pdb_file;
        std::vector <std::string> molecule_ranges;
        std::vector <PDBMolecule> molecules;
        
    public:
        void ReadPDB();

        PDB(std::string filename);
        ~PDB();
};

#endif