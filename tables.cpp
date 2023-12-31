/*Conatains code for the TABLES defined across whole assembler*/
#include<iostream>
#include<map>
#include<string>

using namespace std;

struct opcode{
    string op;
    int format;
    char exists;
    opcode(){
      op="undefined";
      format=0;
      exists='n';
    }
};
struct literal{
    string value;
    string address;
    char exists;
    int blockNumber = 0;
    literal(){
      value="";
      address="?";
      blockNumber = 0;
      exists='n';
    }
};
struct label{
     string address;
     string name;
     int relative;
     int blockNumber;
     char exists;
     label(){
       name="undefined";
       address="0";
       blockNumber = 0;
       exists='n';
       relative = 0;
     }
};
struct blocks{
     string startAddress;
     string name;
     string LOCCTR;
     int number;
     char exists;
     blocks(){
       name="undefined";
       startAddress="?";
       exists='n';
       number = -1;
       LOCCTR = "0";
     }
};
struct REG{
     char num;
     char exists;
     REG(){
       num = 'F';
       exists='n';
     }
};

typedef map<string,label> SYMBOL_TABLE_TYPE;
SYMBOL_TABLE_TYPE SYMTAB;

typedef map<string,opcode> OPCODE_TABLE_TYPE;
OPCODE_TABLE_TYPE OPTAB;

typedef map<string,REG> REG_TABLE_TYPE;
REG_TABLE_TYPE REGTAB;

typedef map<string,literal> LIT_TABLE_TYPE;
LIT_TABLE_TYPE LITTAB;

typedef map<string,blocks> BLOCK_TABLE_TYPE;
BLOCK_TABLE_TYPE BLOCKS;

