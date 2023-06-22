
#include "pass1.cpp"
using namespace std;

ifstream intermediateFile;
ofstream errorFile,objectFile,ListingFile;
bool isComment;
string label,op,operand,comment;
string operand1,operand2;
int line,blockNumber,address,startAddress;
string objectCode, writeData, currentRecord, modificationRecord, endRecord;
int program_counter, base_register_value, currentTextRecordLength;
bool nobase;

string readTillEND(string data,int& index){
  string temp_string = "";
  while(index<data.length() && data[index] != '\t'){
    temp_string += data[index];
    index++;
  }
  index++;
  return temp_string;
}
bool readIntermediateFile(ifstream& readFile,bool& isComment, int& line, int& address, int& blockNumber, string& label, string& op, string& operand, string& comment){
  string fileLine="";
  string temp_string="";
  bool tempStatus;
  int index=0;
  if(!getline(readFile, fileLine)){
    return false;
  }
  line = stoi(readTillEND(fileLine,index));

  isComment = (fileLine[index]=='.')?true:false;
  if(isComment){
    readFirstNonWhiteSpace(fileLine,index,tempStatus,comment,true);
    return true;
  }
  address = stringHexToInt(readTillEND(fileLine,index));
  temp_string = readTillEND(fileLine,index);
  if(temp_string == " "){
    blockNumber = -1;
  }
  else{
    blockNumber = stoi(temp_string);
  }
  label = readTillEND(fileLine,index);
  op = readTillEND(fileLine,index);
  if(op=="BYTE"){
    readByteOperand(fileLine,index,tempStatus,operand);
  }
  else{
    operand = readTillEND(fileLine,index);
  }
  readFirstNonWhiteSpace(fileLine,index,tempStatus,comment,true);
  return true;
}

