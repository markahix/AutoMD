#ifndef topology_h
#define topology_h

#include "utilities.h"

/*
#####################
# FROM AMBER MANUAL #
#####################
Requirements for prmtop parsers Parsers, regardless of the language they are written in, should conform to a list of attributes to maximize the likelihood that they are parsed correctly.
    - Parsers should expect that some 4-character fields (e.g., atom or residue names) may have some names that have 4 characters and therefore might not be whitespace-delimited.
    - Parsers should not expect SECTIONs in the prmtop to be in any particular order.
    - Parsers should not expect or require %COMMENT lines to exist, but should properly parse the file if any number of %COMMENT lines appear as indicated above
    - The topology file may be assumed to have been generated ‘correctly’ by tleap or some other credible source. No graceful error checking is required.
*/

class Topology
{
    private:
        std::string topology_file;
        std::vector <std::string> atom_names;
        std::vector <std::string> residue_labels;
    public:
        void ReadTopology();
        void FindProteins();
        void FindNucleics();
        void FindCounterions();
        void FindSolvents();
        void FindLigands();
        Topology(std::string filename);
        ~Topology();
};
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
        - SOLVENT_POINTERS  <--- this is where I can identify what's solvent and what's solute (includes counterions, so be careful here)
        - ATOMS_PER_MOLECULE <--- this is where I can identify separated protein chains, ligands, counterions, and then solvent molecules.
        - BOX_DIMENSIONS
        - CAP_INFO
        - CAP_INFO2
        - RADIUS_SET
        - RADII
        - IPOL
        - POLARIZABILITY
    */   

#endif