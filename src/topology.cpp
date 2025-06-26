#include "topology.h"


Topology::Topology(std::string filename)
{
    topology_file = filename;
}


Topology::~Topology()
{

}

void Topology::ReadTopology()
{
    std::ifstream ifile(topology_file,std::ios::in);
    std::string line;
    std::string key = "JUNK";
    std::map <std::string, std::vector<std::string>> sections;
    if (!ifile.is_open())
    {
        std::cout << "Unable to open topology file: " << topology_file << std::endl;
        return;
    }

    while (getline(ifile, line))
    {
        // Splits file into manageable chunks, sectioned by "%FLAG" lines.
        if (line.find("%COMMENT") != std::string::npos)
        {
            continue;
        }
        if (line.find("%FORMAT") != std::string::npos)
        {
            continue;
        }
        if (line.find("%VERSION") != std::string::npos)
        {
            continue;
        }
        if (line.find("%FLAG") != std::string::npos)
        {
            key = line.substr(6,line.size());
            std::cout << "Topology Section is currently: " << key << std::endl;
            sections[key] = {};
        }
        else
        {
            sections[key].push_back(line);
        }
    }
    // Parse each section appropriately. ATOM_NAME, RESIDUE_LABEL, RESIDUE_POINTER, ATOMS_PER_MOLECULE, etc.
    
    // Parse "ATOM_NAME" section
    for (int i=0; i < sections["ATOM_NAME"].size(); i++)
    {
        std::string line = sections["ATOM_NAME"][i];
        for (int j=0; j < line.size(); j+=4)
        {
            std::string a_name;
            a_name = line.substr(j,4);
            while (a_name[0] == ' ')
                a_name = a_name.substr(1,a_name.size()-1);
            while (a_name.back() == ' ')
                a_name = a_name.substr(0,a_name.size()-1);
            if (a_name.size() > 0)
                atom_names.push_back(a_name);
        }
    }
    std::cout << "Total number of atoms identified in given PRMTOP: " << atom_names.size() << std::endl;

    // Parse "ATOM_NAME" section
    for (int i=0; i < sections["ATOM_NAME"].size(); i++)
    {
        std::stringstream buffer;
        std::string a_name;
        buffer.str(sections["ATOM_NAME"][i]);
        while (buffer >> a_name)
        {
            atom_names.push_back(a_name);
        }
    }
    std::cout << "Total number of atoms identified in given PRMTOP: " << atom_names.size() << std::endl;
    /*
        1. Identify Molecule Chunks (proteins, nucleic acid chains, etc.)
        2. Identify known residues in each molecule (AAs, DNA/RNA, etc.)
        3. If protein found, identify sequence and output to report.
            a. If modified AAs or unknown residues mixed in, flag in report.
        4. If nucleic acid found, identify sequence and output to report.
            b. If modified NA or unknown residues mixed in, flag in report.
        5. If any small molecules, ligands, etc. are found, identify and flag in report.

    CPPTRAJ analysis should do the following based on prmtop parsing:
        1. Complete system:
            a. RMSD (all non-solvent, non-counterion)
            b. HBond analysis (nointramol)
        1. Protein molecules:
            a. RMSD
            b. RMSF
            c. Correlation
            d. Normal Modes
            e. Ramachandran (backbone phi/psi angles)
        1. RMSD on all non-solvent, non-counterion molecules (proteins, nucleic acids, etc.) individually (to identify internal changes) and collectively (to identify system-wide changes)
        2. Calculate RMSF on all proteins and nucleic acids
        3. Calculate center-of-mass distances between all large molecules.
        4. 
    */


    /*Sections to handle: (https://ambermd.org/prmtop.pdf)
        - TITLE
        - POINTERS
        - ATOM_NAME
        - CHARGE
        - ATOMIC_NUMBER
        - MASS
        - ATOM_TYPE_INDEX
        - NUMBER_EXCLUDED_ATOMS
        - NONBONDED_PARM_INDEX
        - RESIDUE_LABEL
        - RESIDUE_POINTER
        - BOND_FORCE_CONSTANT
        - BOND_EQUIL_VALUE
        - ANGLE_FORCE_CONSTANT
        - ANGLE_EQUIL_VALUE
        - DIHEDRAL_FORCE_CONSTANT
        - DIHEDRAL_PERIODICITY
        - DIHEDRAL_PHASE
        - SCEE_SCALE_FACTOR
        - SCNB_SCALE_FACTOR
        - SOLTY
        - LENNARD_JONES_ACOEF
        - LENNARD_JONES_BCOEF
        - BONDS_INC_HYDROGEN
        - BONDS_WITHOUT_HYDROGEN
        - ANGLES_INC_HYDROGEN
        - ANGLES_WITHOUT_HYDROGEN
        - DIHEDRALS_INC_HYDROGEN
        - DIHEDRALS_WITHOUT_HYDROGEN
        - EXCLUDED_ATOMS_LIST
        - HBOND_ACOEF
        - HBOND_BCOEF
        - HBCUT
        - AMBER_ATOM_TYPE
        - TREE_CHAIN_CLASSIFICATION
        - JOIN_ARRAY
        - IROTAT
        - SOLVENT_POINTERS
        - ATOMS_PER_MOLECULE
        - BOX_DIMENSIONS
        - CAP_INFO
        - CAP_INFO2
        - RADIUS_SET
        - RADII
        - IPOL
        - POLARIZABILITY
    */   
}

void Topology::FindProteins()
{

}

void Topology::FindNucleics()
{

}

void Topology::FindCounterions()
{

}

void Topology::FindSolvents()
{

}

void Topology::FindLigands()
{

}