string createObjectCodeFormat34(){
  string objcode;
  int halfBytes;
  halfBytes = (getFlagFormat(op)=='+')?5:3;

  if(getFlagFormat(operand)=='#'){
    if(operand.substr(operand.length()-2,2)==",X"){
      writeData = "Line: "+to_string(line)+" Index based addressing not supported with Indirect addressing";
      writeToFile(errorFile,writeData);
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+1,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }

    string tempOperand = operand.substr(1,operand.length()-1);
    if(if_all_num(tempOperand)||((SYMTAB[tempOperand].exists=='y')&&SYMTAB[tempOperand].relative==0)){
      int immediateValue;

      if(if_all_num(tempOperand)){
        immediateValue = stoi(tempOperand);
      }
      else{
        immediateValue = stringHexToInt(SYMTAB[tempOperand].address);
      }

      if(immediateValue>=(1<<4*halfBytes)){
        writeData = "Line: "+to_string(line)+" Immediate value exceeds format limit";
        writeToFile(errorFile,writeData);
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+1,2);
        objcode += (halfBytes==5)?"100000":"0000";
      }
      else{
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+1,2);
        objcode += (halfBytes==5)?'1':'0';
        objcode += intToStringHex(immediateValue,halfBytes);
      }
      return objcode;
    }
    else if(SYMTAB[tempOperand].exists=='n') {
      writeData = "Line "+to_string(line);
      writeData += " : Symbol doesn't exists. Found " + tempOperand;
      writeToFile(errorFile,writeData);
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+1,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }
    else{
      int operandAddress = stringHexToInt(SYMTAB[tempOperand].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[tempOperand].blockNumber]].startAddress);

      if(halfBytes==5){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+1,2);
        objcode += '1';
        objcode += intToStringHex(operandAddress,halfBytes);

        modificationRecord += "M^" + intToStringHex(address+1,6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }

      program_counter = address + stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress);
      program_counter += (halfBytes==5)?4:3;
      int relativeAddress = operandAddress - program_counter;

      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+1,2);
        objcode += '2';
        objcode += intToStringHex(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+1,2);
          objcode += '4';
          objcode += intToStringHex(relativeAddress,halfBytes);
          return objcode;
        }
      }

      if(operandAddress<=4095){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+1,2);
        objcode += '0';
        objcode += intToStringHex(operandAddress,halfBytes);

        modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
  }
  else if(getFlagFormat(operand)=='@'){
    string tempOperand = operand.substr(1,operand.length()-1);
    if(tempOperand.substr(tempOperand.length()-2,2)==",X" || SYMTAB[tempOperand].exists=='n'){
      writeData = "Line "+to_string(line);
      writeData += (SYMTAB[tempOperand].exists=='n')?": Symbol doesn't exists":" Index based addressing not supported with Indirect addressing";
      writeToFile(errorFile,writeData);
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+2,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }

    int operandAddress = stringHexToInt(SYMTAB[tempOperand].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[tempOperand].blockNumber]].startAddress);
    program_counter = address + stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress);
    program_counter += (halfBytes==5)?4:3;

    if(halfBytes==3){
      int relativeAddress = operandAddress - program_counter;
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+2,2);
        objcode += '2';
        objcode += intToStringHex(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+2,2);
          objcode += '4';
          objcode += intToStringHex(relativeAddress,halfBytes);
          return objcode;
        }
      }

      if(operandAddress<=4095){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+2,2);
        objcode += '0';
        objcode += intToStringHex(operandAddress,halfBytes);

        modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else{
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+2,2);
      objcode += '1';
      objcode += intToStringHex(operandAddress,halfBytes);

      modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      modificationRecord += (halfBytes==5)?"05":"03";
      modificationRecord += '\n';

      return objcode;
    }

    writeData = "Line: "+to_string(line);
    writeData += "Can't fit into program counter based or base register based addressing.";
    writeToFile(errorFile,writeData);
    objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+2,2);
    objcode += (halfBytes==5)?"100000":"0000";

    return objcode;
  }
  else if(getFlagFormat(operand)=='='){
    string tempOperand = operand.substr(1,operand.length()-1);

    if(tempOperand=="*"){
      tempOperand = "X'" + intToStringHex(address,6) + "'";
      modificationRecord += "M^" + intToStringHex(stringHexToInt(LITTAB[tempOperand].address)+stringHexToInt(BLOCKS[BLocksNumToName[LITTAB[tempOperand].blockNumber]].startAddress),6) + '^';
      modificationRecord += intToStringHex(6,2);
      modificationRecord += '\n';
    }

    if(LITTAB[tempOperand].exists=='n'){
      writeData = "Line "+to_string(line)+" : Symbol doesn't exists. Found " + tempOperand;
      writeToFile(errorFile,writeData);

      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
      objcode += (halfBytes==5)?"000":"0";
      objcode += "000";
      return objcode;
    }

    int operandAddress = stringHexToInt(LITTAB[tempOperand].address) + stringHexToInt(BLOCKS[BLocksNumToName[LITTAB[tempOperand].blockNumber]].startAddress);
    program_counter = address + stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress);
    program_counter += (halfBytes==5)?4:3;

    if(halfBytes==3){
      int relativeAddress = operandAddress - program_counter;
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
        objcode += '2';
        objcode += intToStringHex(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
          objcode += '4';
          objcode += intToStringHex(relativeAddress,halfBytes);
          return objcode;
        }
      }

      if(operandAddress<=4095){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
        objcode += '0';
        objcode += intToStringHex(operandAddress,halfBytes);

        modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else{
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
      objcode += '1';
      objcode += intToStringHex(operandAddress,halfBytes);

      modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      modificationRecord += (halfBytes==5)?"05":"03";
      modificationRecord += '\n';

      return objcode;
    }

    writeData = "Line: "+to_string(line);
    writeData += "Can't fit into program counter based or base register based addressing.";
    writeToFile(errorFile,writeData);
    objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
    objcode += (halfBytes==5)?"100":"0";
    objcode += "000";

    return objcode;
  }
  else{
    int xbpe=0;
    string tempOperand = operand;
    if(operand.substr(operand.length()-2,2)==",X"){
      tempOperand = operand.substr(0,operand.length()-2);
      xbpe = 8;
    }

    if(SYMTAB[tempOperand].exists=='n'){
      writeData = "Line "+to_string(line)+" : Symbol doesn't exists. Found " + tempOperand;
      writeToFile(errorFile,writeData);

      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
      objcode += (halfBytes==5)?(intToStringHex(xbpe+1,1)+"00"):intToStringHex(xbpe,1);
      objcode += "000";
      return objcode;
    }

    int operandAddress = stringHexToInt(SYMTAB[tempOperand].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[tempOperand].blockNumber]].startAddress);
    program_counter = address + stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress);
    program_counter += (halfBytes==5)?4:3;

    if(halfBytes==3){
      int relativeAddress = operandAddress - program_counter;
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
        objcode += intToStringHex(xbpe+2,1);
        objcode += intToStringHex(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
          objcode += intToStringHex(xbpe+4,1);
          objcode += intToStringHex(relativeAddress,halfBytes);
          return objcode;
        }
      }

      if(operandAddress<=4095){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
        objcode += intToStringHex(xbpe,1);
        objcode += intToStringHex(operandAddress,halfBytes);

        modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else{
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
      objcode += intToStringHex(xbpe+1,1);
      objcode += intToStringHex(operandAddress,halfBytes);

      modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      modificationRecord += (halfBytes==5)?"05":"03";
      modificationRecord += '\n';

      return objcode;
    }

    writeData = "Line: "+to_string(line);
    writeData += "Can't fit into program counter based or base register based addressing.";
    writeToFile(errorFile,writeData);
    objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(op)].op)+3,2);
    objcode += (halfBytes==5)?(intToStringHex(xbpe+1,1)+"00"):intToStringHex(xbpe,1);
    objcode += "000";

    return objcode;
  }
  return objcode ;
}