void load_REGTAB(){
  REGTAB["A"].num='0';
  REGTAB["A"].exists='y';

  REGTAB["X"].num='1';
  REGTAB["X"].exists='y';

  REGTAB["L"].num='2';
  REGTAB["L"].exists='y';

  REGTAB["B"].num='3';
  REGTAB["B"].exists='y';

  REGTAB["S"].num='4';
  REGTAB["S"].exists='y';

  REGTAB["T"].num='5';
  REGTAB["T"].exists='y';

  REGTAB["F"].num='6';
  REGTAB["F"].exists='y';

  REGTAB["PC"].num='8';
  REGTAB["PC"].exists='y';

  REGTAB["SW"].num='9';
  REGTAB["SW"].exists='y';
}
void load_OPTAB(){
  OPTAB["ADD"].op="18";
  OPTAB["ADD"].format=3;
  OPTAB["ADD"].exists='y';

  OPTAB["ADDF"].op="58";
  OPTAB["ADDF"].format=3;
  OPTAB["ADDF"].exists='y';

  OPTAB["ADDR"].op="90";
  OPTAB["ADDR"].format=2;
  OPTAB["ADDR"].exists='y';

  OPTAB["AND"].op="40";
  OPTAB["AND"].format=3;
  OPTAB["AND"].exists='y';

  OPTAB["CLEAR"].op="B4";
  OPTAB["CLEAR"].format=2;
  OPTAB["CLEAR"].exists='y';

  OPTAB["COMP"].op="28";
  OPTAB["COMP"].format=3;
  OPTAB["COMP"].exists='y';

  OPTAB["COMPF"].op="88";
  OPTAB["COMPF"].format=3;
  OPTAB["COMPF"].exists='y';

  OPTAB["COMPR"].op="A0";
  OPTAB["COMPR"].format=2;
  OPTAB["COMPR"].exists='y';

  OPTAB["DIV"].op="24";
  OPTAB["DIV"].format=3;
  OPTAB["DIV"].exists='y';

  OPTAB["DIVF"].op="64";
  OPTAB["DIVF"].format=3;
  OPTAB["DIVF"].exists='y';

  OPTAB["DIVR"].op="9C";
  OPTAB["DIVR"].format=2;
  OPTAB["DIVR"].exists='y';

  OPTAB["FIX"].op="C4";
  OPTAB["FIX"].format=1;
  OPTAB["FIX"].exists='y';

  OPTAB["FLOAT"].op="C0";
  OPTAB["FLOAT"].format=1;
  OPTAB["FLOAT"].exists='y';

  OPTAB["HIO"].op="F4";
  OPTAB["HIO"].format=1;
  OPTAB["HIO"].exists='y';

  OPTAB["J"].op="3C";
  OPTAB["J"].format=3;
  OPTAB["J"].exists='y';

  OPTAB["JEQ"].op="30";
  OPTAB["JEQ"].format=3;
  OPTAB["JEQ"].exists='y';

  OPTAB["JGT"].op="34";
  OPTAB["JGT"].format=3;
  OPTAB["JGT"].exists='y';

  OPTAB["JLT"].op="38";
  OPTAB["JLT"].format=3;
  OPTAB["JLT"].exists='y';

  OPTAB["JSUB"].op="48";
  OPTAB["JSUB"].format=3;
  OPTAB["JSUB"].exists='y';

  OPTAB["LDA"].op="00";
  OPTAB["LDA"].format=3;
  OPTAB["LDA"].exists='y';

  OPTAB["LDB"].op="68";
  OPTAB["LDB"].format=3;
  OPTAB["LDB"].exists='y';

  OPTAB["LDCH"].op="50";
  OPTAB["LDCH"].format=3;
  OPTAB["LDCH"].exists='y';

  OPTAB["LDF"].op="70";
  OPTAB["LDF"].format=3;
  OPTAB["LDF"].exists='y';

  OPTAB["LDL"].op="08";
  OPTAB["LDL"].format=3;
  OPTAB["LDL"].exists='y';

  OPTAB["LDS"].op="6C";
  OPTAB["LDS"].format=3;
  OPTAB["LDS"].exists='y';

  OPTAB["LDT"].op="74";
  OPTAB["LDT"].format=3;
  OPTAB["LDT"].exists='y';

  OPTAB["LDX"].op="04";
  OPTAB["LDX"].format=3;
  OPTAB["LDX"].exists='y';

  OPTAB["LPS"].op="D0";
  OPTAB["LPS"].format=3;
  OPTAB["LPS"].exists='y';

  OPTAB["MUL"].op="20";
  OPTAB["MUL"].format=3;
  OPTAB["MUL"].exists='y';

  OPTAB["MULF"].op="60";
  OPTAB["MULF"].format=3;
  OPTAB["MULF"].exists='y';

  OPTAB["MULR"].op="98";
  OPTAB["MULR"].format=2;
  OPTAB["MULR"].exists='y';

  OPTAB["NORM"].op="C8";
  OPTAB["NORM"].format=1;
  OPTAB["NORM"].exists='y';

  OPTAB["OR"].op="44";
  OPTAB["OR"].format=3;
  OPTAB["OR"].exists='y';

  OPTAB["RD"].op="D8";
  OPTAB["RD"].format=3;
  OPTAB["RD"].exists='y';

  OPTAB["RMO"].op="AC";
  OPTAB["RMO"].format=2;
  OPTAB["RMO"].exists='y';

  OPTAB["RSUB"].op="4F";
  OPTAB["RSUB"].format=3;
  OPTAB["RSUB"].exists='y';

  OPTAB["SHIFTL"].op="A4";
  OPTAB["SHIFTL"].format=2;
  OPTAB["SHIFTL"].exists='y';

  OPTAB["SHIFTR"].op="A8";
  OPTAB["SHIFTR"].format=2;
  OPTAB["SHIFTR"].exists='y';

  OPTAB["SIO"].op="F0";
  OPTAB["SIO"].format=1;
  OPTAB["SIO"].exists='y';

  OPTAB["SSK"].op="EC";
  OPTAB["SSK"].format=3;
  OPTAB["SSK"].exists='y';

  OPTAB["STA"].op="0C";
  OPTAB["STA"].format=3;
  OPTAB["STA"].exists='y';

  OPTAB["STB"].op="78";
  OPTAB["STB"].format=3;
  OPTAB["STB"].exists='y';

  OPTAB["STCH"].op="54";
  OPTAB["STCH"].format=3;
  OPTAB["STCH"].exists='y';

  OPTAB["STF"].op="80";
  OPTAB["STF"].format=3;
  OPTAB["STF"].exists='y';

  OPTAB["STI"].op="D4";
  OPTAB["STI"].format=3;
  OPTAB["STI"].exists='y';

  OPTAB["STL"].op="14";
  OPTAB["STL"].format=3;
  OPTAB["STL"].exists='y';

  OPTAB["STS"].op="7C";
  OPTAB["STS"].format=3;
  OPTAB["STS"].exists='y';

  OPTAB["STSW"].op="E8";
  OPTAB["STSW"].format=3;
  OPTAB["STSW"].exists='y';

  OPTAB["STT"].op="84";
  OPTAB["STT"].format=3;
  OPTAB["STT"].exists='y';

  OPTAB["STX"].op="10";
  OPTAB["STX"].format=3;
  OPTAB["STX"].exists='y';

  OPTAB["SUB"].op="1C";
  OPTAB["SUB"].format=3;
  OPTAB["SUB"].exists='y';

  OPTAB["SUBF"].op="5C";
  OPTAB["SUBF"].format=3;
  OPTAB["SUBF"].exists='y';

  OPTAB["SUBR"].op="94";
  OPTAB["SUBR"].format=2;
  OPTAB["SUBR"].exists='y';

  OPTAB["SVC"].op="B0";
  OPTAB["SVC"].format=2;
  OPTAB["SVC"].exists='y';

  OPTAB["TD"].op="E0";
  OPTAB["TD"].format=3;
  OPTAB["TD"].exists='y';

  OPTAB["TIO"].op="F8";
  OPTAB["TIO"].format=1;
  OPTAB["TIO"].exists='y';

  OPTAB["TIX"].op="2C";
  OPTAB["TIX"].format=3;
  OPTAB["TIX"].exists='y';

  OPTAB["TIXR"].op="B8";
  OPTAB["TIXR"].format=2;
  OPTAB["TIXR"].exists='y';

  OPTAB["WD"].op="DC";
  OPTAB["WD"].format=3;
  OPTAB["WD"].exists='y';
}

void load_BLOCKS(){
  BLOCKS[""].exists = 'y';
  BLOCKS[""].name = "";
  BLOCKS[""].startAddress = "00000";
  BLOCKS[""].number=0;
  BLOCKS[""].LOCCTR = "0";
}
void load_tables(){
  load_BLOCKS();
  load_OPTAB();
  load_REGTAB();
}
