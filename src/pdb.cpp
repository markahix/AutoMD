#include "pdb.h"

// PDBAtom Functions
PDBAtom::PDBAtom(std::string line)
{
    index = atoi(line.substr(6,5).c_str());
    atom_name = line.substr(12,4);
    residue_name = line.substr(17,3);
    chain_id = line.substr(21,1);
    if (chain_id == " ")
    {
        chain_id = "A";
    }
    residue_number = atoi(line.substr(22,4).c_str());
    x = atof(line.substr(30,8).c_str());
    y = atof(line.substr(38,8).c_str());
    z = atof(line.substr(46,8).c_str());
    element = line.substr(76,2);
    charge = 0;
    if (line.substr(78,2).find_first_not_of(' ') != std::string::npos)
    {
        charge = atoi(line.substr(78,2).c_str());
    }
}
PDBAtom::~PDBAtom()
{

}
void PDBAtom::set_index(int idx)
{
    index = idx;
}
void PDBAtom::set_atom_name(std::string aname)
{
    atom_name = aname;
}
void PDBAtom::set_residue_name(std::string rname)
{
    residue_name = rname;
}
void PDBAtom::set_chain_id(std::string chid)
{
    chain_id = chid;
}
void PDBAtom::set_residue_number(int rnum)
{
    residue_number = rnum;
}
void PDBAtom::set_xyz(double newx, double newy, double newz)
{
    x = newx;
    y = newy;
    z = newz;
}
void PDBAtom::set_element(std::string new_element)
{
    element = new_element;
}
void PDBAtom::set_charge(int new_charge)
{
    charge = new_charge;
}
std::string PDBAtom::get_atom_name()
{
    return atom_name;
}
//PDBResidue Functions

void PDBResidue::AddAtom(std::string line)
{
    atoms.push_back(PDBAtom(line));
}
void PDBResidue::CheckForErrors()
{   
    // Check for unique atom names.
    std::vector<std::string> tmp_atom_names = {};
    for (int i = 0;i < atoms.size(); i++)
    {
        tmp_atom_names.push_back(atoms[i].get_atom_name());
    }
    std::set<std::string> unique_names(tmp_atom_names.begin(), tmp_atom_names.end());
    if (unique_names.size() != tmp_atom_names.size())
    {
        std::cout << "ERROR: Residue " << resid << " may have non-unique atom names." << std::endl;
    }
    
}
void PDBResidue::set_resid(int new_resid)
{
    resid = new_resid;
    for (int i = 0;i < atoms.size(); i++)
    {
        atoms[i].set_residue_number(resid);
    }
}
void PDBResidue::set_resname(std::string new_resname)
{
    resname = new_resname;
    for (int i = 0;i < atoms.size(); i++)
    {
        atoms[i].set_residue_name(resname);
    }
}
void PDBResidue::set_chain_id(std::string new_chid)
{
    chain_id = new_chid;
    for (int i = 0;i < atoms.size(); i++)
    {
        atoms[i].set_chain_id(chain_id);
    }
}

PDBResidue::PDBResidue(int id, std::string name, std::string chain)
{
    atoms = {};
    resid = id;
    resname = name;
    chain_id = chain;
}
PDBResidue::~PDBResidue()
{

}


// CONSTRUCTOR
PDB::PDB(std::string filename)
{
    pdb_file = filename;
    molecule_ranges = {};
}

// DESTRUCTOR
PDB::~PDB()
{

}

void PDB::ReadPDB()
{
    std::string line;
    std::ifstream ifile(pdb_file,std::ios::in);
    if (!ifile.is_open())
    {
        std::cout << "Unable to open " << pdb_file << ". Aborting. " << std::endl;
        return;
    }

    std::stringstream current_molecule_range;
    int current_residue = 0;
    int start_molecule = 0;
    bool first_atom_found = false;

    while (getline(ifile,line))
    {   
        if (line.find("END") != std::string::npos)
        {   // Reached the end of the PDB's useful information, bail out.
            break;
        }
        if (line.find("TER") != std::string::npos)
        {
            // End of molecule, close the current molecule and prepare another molecule.
            current_molecule_range.str("");
            current_molecule_range << ":" << start_molecule;
            if (current_residue != start_molecule)
            {
                current_molecule_range << "-" << current_residue;
            }
            molecule_ranges.push_back(current_molecule_range.str());
            first_atom_found = false;
            
            continue;
        }
        if ((line.find("ATOM") == std::string::npos) && (line.find("HETATM") == std::string::npos) )
        {   // If line is not an atom-containing line, continue.
            continue;
        }
        current_residue = atoi(line.substr(22,4).c_str());
        if (!first_atom_found)
        {
            start_molecule = atoi(line.substr(22,4).c_str());
            first_atom_found = true;
        }
    }

}