void writeTextRecord(bool lastRecord=false){
  if(lastRecord){
    if(currentRecord.length()>0){
      writeData = intToStringHex(currentRecord.length()/2,2) + '^' + currentRecord;
      writeToFile(objectFile,writeData);
      currentRecord = "";
    }
    return;
  }
  if(objectCode != ""){
    if(currentRecord.length()==0){
      writeData = "T^" + intToStringHex(address+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      writeToFile(objectFile,writeData,false);
    }
    if((currentRecord + objectCode).length()>60){
      writeData = intToStringHex(currentRecord.length()/2,2) + '^' + currentRecord;
      writeToFile(objectFile,writeData);

      currentRecord = "";
      writeData = "T^" + intToStringHex(address+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      writeToFile(objectFile,writeData,false);
    }

    currentRecord += objectCode;
  }
  else{
    if(op=="START"||op=="END"||op=="BASE"||op=="NOBASE"||op=="LTORG"||op=="ORG"||op=="EQU"){
    }
    else{
      if(currentRecord.length()>0){
        writeData = intToStringHex(currentRecord.length()/2,2) + '^' + currentRecord;
        writeToFile(objectFile,writeData);
      }
      currentRecord = "";
    }
  }
}

void writeEndRecord(bool write=true){
  if(write){
    if(endRecord.length()>0){
      writeToFile(objectFile,endRecord);
    }
    else{
      writeEndRecord(false);
    }
  }
  if(operand==""||operand==" "){
    endRecord = "E^" + intToStringHex(startAddress,6);
  }
  else{
    int firstExecutableAddress;
    if(SYMTAB[operand].exists=='n'){
      firstExecutableAddress = startAddress;
      writeData = "Line "+to_string(line)+" : Symbol doesn't exists. Found " + operand;
      writeToFile(errorFile,writeData);
    }
    else{
      firstExecutableAddress = stringHexToInt(SYMTAB[operand].address);
    }
    endRecord = "E^" + intToStringHex(firstExecutableAddress,6);
  }
}

void pass2(){
  string temp_string;
  intermediateFile.open("intermediate_"+FILE_NAME);
  if(!intermediateFile){
    cout<<"can't open : intermediate_"<<FILE_NAME<<endl;
    exit(1);
  }
  getline(intermediateFile, temp_string);

  objectFile.open("object_"+FILE_NAME);
  if(!objectFile){
    cout<<"can't open : object_"<<FILE_NAME<<endl;
    exit(1);
  }

  ListingFile.open("listing_"+FILE_NAME);
  if(!ListingFile){
    cout<<"can't open: listing_"<<FILE_NAME<<endl;
    exit(1);
  }
  writeToFile(ListingFile,"Line\tAddress\tLabel\tOPCODE\tOPERAND\tObjectCode\tComment");

  errorFile.open("error_"+FILE_NAME,fstream::app);
  if(!errorFile){
    cout<<"can't open: error_"<<FILE_NAME<<endl;
    exit(1);
  }
  writeToFile(errorFile,"\n\nPASS2:-");

  objectCode = "";
  currentTextRecordLength=0;
  currentRecord = "";
  modificationRecord = "";
  blockNumber = 0;
  nobase = true;

  readIntermediateFile(intermediateFile,isComment,line,address,blockNumber,label,op,operand,comment);
  while(isComment){
    writeData = to_string(line) + "\t" + comment;
    writeToFile(ListingFile,writeData);
    readIntermediateFile(intermediateFile,isComment,line,address,blockNumber,label,op,operand,comment);
  }

  if(op=="START"){
    startAddress = address;
    writeData = to_string(line) + "\t" + intToStringHex(address) + "\t" + to_string(blockNumber) + "\t" + label + "\t" + op + "\t" + operand + "\t" + objectCode +"\t" + comment;
    writeToFile(ListingFile,writeData);
  }
  else{
    label = "";
    startAddress = 0;
    address = 0;
  }

  writeData = "H^"+expandString(label,6,' ',true)+'^'+intToStringHex(address,6)+'^'+intToStringHex(program_length,6);
  writeToFile(objectFile,writeData);

  readIntermediateFile(intermediateFile,isComment,line,address,blockNumber,label,op,operand,comment);
  currentTextRecordLength  = 0;

  while(op!="END"){
    if(!isComment){
      if(OPTAB[getRealOpcode(op)].exists=='y'){
        if(OPTAB[getRealOpcode(op)].format==1){
          objectCode = OPTAB[getRealOpcode(op)].op;
        }
        else if(OPTAB[getRealOpcode(op)].format==2){
          operand1 = operand.substr(0,operand.find(','));
          operand2 = operand.substr(operand.find(',')+1,operand.length()-operand.find(',') -1 );

          if(operand2==operand){
            if(getRealOpcode(op)=="SVC"){
              objectCode = OPTAB[getRealOpcode(op)].op + intToStringHex(stoi(operand1),1) + '0';
            }
            else if(REGTAB[operand1].exists=='y'){
              objectCode = OPTAB[getRealOpcode(op)].op + REGTAB[operand1].num + '0';
            }
            else{
              objectCode = getRealOpcode(op) + '0' + '0';
              writeData = "Line: "+to_string(line)+" Invalid Register name";
              writeToFile(errorFile,writeData);
            }
          }
          else{
            if(REGTAB[operand1].exists=='n'){
              objectCode = OPTAB[getRealOpcode(op)].op + "00";
              writeData = "Line: "+to_string(line)+" Invalid Register name";
              writeToFile(errorFile,writeData);
            }
            else if(getRealOpcode(op)=="SHIFTR" || getRealOpcode(op)=="SHIFTL"){
              objectCode = OPTAB[getRealOpcode(op)].op + REGTAB[operand1].num + intToStringHex(stoi(operand2),1);
            }
            else if(REGTAB[operand2].exists=='n'){
              objectCode = OPTAB[getRealOpcode(op)].op + "00";
              writeData = "Line: "+to_string(line)+" Invalid Register name";
              writeToFile(errorFile,writeData);
            }
            else{
              objectCode = OPTAB[getRealOpcode(op)].op + REGTAB[operand1].num + REGTAB[operand2].num;
            }
          }
        }
        else if(OPTAB[getRealOpcode(op)].format==3){
          if(getRealOpcode(op)=="RSUB"){
            objectCode = OPTAB[getRealOpcode(op)].op;
            objectCode += (getFlagFormat(op)=='+')?"000000":"0000";
          }
          else{
            objectCode = createObjectCodeFormat34();
          }
        }
      }
      else if(op=="BYTE"){
        if(operand[0]=='X'){
          objectCode = operand.substr(2,operand.length()-3);
        }
        else if(operand[0]=='C'){
          objectCode = stringToHexString(operand.substr(2,operand.length()-3));
        }
      }
      else if(label=="*"){
        if(op[1]=='C'){
          objectCode = stringToHexString(op.substr(3,op.length()-4));
        }
        else if(op[1]=='X'){
          objectCode = op.substr(3,op.length()-4);
        }
      }
      else if(op=="WORD"){
        objectCode = intToStringHex(stoi(operand),6);
      }
      else if(op=="BASE"){
        if(SYMTAB[operand].exists=='y'){
          base_register_value = stringHexToInt(SYMTAB[operand].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[operand].blockNumber]].startAddress);
          nobase = false;
        }
        else{
          writeData = "Line "+to_string(line)+" : Symbol doesn't exists. Found " + operand;
          writeToFile(errorFile,writeData);
        }
        objectCode = "";
      }
      else if(op=="NOBASE"){
        if(nobase){
          writeData = "Line "+to_string(line)+": Assembler wasn't using base addressing";
          writeToFile(errorFile,writeData);
        }
        else{
          nobase = true;
        }
        objectCode = "";
      }
      else{
        objectCode = "";
      }

      writeTextRecord();

      if(blockNumber==-1){
        writeData = to_string(line) + "\t" + intToStringHex(address) + "\t" + " " + "\t" + label + "\t" + op + "\t" + operand + "\t" + objectCode +"\t" + comment;
      }
      else{
        writeData = to_string(line) + "\t" + intToStringHex(address) + "\t" + to_string(blockNumber) + "\t" + label + "\t" + op + "\t" + operand + "\t" + objectCode +"\t" + comment;
      }
    }
    else{
      writeData = to_string(line) + "\t" + comment;
    }
    writeToFile(ListingFile,writeData);
    readIntermediateFile(intermediateFile,isComment,line,address,blockNumber,label,op,operand,comment);
    objectCode = "";
  }
  writeTextRecord();

  writeEndRecord(false);
  writeData = to_string(line) + "\t" + intToStringHex(address) + "\t" + " " + "\t" + label + "\t" + op + "\t" + operand + "\t" + "" +"\t" + comment;
  writeToFile(ListingFile,writeData);

  while(readIntermediateFile(intermediateFile,isComment,line,address,blockNumber,label,op,operand,comment)){
    if(label=="*"){
      if(op[1]=='C'){
        objectCode = stringToHexString(op.substr(3,op.length()-4));
      }
      else if(op[1]=='X'){
        objectCode = op.substr(3,op.length()-4);
      }
      writeTextRecord();
    }
    writeData = to_string(line) + "\t" + intToStringHex(address) + "\t" + to_string(blockNumber) + label + "\t" + op + "\t" + operand + "\t" + objectCode +"\t" + comment;
    writeToFile(ListingFile,writeData);
  }
  writeTextRecord(true);
  writeToFile(objectFile,modificationRecord,false);
  writeEndRecord(true);
}

int main(){
  cout<<"ENTER INPUT FILENAME :";
  cin>>FILE_NAME;
  cout<<endl;
  cout<<"LOADING OPTAB"<<endl;
  load_tables();
  cout<<endl;
  cout<<"FIRST PASS"<<endl;
  cout<<"Writing intermediate file to 'intermediate_"<<FILE_NAME<<"'"<<endl;
  cout<<"Writing error file to 'error_"<<FILE_NAME<<"'"<<endl;
  pass1();
  cout<<endl;
  cout<<"Performing SECOND PASS"<<endl;
  cout<<"Writing object file to 'object_"<<FILE_NAME<<"'"<<endl;
  cout<<"Writing listing file to 'listing_"<<FILE_NAME<<"'"<<endl;
  pass2();
}
