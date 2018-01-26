#include "NWChemExRuntime/AtomicInfo.hpp"
#include <algorithm>

namespace NWXRuntime {
namespace detail_ {
extern const std::unordered_map<std::string, std::size_t> sym2Z_{
  { "gho" , 0 },
  { "GHO" , 0 },
  { "h" , 1 },
  { "H" , 1 },
  { "he" , 2 },
  { "HE" , 2 },
  { "He", 2 },
  { "li" , 3 },
  { "LI" , 3 },
  { "Li", 3 },
  { "be" , 4 },
  { "BE" , 4 },
  { "Be", 4 },
  { "b" , 5 },
  { "B" , 5 },
  { "c" , 6 },
  { "C" , 6 },
  { "n" , 7 },
  { "N" , 7 },
  { "o" , 8 },
  { "O" , 8 },
  { "f" , 9 },
  { "F" , 9 },
  { "ne" , 10 },
  { "NE" , 10 },
  { "Ne", 10 },
  { "na" , 11 },
  { "NA" , 11 },
  { "Na", 11 },
  { "mg" , 12 },
  { "MG" , 12 },
  { "Mg", 12 },
  { "al" , 13 },
  { "AL" , 13 },
  { "Al", 13 },
  { "si" , 14 },
  { "SI" , 14 },
  { "Si", 14 },
  { "p" , 15 },
  { "P" , 15 },
  { "s" , 16 },
  { "S" , 16 },
  { "cl" , 17 },
  { "CL" , 17 },
  { "Cl", 17 },
  { "ar" , 18 },
  { "AR" , 18 },
  { "Ar", 18 },
  { "k" , 19 },
  { "K" , 19 },
  { "ca" , 20 },
  { "CA" , 20 },
  { "Ca", 20 },
  { "sc" , 21 },
  { "SC" , 21 },
  { "Sc", 21 },
  { "ti" , 22 },
  { "TI" , 22 },
  { "Ti", 22 },
  { "v" , 23 },
  { "V" , 23 },
  { "cr" , 24 },
  { "CR" , 24 },
  { "Cr", 24 },
  { "mn" , 25 },
  { "MN" , 25 },
  { "Mn", 25 },
  { "fe" , 26 },
  { "FE" , 26 },
  { "Fe", 26 },
  { "co" , 27 },
  { "CO" , 27 },
  { "Co", 27 },
  { "ni" , 28 },
  { "NI" , 28 },
  { "Ni", 28 },
  { "cu" , 29 },
  { "CU" , 29 },
  { "Cu", 29 },
  { "zn" , 30 },
  { "ZN" , 30 },
  { "Zn", 30 },
  { "ga" , 31 },
  { "GA" , 31 },
  { "Ga", 31 },
  { "ge" , 32 },
  { "GE" , 32 },
  { "Ge", 32 },
  { "as" , 33 },
  { "AS" , 33 },
  { "As", 33 },
  { "se" , 34 },
  { "SE" , 34 },
  { "Se", 34 },
  { "br" , 35 },
  { "BR" , 35 },
  { "Br", 35 },
  { "kr" , 36 },
  { "KR" , 36 },
  { "Kr", 36 },
  { "rb" , 37 },
  { "RB" , 37 },
  { "Rb", 37 },
  { "sr" , 38 },
  { "SR" , 38 },
  { "Sr", 38 },
  { "y" , 39 },
  { "Y" , 39 },
  { "zr" , 40 },
  { "ZR" , 40 },
  { "Zr", 40 },
  { "nb" , 41 },
  { "NB" , 41 },
  { "Nb", 41 },
  { "mo" , 42 },
  { "MO" , 42 },
  { "Mo", 42 },
  { "tc" , 43 },
  { "TC" , 43 },
  { "Tc", 43 },
  { "ru" , 44 },
  { "RU" , 44 },
  { "Ru", 44 },
  { "rh" , 45 },
  { "RH" , 45 },
  { "Rh", 45 },
  { "pd" , 46 },
  { "PD" , 46 },
  { "Pd", 46 },
  { "ag" , 47 },
  { "AG" , 47 },
  { "Ag", 47 },
  { "cd" , 48 },
  { "CD" , 48 },
  { "Cd", 48 },
  { "in" , 49 },
  { "IN" , 49 },
  { "In", 49 },
  { "sn" , 50 },
  { "SN" , 50 },
  { "Sn", 50 },
  { "sb" , 51 },
  { "SB" , 51 },
  { "Sb", 51 },
  { "te" , 52 },
  { "TE" , 52 },
  { "Te", 52 },
  { "i" , 53 },
  { "I" , 53 },
  { "xe" , 54 },
  { "XE" , 54 },
  { "Xe", 54 },
  { "cs" , 55 },
  { "CS" , 55 },
  { "Cs", 55 },
  { "ba" , 56 },
  { "BA" , 56 },
  { "Ba", 56 },
  { "la" , 57 },
  { "LA" , 57 },
  { "La", 57 },
  { "ce" , 58 },
  { "CE" , 58 },
  { "Ce", 58 },
  { "pr" , 59 },
  { "PR" , 59 },
  { "Pr", 59 },
  { "nd" , 60 },
  { "ND" , 60 },
  { "Nd", 60 },
  { "pm" , 61 },
  { "PM" , 61 },
  { "Pm", 61 },
  { "sm" , 62 },
  { "SM" , 62 },
  { "Sm", 62 },
  { "eu" , 63 },
  { "EU" , 63 },
  { "Eu", 63 },
  { "gd" , 64 },
  { "GD" , 64 },
  { "Gd", 64 },
  { "tb" , 65 },
  { "TB" , 65 },
  { "Tb", 65 },
  { "dy" , 66 },
  { "DY" , 66 },
  { "Dy", 66 },
  { "ho" , 67 },
  { "HO" , 67 },
  { "Ho", 67 },
  { "er" , 68 },
  { "ER" , 68 },
  { "Er", 68 },
  { "tm" , 69 },
  { "TM" , 69 },
  { "Tm", 69 },
  { "yb" , 70 },
  { "YB" , 70 },
  { "Yb", 70 },
  { "lu" , 71 },
  { "LU" , 71 },
  { "Lu", 71 },
  { "hf" , 72 },
  { "HF" , 72 },
  { "Hf", 72 },
  { "ta" , 73 },
  { "TA" , 73 },
  { "Ta", 73 },
  { "w" , 74 },
  { "W" , 74 },
  { "re" , 75 },
  { "RE" , 75 },
  { "Re", 75 },
  { "os" , 76 },
  { "OS" , 76 },
  { "Os", 76 },
  { "ir" , 77 },
  { "IR" , 77 },
  { "Ir", 77 },
  { "pt" , 78 },
  { "PT" , 78 },
  { "Pt", 78 },
  { "au" , 79 },
  { "AU" , 79 },
  { "Au", 79 },
  { "hg" , 80 },
  { "HG" , 80 },
  { "Hg", 80 },
  { "tl" , 81 },
  { "TL" , 81 },
  { "Tl", 81 },
  { "pb" , 82 },
  { "PB" , 82 },
  { "Pb", 82 },
  { "bi" , 83 },
  { "BI" , 83 },
  { "Bi", 83 },
  { "po" , 84 },
  { "PO" , 84 },
  { "Po", 84 },
  { "at" , 85 },
  { "AT" , 85 },
  { "At", 85 },
  { "rn" , 86 },
  { "RN" , 86 },
  { "Rn", 86 },
  { "fr" , 87 },
  { "FR" , 87 },
  { "Fr", 87 },
  { "ra" , 88 },
  { "RA" , 88 },
  { "Ra", 88 },
  { "ac" , 89 },
  { "AC" , 89 },
  { "Ac", 89 },
  { "th" , 90 },
  { "TH" , 90 },
  { "Th", 90 },
  { "pa" , 91 },
  { "PA" , 91 },
  { "Pa", 91 },
  { "u" , 92 },
  { "U" , 92 },
  { "np" , 93 },
  { "NP" , 93 },
  { "Np", 93 },
  { "pu" , 94 },
  { "PU" , 94 },
  { "Pu", 94 },
  { "am" , 95 },
  { "AM" , 95 },
  { "Am", 95 },
  { "cm" , 96 },
  { "CM" , 96 },
  { "Cm", 96 },
  { "bk" , 97 },
  { "BK" , 97 },
  { "Bk", 97 },
  { "cf" , 98 },
  { "CF" , 98 },
  { "Cf", 98 },
  { "es" , 99 },
  { "ES" , 99 },
  { "Es", 99 },
  { "fm" , 100 },
  { "FM" , 100 },
  { "Fm", 100 },
  { "md" , 101 },
  { "MD" , 101 },
  { "Md", 101 },
  { "no" , 102 },
  { "NO" , 102 },
  { "No", 102 },
  { "lr" , 103 },
  { "LR" , 103 },
  { "Lr", 103 },
  { "rf" , 104 },
  { "RF" , 104 },
  { "Rf", 104 },
  { "db" , 105 },
  { "DB" , 105 },
  { "Db", 105 },
  { "sg" , 106 },
  { "SG" , 106 },
  { "Sg", 106 },
  { "bh" , 107 },
  { "BH" , 107 },
  { "Bh", 107 },
  { "hs" , 108 },
  { "HS" , 108 },
  { "Hs", 108 },
  { "mt" , 109 },
  { "MT" , 109 },
  { "Mt", 109 },
  { "ds" , 110 },
  { "DS" , 110 },
  { "Ds", 110 },
  { "rg" , 111 },
  { "RG" , 111 },
  { "Rg", 111 },
  { "cn" , 112 },
  { "CN" , 112 },
  { "Cn", 112 },
  { "nh" , 113 },
  { "NH" , 113 },
  { "Nh", 113 },
  { "fl" , 114 },
  { "FL" , 114 },
  { "Fl", 114 },
  { "mc" , 115 },
  { "MC" , 115 },
  { "Mc", 115 },
  { "lv" , 116 },
  { "LV" , 116 },
  { "Lv", 116 },
  { "ts" , 117 },
  { "TS" , 117 },
  { "Ts", 117 },
  { "og" , 118 },
  { "OG" , 118 },
  { "Og", 118 },
  { "chg" , 999 },
  { "CHG" , 999 },
  { "dum" , 9999 },
  { "DUM" , 9999 },
}; // close sym2Z_


extern const std::unordered_map<std::size_t, AtomicInfo> atomic_data_{
  { 0,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    0,//Z
    "gho",//Atomic symbol
    "ghost",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            0,{0,0.0000000000000000,0.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 1,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    1,//Z
    "h",//Atomic symbol
    "hydrogen",//Full name
    2,//Multiplicity
    1.0079750000000001,//Mass
    0.5858150988919267,//Covalent radius
    2.2676713505493939,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            1,{1,1.0078250322000000,0.9998550000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            2,{2,2.0141017781000001,0.0001450000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 2,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    2,//Z
    "he",//Atomic symbol
    "helium",//Full name
    1,//Multiplicity
    4.0026020000000004,//Mass
    0.5291233151281919,//Covalent radius
    2.6456165756409593,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            3,{3,3.0160293199999999,0.0000020000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            4,{4,4.0026032541000003,0.9999980000000001}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 3,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    3,//Z
    "li",//Atomic symbol
    "lithium",//Full name
    2,//Multiplicity
    6.9675000000000002,//Mass
    2.4188494405860199,//Covalent radius
    3.4393015483332472,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            6,{6,6.0151228870000004,0.0485000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            7,{7,7.0160034400000004,0.9515000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 4,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    4,//Z
    "be",//Atomic symbol
    "beryllium",//Full name
    1,//Multiplicity
    9.0121830999999997,//Mass
    1.8141370804395149,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            9,{9,9.0121830999999997,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 5,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    5,//Z
    "b",//Atomic symbol
    "boron",//Full name
    2,//Multiplicity
    10.8134999999999994,//Mass
    1.6062672066391539,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            10,{10,10.0129370000000009,0.1965000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            11,{11,11.0093049999999995,0.8035000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 6,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    6,//Z
    "c",//Atomic symbol
    "carbon",//Full name
    3,//Multiplicity
    12.0106000000000002,//Mass
    1.4361918553479494,//Covalent radius
    3.2125344132783078,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            12,{12,12.0000000000000000,0.9893999999999999}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            13,{13,13.0033548349999997,0.0106000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 7,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    7,//Z
    "n",//Atomic symbol
    "nitrogen",//Full name
    4,//Multiplicity
    14.0068549999999998,//Mass
    1.3417055490750580,//Covalent radius
    2.9290754944596338,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            14,{14,14.0030740040000001,0.9962050000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            15,{15,15.0001088990000007,0.0037950000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 8,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    8,//Z
    "o",//Atomic symbol
    "oxygen",//Full name
    3,//Multiplicity
    15.9993999999999996,//Mass
    1.2472192428021665,//Covalent radius
    2.8723837106958987,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            16,{16,15.9949146199999994,0.9975700000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            17,{17,16.9991317570000007,0.0003835000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            18,{18,17.9991596129999998,0.0020450000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 9,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    9,//Z
    "f",//Atomic symbol
    "fluorine",//Full name
    2,//Multiplicity
    18.9984031629999990,//Mass
    1.0771438915109621,//Covalent radius
    2.7778974044230074,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            19,{19,18.9984031629999990,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 10,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    10,//Z
    "ne",//Atomic symbol
    "neon",//Full name
    1,//Multiplicity
    20.1797000000000004,//Mass
    1.0960411527655403,//Covalent radius
    2.9101782332050554,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            20,{20,19.9924401799999991,0.9048000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            21,{21,20.9938466999999989,0.0027000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            22,{22,21.9913850999999987,0.0925000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 11,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    11,//Z
    "na",//Atomic symbol
    "sodium",//Full name
    2,//Multiplicity
    22.9897692800000009,//Mass
    3.1369453682599948,//Covalent radius
    4.2896783047892697,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            23,{23,22.9897692800000009,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 12,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    12,//Z
    "mg",//Atomic symbol
    "magnesium",//Full name
    1,//Multiplicity
    24.3054999999999986,//Mass
    2.6645138368955377,//Covalent radius
    3.2692261970420424,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            24,{24,23.9850417000000000,0.7896500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            25,{25,24.9858370000000001,0.1001100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            26,{26,25.9825930000000014,0.1102500000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 13,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    13,//Z
    "al",//Atomic symbol
    "aluminum",//Full name
    2,//Multiplicity
    26.9815384999999992,//Mass
    2.2865686118039719,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            27,{27,26.9815384999999992,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 14,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    14,//Z
    "si",//Atomic symbol
    "silicon",//Full name
    3,//Multiplicity
    28.0850000000000009,//Mass
    2.0975959992581892,//Covalent radius
    3.9684248634614390,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            28,{28,27.9769265350000005,0.9225449999999999}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            29,{29,28.9764946650000006,0.0467200000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            30,{30,29.9737700099999991,0.0307350000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 15,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    15,//Z
    "p",//Atomic symbol
    "phosphorus",//Full name
    4,//Multiplicity
    30.9737619980000005,//Mass
    2.0220069542398762,//Covalent radius
    3.4015070258240905,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            31,{31,30.9737619980000005,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 16,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    16,//Z
    "s",//Atomic symbol
    "sulfur",//Full name
    3,//Multiplicity
    32.0675000000000026,//Mass
    1.9842124317307195,//Covalent radius
    3.4015070258240905,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            32,{32,31.9720711739999999,0.9485000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            33,{33,32.9714589100000026,0.0076300000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            34,{34,33.9678669999999983,0.0436500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            36,{36,35.9670810000000003,0.0001580000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 17,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    17,//Z
    "cl",//Atomic symbol
    "chlorine",//Full name
    2,//Multiplicity
    35.4515000000000029,//Mass
    1.9275206479669846,//Covalent radius
    3.3070207195511991,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            35,{35,34.9688526999999993,0.7580000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            37,{37,36.9659025999999997,0.2420000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 18,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    18,//Z
    "ar",//Atomic symbol
    "argon",//Full name
    1,//Multiplicity
    39.9480000000000004,//Mass
    2.0031096929852978,//Covalent radius
    3.5526851158607169,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            36,{36,35.9675451000000024,0.0033360000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            38,{38,37.9627320000000026,0.0006290000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            40,{40,39.9623831199999984,0.9960350000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 19,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    19,//Z
    "k",//Atomic symbol
    "potassium",//Full name
    2,//Multiplicity
    39.0983000000000018,//Mass
    3.8361440346793909,//Covalent radius
    5.1967468450090273,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            39,{39,38.9637064899999999,0.9325810000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            40,{40,39.9639981999999989,0.0001170000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            41,{41,40.9618252599999977,0.0673020000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 20,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    20,//Z
    "ca",//Atomic symbol
    "calcium",//Full name
    1,//Multiplicity
    40.0780000000000030,//Mass
    3.3259179808057775,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            40,{40,39.9625909000000021,0.9694100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            42,{42,41.9586180000000013,0.0064700000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            43,{43,42.9587659999999971,0.0013500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            44,{44,43.9554820000000035,0.0208600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            46,{46,45.9536900000000017,0.0000400000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            48,{48,47.9525227999999970,0.0018700000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 21,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    21,//Z
    "sc",//Atomic symbol
    "scandium",//Full name
    2,//Multiplicity
    44.9559080000000009,//Mass
    3.2125344132783078,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            45,{45,44.9559080000000009,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 22,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    22,//Z
    "ti",//Atomic symbol
    "titanium",//Full name
    3,//Multiplicity
    47.8669999999999973,//Mass
    3.0235618007325251,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            46,{46,45.9526279999999971,0.0825000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            47,{47,46.9517590000000027,0.0744000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            48,{48,47.9479419999999976,0.7372000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            49,{49,48.9478659999999977,0.0541000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            50,{50,49.9447869999999980,0.0518000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 23,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    23,//Z
    "v",//Atomic symbol
    "vanadium",//Full name
    4,//Multiplicity
    50.9414999999999978,//Mass
    2.8912809719504771,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            50,{50,49.9471559999999997,0.0025000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            51,{51,50.9439569999999975,0.9975000000000001}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 24,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    24,//Z
    "cr",//Atomic symbol
    "chromium",//Full name
    7,//Multiplicity
    51.9960999999999984,//Mass
    2.6267193143863810,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            50,{50,49.9460419999999985,0.0434500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            52,{52,51.9405059999999992,0.8378900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            53,{53,52.9406480000000030,0.0950100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            54,{54,53.9388790000000000,0.0236500000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 25,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    25,//Z
    "mn",//Atomic symbol
    "manganese",//Full name
    6,//Multiplicity
    54.9380439999999979,//Mass
    2.6267193143863810,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            55,{55,54.9380439999999979,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 26,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    26,//Z
    "fe",//Atomic symbol
    "iron",//Full name
    5,//Multiplicity
    55.8449999999999989,//Mass
    2.4944384856043329,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            54,{54,53.9396089999999973,0.0584500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            56,{56,55.9349360000000004,0.9175400000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            57,{57,56.9353929999999977,0.0211900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            58,{58,57.9332739999999973,0.0028200000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 27,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    27,//Z
    "co",//Atomic symbol
    "cobalt",//Full name
    4,//Multiplicity
    58.9331940000000003,//Mass
    2.3810549180768632,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            59,{59,58.9331940000000003,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 28,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    28,//Z
    "ni",//Atomic symbol
    "nickel",//Full name
    3,//Multiplicity
    58.6933999999999969,//Mass
    2.3432603955677069,//Covalent radius
    3.0802535844962597,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            58,{58,57.9353419999999986,0.6807690000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            60,{60,59.9307859999999977,0.2622310000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            61,{61,60.9310559999999981,0.0113990000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            62,{62,61.9283450000000002,0.0363450000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            64,{64,63.9279670000000024,0.0092560000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 29,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    29,//Z
    "cu",//Atomic symbol
    "copper",//Full name
    2,//Multiplicity
    63.5459999999999994,//Mass
    2.4944384856043329,//Covalent radius
    2.6456165756409593,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            63,{63,62.9295979999999986,0.6915000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            65,{65,64.9277900000000017,0.3085000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 30,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    30,//Z
    "zn",//Atomic symbol
    "zinc",//Full name
    1,//Multiplicity
    65.3799999999999955,//Mass
    2.3054658730585502,//Covalent radius
    2.6267193143863810,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            64,{64,63.9291419999999988,0.4917000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            66,{66,65.9260340000000014,0.2773000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            67,{67,66.9271279999999962,0.0404000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            68,{68,67.9248450000000048,0.1845000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            70,{70,69.9253199999999993,0.0061000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 31,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    31,//Z
    "ga",//Atomic symbol
    "gallium",//Full name
    2,//Multiplicity
    69.7229999999999990,//Mass
    2.3054658730585502,//Covalent radius
    3.5337878546061385,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            69,{69,68.9255739999999975,0.6010799999999999}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            71,{71,70.9247029999999938,0.3989200000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 32,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    32,//Z
    "ge",//Atomic symbol
    "germanium",//Full name
    3,//Multiplicity
    72.6299999999999955,//Mass
    2.2676713505493939,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            70,{70,69.9242490000000032,0.2052000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            72,{72,71.9220758000000018,0.2745000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            73,{73,72.9234589999999940,0.0776000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            74,{74,73.9211777600000062,0.3652000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            76,{76,75.9214027000000016,0.0775000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 33,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    33,//Z
    "as",//Atomic symbol
    "arsenic",//Full name
    4,//Multiplicity
    74.9215949999999964,//Mass
    2.2487740892948156,//Covalent radius
    3.4959933320969818,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            75,{75,74.9215949999999964,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 34,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    34,//Z
    "se",//Atomic symbol
    "selenium",//Full name
    3,//Multiplicity
    78.9710000000000036,//Mass
    2.2676713505493939,//Covalent radius
    3.5904796383698736,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            74,{74,73.9224758999999949,0.0086000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            76,{76,75.9192137000000002,0.0923000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            77,{77,76.9199141999999938,0.0760000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            78,{78,77.9173090000000030,0.2369000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            80,{80,79.9165220000000005,0.4980000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            82,{82,81.9167000000000058,0.0882000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 35,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    35,//Z
    "br",//Atomic symbol
    "bromine",//Full name
    2,//Multiplicity
    79.9039999999999964,//Mass
    2.2676713505493939,//Covalent radius
    3.4959933320969818,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            79,{79,78.9183380000000056,0.5065000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            81,{81,80.9162900000000036,0.4935000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 36,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    36,//Z
    "kr",//Atomic symbol
    "krypton",//Full name
    1,//Multiplicity
    83.7980000000000018,//Mass
    2.1920823055310805,//Covalent radius
    3.8172467734248130,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            78,{78,77.9203650000000039,0.0035500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            80,{80,79.9163779999999946,0.0228600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            82,{82,81.9134829999999994,0.1159300000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            83,{83,82.9141269999999935,0.1150000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            84,{84,83.9114977299999936,0.5698700000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            86,{86,85.9106106299999936,0.1727900000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 37,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    37,//Z
    "rb",//Atomic symbol
    "rubidium",//Full name
    2,//Multiplicity
    85.4677999999999969,//Mass
    4.1573974760072216,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            85,{85,84.9117897400000032,0.7217000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            87,{87,86.9091805300000004,0.2783000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 38,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    38,//Z
    "sr",//Atomic symbol
    "strontium",//Full name
    1,//Multiplicity
    87.6200000000000045,//Mass
    3.6849659446427649,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            84,{84,83.9134190000000046,0.0056000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            86,{86,85.9092610000000008,0.0986000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            87,{87,86.9088780000000014,0.0700000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            88,{88,87.9056130000000024,0.8258000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 39,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    39,//Z
    "y",//Atomic symbol
    "yttrium",//Full name
    2,//Multiplicity
    88.9058399999999978,//Mass
    3.5904796383698736,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            89,{89,88.9058399999999978,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 40,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    40,//Z
    "zr",//Atomic symbol
    "zirconium",//Full name
    3,//Multiplicity
    91.2240000000000038,//Mass
    3.3070207195511991,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            90,{90,89.9047000000000054,0.5145000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            91,{91,90.9056400000000053,0.1122000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            92,{92,91.9050299999999964,0.1715000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            94,{94,93.9063100000000048,0.1738000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            96,{96,95.9082700000000017,0.0280000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 41,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    41,//Z
    "nb",//Atomic symbol
    "niobium",//Full name
    6,//Multiplicity
    92.9063699999999955,//Mass
    3.0991508457508381,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            93,{93,92.9063699999999955,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 42,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    42,//Z
    "mo",//Atomic symbol
    "molybdenum",//Full name
    7,//Multiplicity
    95.9500000000000028,//Mass
    2.9101782332050554,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            92,{92,91.9068079999999981,0.1464900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            94,{94,93.9050849999999997,0.0918700000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            95,{95,94.9058390000000003,0.1587300000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            96,{96,95.9046759999999949,0.1667300000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            97,{97,96.9060180000000031,0.0958200000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            98,{98,97.9054050000000018,0.2429200000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            100,{100,99.9074719999999985,0.0974400000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 43,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    43,//Z
    "tc",//Atomic symbol
    "technetium",//Full name
    6,//Multiplicity
    0.0000000000000000,//Mass
    2.7778974044230074,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            98,{98,97.9072100000000063,0.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 44,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    44,//Z
    "ru",//Atomic symbol
    "ruthenium",//Full name
    5,//Multiplicity
    101.0699999999999932,//Mass
    2.7590001431684290,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            96,{96,95.9075899999999990,0.0554000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            98,{98,97.9052899999999937,0.0187000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            99,{99,98.9059340000000020,0.1276000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            100,{100,99.9042139999999961,0.1260000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            101,{101,100.9055769999999939,0.1706000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            102,{102,101.9043439999999947,0.3155000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            104,{104,103.9054299999999955,0.1862000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 45,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    45,//Z
    "rh",//Atomic symbol
    "rhodium",//Full name
    4,//Multiplicity
    102.9055000000000035,//Mass
    2.6834110981501160,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            103,{103,102.9055000000000035,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 46,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    46,//Z
    "pd",//Atomic symbol
    "palladium",//Full name
    1,//Multiplicity
    106.4200000000000017,//Mass
    2.6267193143863810,//Covalent radius
    3.0802535844962597,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            102,{102,101.9056000000000068,0.0102000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            104,{104,103.9040310000000034,0.1114000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            105,{105,104.9050799999999981,0.2233000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            106,{106,105.9034800000000018,0.2733000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            108,{108,107.9038919999999990,0.2646000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            110,{110,109.9051719999999932,0.1172000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 47,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    47,//Z
    "ag",//Atomic symbol
    "silver",//Full name
    2,//Multiplicity
    107.8682000000000016,//Mass
    2.7401028819138507,//Covalent radius
    3.2503289357874645,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            107,{107,106.9050900000000013,0.5183900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            109,{109,108.9047549999999944,0.4816100000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 48,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    48,//Z
    "cd",//Atomic symbol
    "cadmium",//Full name
    1,//Multiplicity
    112.4140000000000015,//Mass
    2.7212056206592723,//Covalent radius
    2.9857672782233684,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            106,{106,105.9064599999999956,0.0124500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            108,{108,107.9041830000000033,0.0088800000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            110,{110,109.9030070000000023,0.1247000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            111,{111,110.9041830000000033,0.1279500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            112,{112,111.9027629999999931,0.2410900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            113,{113,112.9044080000000037,0.1222700000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            114,{114,113.9033649999999938,0.2875400000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            116,{116,115.9047630000000026,0.0751200000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 49,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    49,//Z
    "in",//Atomic symbol
    "indium",//Full name
    2,//Multiplicity
    114.8179999999999978,//Mass
    2.6834110981501160,//Covalent radius
    3.6471714221336082,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            113,{113,112.9040619999999961,0.0428100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            115,{115,114.9038787799999994,0.9571900000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 50,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    50,//Z
    "sn",//Atomic symbol
    "tin",//Full name
    3,//Multiplicity
    118.7099999999999937,//Mass
    2.6267193143863810,//Covalent radius
    4.1007056922434870,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            112,{112,111.9048240000000050,0.0097000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            114,{114,113.9027829999999994,0.0066000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            115,{115,114.9033447000000052,0.0034000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            116,{116,115.9017429999999962,0.1454000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            117,{117,116.9029539999999940,0.0768000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            118,{118,117.9016069999999985,0.2422000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            119,{119,118.9033110000000022,0.0859000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            120,{120,119.9022020000000026,0.3258000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            122,{122,121.9034400000000034,0.0463000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            124,{124,123.9052769999999981,0.0579000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 51,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    51,//Z
    "sb",//Atomic symbol
    "antimony",//Full name
    4,//Multiplicity
    121.7600000000000051,//Mass
    2.6267193143863810,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            121,{121,120.9038099999999929,0.5721000000000001}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            123,{123,122.9042100000000062,0.4279000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 52,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    52,//Z
    "te",//Atomic symbol
    "tellurium",//Full name
    3,//Multiplicity
    127.5999999999999943,//Mass
    2.6078220531318026,//Covalent radius
    3.8928358184431260,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            120,{120,119.9040600000000012,0.0009000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            122,{122,121.9030400000000043,0.0255000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            123,{123,122.9042699999999968,0.0089000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            124,{124,123.9028200000000055,0.0474000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            125,{125,124.9044300000000050,0.0707000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            126,{126,125.9033100000000047,0.1884000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            128,{128,127.9044609999999977,0.3174000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            130,{130,129.9062227500000120,0.3408000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 53,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    53,//Z
    "i",//Atomic symbol
    "iodine",//Full name
    2,//Multiplicity
    126.9044700000000034,//Mass
    2.6267193143863810,//Covalent radius
    3.7416577284064996,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            127,{127,126.9044700000000034,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 54,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    54,//Z
    "xe",//Atomic symbol
    "xenon",//Full name
    1,//Multiplicity
    131.2930000000000064,//Mass
    2.6456165756409593,//Covalent radius
    4.0818084309889091,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            124,{124,123.9058899999999994,0.0009500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            126,{126,125.9043000000000063,0.0008900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            128,{128,127.9035310000000010,0.0191000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            129,{129,128.9047808599999883,0.2640100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            130,{130,129.9035093999999901,0.0407100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            131,{131,130.9050839999999880,0.2123200000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            132,{132,131.9041550899999891,0.2690900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            134,{134,133.9053949999999986,0.1043600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            136,{136,135.9072144799999933,0.0885700000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 55,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    55,//Z
    "cs",//Atomic symbol
    "cesium",//Full name
    2,//Multiplicity
    132.9054519599999935,//Mass
    4.6109317461171004,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            133,{133,132.9054519599999935,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 56,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    56,//Z
    "ba",//Atomic symbol
    "barium",//Full name
    1,//Multiplicity
    137.3269999999999982,//Mass
    4.0629111697343303,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            130,{130,129.9063199999999938,0.0011000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            132,{132,131.9050609999999892,0.0010000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            134,{134,133.9045079999999928,0.0242000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            135,{135,134.9056879999999978,0.0659000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            136,{136,135.9045759999999916,0.0785000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            137,{137,136.9058269999999879,0.1123000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            138,{138,137.9052470000000028,0.7170000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 57,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    57,//Z
    "la",//Atomic symbol
    "lanthanum",//Full name
    2,//Multiplicity
    138.9054700000000082,//Mass
    3.9117330796977043,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            138,{138,137.9071199999999919,0.0008881000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            139,{139,138.9063600000000065,0.9991119000000001}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 58,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    58,//Z
    "ce",//Atomic symbol
    "cerium",//Full name
    1,//Multiplicity
    140.1160000000000139,//Mass
    3.8550412959339693,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            136,{136,135.9071289999999976,0.0018600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            138,{138,137.9059900000000027,0.0025100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            140,{140,139.9054399999999987,0.8844900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            142,{142,141.9092499999999859,0.1111400000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 59,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    59,//Z
    "pr",//Atomic symbol
    "praseodymium",//Full name
    4,//Multiplicity
    140.9076599999999928,//Mass
    3.8361440346793909,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            141,{141,140.9076599999999928,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 60,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    60,//Z
    "nd",//Atomic symbol
    "neodymium",//Full name
    5,//Multiplicity
    144.2419999999999902,//Mass
    3.7983495121702346,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            142,{142,141.9077299999999866,0.2715300000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            143,{143,142.9098199999999963,0.1217300000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            144,{144,143.9100899999999967,0.2379800000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            145,{145,144.9125799999999913,0.0829300000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            146,{146,145.9131199999999922,0.1718900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            148,{148,147.9168999999999983,0.0575600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            150,{150,149.9208999999999889,0.0563800000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 61,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    61,//Z
    "pm",//Atomic symbol
    "promethium",//Full name
    6,//Multiplicity
    0.0000000000000000,//Mass
    3.7605549896610779,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            145,{145,144.9127599999999916,0.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 62,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    62,//Z
    "sm",//Atomic symbol
    "samarium",//Full name
    7,//Multiplicity
    150.3600000000000136,//Mass
    3.7416577284064996,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            144,{144,143.9120100000000093,0.0308000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            147,{147,146.9148999999999887,0.1500000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            148,{148,147.9148299999999949,0.1125000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            149,{149,148.9171900000000051,0.1382000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            150,{150,149.9172800000000052,0.0737000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            152,{152,151.9197399999999902,0.2674000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            154,{154,153.9222200000000100,0.2274000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 63,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    63,//Z
    "eu",//Atomic symbol
    "europium",//Full name
    8,//Multiplicity
    151.9639999999999986,//Mass
    3.7416577284064996,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            151,{151,150.9198599999999999,0.4781000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            153,{153,152.9212400000000116,0.5219000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 64,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    64,//Z
    "gd",//Atomic symbol
    "gadolinium",//Full name
    9,//Multiplicity
    157.2500000000000000,//Mass
    3.7038632058973433,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            152,{152,151.9198000000000093,0.0020000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            154,{154,153.9208700000000078,0.0218000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            155,{155,154.9226299999999981,0.1480000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            156,{156,155.9221300000000099,0.2047000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            157,{157,156.9239699999999971,0.1565000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            158,{158,157.9241100000000131,0.2484000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            160,{160,159.9270600000000115,0.2186000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 65,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    65,//Z
    "tb",//Atomic symbol
    "terbium",//Full name
    6,//Multiplicity
    158.9253500000000088,//Mass
    3.6660686833881866,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            159,{159,158.9253500000000088,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 66,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    66,//Z
    "dy",//Atomic symbol
    "dysprosium",//Full name
    5,//Multiplicity
    162.5000000000000000,//Mass
    3.6282741608790299,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            156,{156,155.9242800000000102,0.0005600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            158,{158,157.9244199999999978,0.0009500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            160,{160,159.9251999999999896,0.0232900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            161,{161,160.9269400000000019,0.1888900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            162,{162,161.9268099999999890,0.2547500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            163,{163,162.9287400000000048,0.2489600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            164,{164,163.9291800000000023,0.2826000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 67,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    67,//Z
    "ho",//Atomic symbol
    "holmium",//Full name
    4,//Multiplicity
    164.9303299999999979,//Mass
    3.6282741608790299,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            165,{165,164.9303299999999979,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 68,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    68,//Z
    "er",//Atomic symbol
    "erbium",//Full name
    3,//Multiplicity
    167.2589999999999861,//Mass
    3.5715823771152952,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            162,{162,161.9287899999999922,0.0013900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            164,{164,163.9292100000000119,0.0160100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            166,{166,165.9302999999999884,0.3350300000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            167,{167,166.9320500000000038,0.2286900000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            168,{168,167.9323799999999949,0.2697800000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            170,{170,169.9354700000000093,0.1491000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 69,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    69,//Z
    "tm",//Atomic symbol
    "thulium",//Full name
    2,//Multiplicity
    168.9342200000000105,//Mass
    3.5904796383698736,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            169,{169,168.9342200000000105,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 70,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    70,//Z
    "yb",//Atomic symbol
    "ytterbium",//Full name
    1,//Multiplicity
    173.0449999999999875,//Mass
    3.5337878546061385,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            168,{168,167.9338899999999910,0.0012600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            170,{170,169.9347699999999861,0.0302300000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            171,{171,170.9363299999999981,0.1421600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            172,{172,171.9363899999999887,0.2175400000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            173,{173,172.9382200000000012,0.1609800000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            174,{174,173.9388700000000085,0.3189600000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            176,{176,175.9425799999999924,0.1288700000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 71,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    71,//Z
    "lu",//Atomic symbol
    "lutetium",//Full name
    2,//Multiplicity
    174.9668000000000063,//Mass
    3.5337878546061385,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            175,{175,174.9407799999999895,0.9740100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            176,{176,175.9426899999999989,0.0259900000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 72,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    72,//Z
    "hf",//Atomic symbol
    "hafnium",//Full name
    3,//Multiplicity
    178.4900000000000091,//Mass
    3.3070207195511991,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            174,{174,173.9400500000000136,0.0016000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            176,{176,175.9414099999999905,0.0526000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            177,{177,176.9432299999999998,0.1860000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            178,{178,177.9437100000000100,0.2728000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            179,{179,178.9458199999999977,0.1362000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            180,{180,179.9465600000000052,0.3508000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 73,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    73,//Z
    "ta",//Atomic symbol
    "tantalum",//Full name
    4,//Multiplicity
    180.9478799999999978,//Mass
    3.2125344132783078,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            180,{180,179.9474600000000066,0.0001201000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            181,{181,180.9480000000000075,0.9998799000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 74,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    74,//Z
    "w",//Atomic symbol
    "tungsten",//Full name
    5,//Multiplicity
    183.8400000000000034,//Mass
    3.0613563232416814,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            180,{180,179.9467099999999959,0.0012000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            182,{182,181.9482040000000040,0.2650000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            183,{183,182.9502229999999940,0.1431000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            184,{184,183.9509309999999971,0.3064000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            186,{186,185.9543600000000083,0.2843000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 75,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    75,//Z
    "re",//Atomic symbol
    "rhenium",//Full name
    6,//Multiplicity
    186.2069999999999936,//Mass
    2.8534864494413203,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            185,{185,184.9529550000000029,0.3740000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            187,{187,186.9557499999999948,0.6260000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 76,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    76,//Z
    "os",//Atomic symbol
    "osmium",//Full name
    5,//Multiplicity
    190.2299999999999898,//Mass
    2.7212056206592723,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            184,{184,183.9524890000000141,0.0002000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            186,{186,185.9538400000000138,0.0159000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            187,{187,186.9557499999999948,0.0196000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            188,{188,187.9558399999999949,0.1324000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            189,{189,188.9581399999999860,0.1615000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            190,{190,189.9584399999999960,0.2626000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            192,{192,191.9614799999999946,0.4078000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 77,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    77,//Z
    "ir",//Atomic symbol
    "iridium",//Full name
    4,//Multiplicity
    192.2170000000000130,//Mass
    2.6645138368955377,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            191,{191,190.9605899999999963,0.3730000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            193,{193,192.9629199999999969,0.6270000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 78,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    78,//Z
    "pt",//Atomic symbol
    "platinum",//Full name
    3,//Multiplicity
    195.0840000000000032,//Mass
    2.5700275306226463,//Covalent radius
    3.3070207195511991,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            190,{190,189.9599300000000142,0.0001200000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            192,{192,191.9610399999999970,0.0078200000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            194,{194,193.9626810000000035,0.3286400000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            195,{195,194.9647919999999885,0.3377500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            196,{196,195.9649520000000109,0.2521100000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            198,{198,197.9678900000000112,0.0735600000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 79,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    79,//Z
    "au",//Atomic symbol
    "gold",//Full name
    2,//Multiplicity
    196.9665689999999927,//Mass
    2.5700275306226463,//Covalent radius
    3.1369453682599948,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            197,{197,196.9665689999999927,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 80,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    80,//Z
    "hg",//Atomic symbol
    "mercury",//Full name
    1,//Multiplicity
    200.5920000000000130,//Mass
    2.4944384856043329,//Covalent radius
    2.9290754944596338,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            196,{196,195.9658300000000111,0.0015000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            198,{198,197.9667689999999993,0.1004000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            199,{199,198.9682809999999904,0.1694000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            200,{200,199.9683269999999879,0.2314000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            201,{201,200.9703030000000012,0.1317000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            202,{202,201.9706429999999955,0.2974000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            204,{204,203.9734939999999881,0.0682000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 81,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    81,//Z
    "tl",//Atomic symbol
    "thallium",//Full name
    2,//Multiplicity
    204.3834999999999980,//Mass
    2.7401028819138507,//Covalent radius
    3.7038632058973433,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            203,{203,202.9723449999999900,0.2951500000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            205,{205,204.9744279999999890,0.7048500000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 82,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    82,//Z
    "pb",//Atomic symbol
    "lead",//Full name
    3,//Multiplicity
    207.1999999999999886,//Mass
    2.7590001431684290,//Covalent radius
    3.8172467734248130,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            204,{204,203.9730439999999874,0.0140000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            206,{206,205.9744660000000067,0.2410000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            207,{207,206.9758970000000033,0.2210000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            208,{208,207.9766529999999989,0.5240000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 83,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    83,//Z
    "bi",//Atomic symbol
    "bismuth",//Full name
    4,//Multiplicity
    208.9804000000000030,//Mass
    2.7967946656775857,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            209,{209,208.9804000000000030,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 84,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    84,//Z
    "po",//Atomic symbol
    "polonium",//Full name
    3,//Multiplicity
    0.0000000000000000,//Mass
    2.6456165756409593,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 85,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    85,//Z
    "at",//Atomic symbol
    "astatine",//Full name
    2,//Multiplicity
    0.0000000000000000,//Mass
    2.8345891881867420,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 86,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    86,//Z
    "rn",//Atomic symbol
    "radon",//Full name
    1,//Multiplicity
    0.0000000000000000,//Mass
    2.8345891881867420,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 87,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    87,//Z
    "fr",//Atomic symbol
    "francium",//Full name
    2,//Multiplicity
    0.0000000000000000,//Mass
    4.9132879261903533,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 88,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    88,//Z
    "ra",//Atomic symbol
    "radium",//Full name
    1,//Multiplicity
    0.0000000000000000,//Mass
    4.1762947372618004,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 89,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    89,//Z
    "ac",//Atomic symbol
    "actinium",//Full name
    2,//Multiplicity
    0.0000000000000000,//Mass
    4.0629111697343303,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 90,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    90,//Z
    "th",//Atomic symbol
    "thorium",//Full name
    3,//Multiplicity
    232.0377000000000010,//Mass
    3.8928358184431260,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            230,{230,230.0331299999999999,0.0002000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            232,{232,232.0380600000000015,0.9998000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 91,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    91,//Z
    "pa",//Atomic symbol
    "protactinium",//Full name
    4,//Multiplicity
    231.0358799999999917,//Mass
    3.7794522509156563,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            231,{231,231.0358799999999917,1.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 92,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    92,//Z
    "u",//Atomic symbol
    "uranium",//Full name
    5,//Multiplicity
    238.0289099999999962,//Mass
    3.7038632058973433,//Covalent radius
    3.5148905933515602,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            233,{233,233.0396399999999915,0.0000000000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            234,{234,234.0409500000000094,0.0000540000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            235,{235,235.0439299999999889,0.0072040000000000}
        },//End pair<int,IsotopeData
        {//Start pair<int,IsotopeData
            238,{238,238.0507900000000063,0.9927420000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 93,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    93,//Z
    "np",//Atomic symbol
    "neptunium",//Full name
    6,//Multiplicity
    0.0000000000000000,//Mass
    3.5904796383698736,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 94,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    94,//Z
    "pu",//Atomic symbol
    "plutonium",//Full name
    7,//Multiplicity
    0.0000000000000000,//Mass
    3.5337878546061385,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 95,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    95,//Z
    "am",//Atomic symbol
    "americium",//Full name
    8,//Multiplicity
    0.0000000000000000,//Mass
    3.4015070258240905,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 96,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    96,//Z
    "cm",//Atomic symbol
    "curium",//Full name
    9,//Multiplicity
    0.0000000000000000,//Mass
    3.1936371520237294,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 97,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    97,//Z
    "bk",//Atomic symbol
    "berkelium",//Full name
    6,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 98,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    98,//Z
    "cf",//Atomic symbol
    "californium",//Full name
    5,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 99,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    99,//Z
    "es",//Atomic symbol
    "einsteinium",//Full name
    4,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 100,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    100,//Z
    "fm",//Atomic symbol
    "fermium",//Full name
    3,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 101,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    101,//Z
    "md",//Atomic symbol
    "mendelevium",//Full name
    2,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 102,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    102,//Z
    "no",//Atomic symbol
    "nobelium",//Full name
    1,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 103,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    103,//Z
    "lr",//Atomic symbol
    "lawrencium",//Full name
    2,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 104,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    104,//Z
    "rf",//Atomic symbol
    "rutherfordium",//Full name
    3,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 105,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    105,//Z
    "db",//Atomic symbol
    "dubnium",//Full name
    4,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 106,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    106,//Z
    "sg",//Atomic symbol
    "seaborgium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 107,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    107,//Z
    "bh",//Atomic symbol
    "bohrium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 108,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    108,//Z
    "hs",//Atomic symbol
    "hassium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 109,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    109,//Z
    "mt",//Atomic symbol
    "meitnerium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 110,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    110,//Z
    "ds",//Atomic symbol
    "darmstadtium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 111,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    111,//Z
    "rg",//Atomic symbol
    "roentgenium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 112,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    112,//Z
    "cn",//Atomic symbol
    "copernicium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 113,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    113,//Z
    "nh",//Atomic symbol
    "nihonium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 114,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    114,//Z
    "fl",//Atomic symbol
    "flerovium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 115,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    115,//Z
    "mc",//Atomic symbol
    "moscovium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 116,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    116,//Z
    "lv",//Atomic symbol
    "livermorium",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 117,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    117,//Z
    "ts",//Atomic symbol
    "tennessine",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 118,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    118,//Z
    "og",//Atomic symbol
    "oganesson",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 999,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    999,//Z
    "chg",//Atomic symbol
    "charge",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            999,{999,0.0000000000000000,0.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
  { 9999,//Start pair<int,AtomicInfo>
{//Start AtomicInfo instance
    9999,//Z
    "dum",//Atomic symbol
    "dummy",//Full name
    0,//Multiplicity
    0.0000000000000000,//Mass
    0.0000000000000000,//Covalent radius
    0.0000000000000000,//VDW radius
    {//Start map<int,IsotopeData>
        {//Start pair<int,IsotopeData
            9999,{9999,0.0000000000000000,0.0000000000000000}
        },//End pair<int,IsotopeData
    }//End map<int,IsotopeData>
}//End AtomicInfo
  },//End pair<int,AtomicInfo>
};
}}//End namespaces
