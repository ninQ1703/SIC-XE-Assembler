#include "utility.cpp" 
#include "tables.cpp"

using namespace std;

string FILE_NAME;
bool error_flag=false;
int program_length;
string *BLocksNumToName;

void LTORGhandling(string& Prefix, int& lineNoD,int lineNo,int& LOCCTR, int& lastDLOCCTR, int curBlockNo){
  string litAdrs,litValue;
  Prefix = "";
  for(auto const& it: LITTAB){
    litAdrs = it.second.address;
    litValue = it.second.value;
    if(litAdrs!="?"){
    }
    else{
      lineNo += 5;
      lineNoD += 5;
      LITTAB[it.first].address = intToStringHex(LOCCTR);
      LITTAB[it.first].blockNumber = curBlockNo;
      Prefix += "\n" + to_string(lineNo) + "\t" + intToStringHex(LOCCTR) + "\t" + to_string(curBlockNo) + "\t" + "*" + "\t" + "="+litValue + "\t" + " " + "\t" + " ";

      if(litValue[0]=='X'){
        LOCCTR += (litValue.length() -3)/2;
        lastDLOCCTR += (litValue.length() -3)/2;
      }
      else if(litValue[0]=='C'){
        LOCCTR += litValue.length() -3;
        lastDLOCCTR += litValue.length() -3;
      }
    }
  }
}

void evaluateExpression(string expression, bool& relative,string& tempOp,int lineNo, ofstream& errorFile,bool& error_flag){
  string singleOperand="?",singleOperator="?",valueString="",valueTemp="",writeData="";
  int lastOpd=0,lastOpt=0,pairCount=0;
  char lastByte = ' ';
  bool Illegal = false;

  for(int i=0;i<expression.length();){
    singleOperand = "";

    lastByte = expression[i];
    while((lastByte!='+' && lastByte!='-' && lastByte!='/' && lastByte!='*') && i<expression.length()){
      singleOperand += lastByte;
      lastByte = expression[++i];
    }

    if(SYMTAB[singleOperand].exists=='y'){
      lastOpd = SYMTAB[singleOperand].relative;
      valueTemp = to_string(stringHexToInt(SYMTAB[singleOperand].address));
    }
    else if((singleOperand != "" || singleOperand !="?" ) && if_all_num(singleOperand)){
      lastOpd = 0;
      valueTemp = singleOperand;
    }
    else{
      writeData = "Line: "+to_string(lineNo)+" : Can't find symbol. Found "+singleOperand;
      writeToFile(errorFile,writeData);
      Illegal = true;
      break;
    }

    if(lastOpd*lastOpt == 1){

      writeData = "Line: "+to_string(lineNo)+" : Illegal expression";
      writeToFile(errorFile,writeData);
      error_flag = true;
      Illegal = true;
      break;
    }
    else if((singleOperator=="-" || singleOperator=="+" || singleOperator=="?")&&lastOpd==1){
      if(singleOperator=="-"){
        pairCount--;
      }
      else{
        pairCount++;
      }
    }

    valueString += valueTemp;

    singleOperator= "";
    while(i<expression.length()&&(lastByte=='+'||lastByte=='-'||lastByte=='/'||lastByte=='*')){
      singleOperator += lastByte;
      lastByte = expression[++i];
    }

    if(singleOperator.length()>1){
      writeData = "Line: "+to_string(lineNo)+" : Illegal operator in expression. Found "+singleOperator;
      writeToFile(errorFile,writeData);
      error_flag = true;
      Illegal = true;
      break;
    }

    if(singleOperator=="*" || singleOperator == "/"){
      lastOpt = 1;
    }
    else{
      lastOpt = 0;
    }

    valueString += singleOperator;
  }

  if(!Illegal){
    if(pairCount==1){
      relative = 1;
      EvaluateString tempOBJ(valueString);
      tempOp = intToStringHex(tempOBJ.getResult());
    }
    else if(pairCount==0){
      relative = 0;
      cout<<valueString<<endl;
      EvaluateString tempOBJ(valueString);
      tempOp = intToStringHex(tempOBJ.getResult());
    }
    else{
      writeData = "Line: "+to_string(lineNo)+" : Illegal expression";
      writeToFile(errorFile,writeData);
      error_flag = true;
      tempOp = "00000";
      relative = 0;
    }
  }
  else{
    tempOp = "00000";
    error_flag = true;
    relative = 0;
  }
}
void pass1(){
  ifstream sourceFile;
  ofstream intermediateFile, errorFile;

  sourceFile.open(FILE_NAME);
  if(!sourceFile){
    cout<<"can't open : "<<FILE_NAME<<endl;
    exit(1);
  }

  intermediateFile.open("intermediate_" + FILE_NAME);
  if(!intermediateFile){
    cout<<"can't open : intermediate_"<<FILE_NAME<<endl;
    exit(1);
  }
  writeToFile(intermediateFile,"Line\tAddress\tLabel\tOPCODE\tOPERAND\tComment");
  errorFile.open("error_"+FILE_NAME);
  if(!errorFile){
    cout<<"can't open: error_"<<FILE_NAME<<endl;
    exit(1);
  }
  writeToFile(errorFile,"PASS1-");

  string fileLine;
  string writeData,writeDataSuffix="",writeDataPrefix="";
  int index=0;

  string curBlockName = "";
  int curBlockNo = 0;
  int totalBlocks = 1;

  bool statusCode;
  string label,opcode,operand,comment;
  string tempOp;

  int startAddress,LOCCTR,saveLOCCTR,lineNo,lastDLOCCTR,lineNoD=0;
  lineNo = 0;
  lastDLOCCTR = 0;

  getline(sourceFile,fileLine);
  lineNo += 5;
  while(checkCommentLine(fileLine)){
    writeData = to_string(lineNo) + "\t" + fileLine;
    writeToFile(intermediateFile,writeData);
    getline(sourceFile,fileLine);
    lineNo += 5;
    index = 0;
  }

  readFirstNonWhiteSpace(fileLine,index,statusCode,label);
  readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);

  if(opcode=="START"){
    readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
    readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);
    startAddress = stringHexToInt(operand);
 
    LOCCTR = startAddress;
    writeData = to_string(lineNo) + "\t" + intToStringHex(LOCCTR-lastDLOCCTR) + "\t" + to_string(curBlockNo) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
    writeToFile(intermediateFile,writeData); 

    getline(sourceFile,fileLine);
    lineNo += 5;
    index = 0;
    readFirstNonWhiteSpace(fileLine,index,statusCode,label); 
    readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);
  }
  else{
    startAddress = 0;
    LOCCTR = 0;
  }

  while(opcode!="END"){
    if(!checkCommentLine(fileLine)){
      if(label!=""){
        if(SYMTAB[label].exists=='n'){
          SYMTAB[label].name = label;
          SYMTAB[label].address = intToStringHex(LOCCTR);
          SYMTAB[label].relative = 1;
          SYMTAB[label].exists = 'y';
          SYMTAB[label].blockNumber = curBlockNo;
        }
        else{
          writeData = "Line: "+to_string(lineNo)+" : Duplicate symbol for '"+label+"'. Previously defined at "+SYMTAB[label].address;
          writeToFile(errorFile,writeData);
          error_flag = true;
        }
      }
      if(OPTAB[getRealOpcode(opcode)].exists=='y'){
        if(OPTAB[getRealOpcode(opcode)].format==3){
          LOCCTR += 3;
          lastDLOCCTR += 3;
          if(getFlagFormat(opcode)=='+'){
            LOCCTR += 1;
            lastDLOCCTR += 1;
          }
          if(getRealOpcode(opcode)=="RSUB"){
            operand = " ";
          }
          else{
            readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
            if(operand[operand.length()-1] == ','){
              readFirstNonWhiteSpace(fileLine,index,statusCode,tempOp);
              operand += tempOp;
            }
          }

          if(getFlagFormat(operand)=='='){
            tempOp = operand.substr(1,operand.length()-1);
            if(tempOp=="*"){
              tempOp = "X'" + intToStringHex(LOCCTR-lastDLOCCTR,6) + "'";
            }
            if(LITTAB[tempOp].exists=='n'){
              LITTAB[tempOp].value = tempOp;
              LITTAB[tempOp].exists = 'y';
              LITTAB[tempOp].address = "?";
              LITTAB[tempOp].blockNumber = -1;
            }
          }
        }
        else if(OPTAB[getRealOpcode(opcode)].format==1){
          operand = " ";
          LOCCTR += OPTAB[getRealOpcode(opcode)].format;
          lastDLOCCTR += OPTAB[getRealOpcode(opcode)].format;
        }
        else{
          LOCCTR += OPTAB[getRealOpcode(opcode)].format;
          lastDLOCCTR += OPTAB[getRealOpcode(opcode)].format;
          readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
          if(operand[operand.length()-1] == ','){
            readFirstNonWhiteSpace(fileLine,index,statusCode,tempOp);
            operand += tempOp;
          }
        }
      }
      else if(opcode == "WORD"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        LOCCTR += 3;
        lastDLOCCTR += 3;
      }
      else if(opcode == "RESW"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        LOCCTR += 3*stoi(operand);
        lastDLOCCTR += 3*stoi(operand);
      }
      else if(opcode == "RESB"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        LOCCTR += stoi(operand);
        lastDLOCCTR += stoi(operand);
      }
      else if(opcode == "BYTE"){
        readByteOperand(fileLine,index,statusCode,operand);
        if(operand[0]=='X'){
          LOCCTR += (operand.length() -3)/2;
          lastDLOCCTR += (operand.length() -3)/2;
        }
        else if(operand[0]=='C'){
          LOCCTR += operand.length() -3;
          lastDLOCCTR += operand.length() -3;
        }
      }
      else if(opcode=="BASE"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
      }
      else if(opcode=="LTORG"){
        operand = " ";
        LTORGhandling(writeDataSuffix,lineNoD,lineNo,LOCCTR,lastDLOCCTR,curBlockNo);
      }
      else if(opcode=="ORG"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);

        char lastByte = operand[operand.length()-1];
        while(lastByte=='+'||lastByte=='-'||lastByte=='/'||lastByte=='*'){
          readFirstNonWhiteSpace(fileLine,index,statusCode,tempOp);
          operand += tempOp;
          lastByte = operand[operand.length()-1];
        }

        int tempVariable;
        tempVariable = saveLOCCTR;
        saveLOCCTR = LOCCTR;
        LOCCTR = tempVariable;

        if(SYMTAB[operand].exists=='y'){
          LOCCTR = stringHexToInt(SYMTAB[operand].address);
        }
        else{
          bool relative;
          error_flag = false;
          evaluateExpression(operand,relative,tempOp,lineNo,errorFile,error_flag);
          if(!error_flag){
            LOCCTR = stringHexToInt(tempOp);
          }
          error_flag = false;
        }
      }
      else if(opcode=="USE"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        BLOCKS[curBlockName].LOCCTR = intToStringHex(LOCCTR);

        if(BLOCKS[operand].exists=='n'){
          BLOCKS[operand].exists = 'y';
          BLOCKS[operand].name = operand;
          BLOCKS[operand].number = totalBlocks++;
          BLOCKS[operand].LOCCTR = "0";
        }
        curBlockNo = BLOCKS[operand].number;
        curBlockName = BLOCKS[operand].name;
        LOCCTR = stringHexToInt(BLOCKS[operand].LOCCTR);

      }
      else if(opcode=="EQU"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        tempOp = "";
        bool relative;

        if(operand=="*"){
          tempOp = intToStringHex(LOCCTR-lastDLOCCTR,6);
          relative = 1;
        }
        else if(if_all_num(operand)){
          tempOp = intToStringHex(stoi(operand),6);
          relative = 0;
        }
        else{
          char lastByte = operand[operand.length()-1];

          while(lastByte=='+'||lastByte=='-'||lastByte=='/'||lastByte=='*'){
            readFirstNonWhiteSpace(fileLine,index,statusCode,tempOp);
            operand += tempOp;
            lastByte = operand[operand.length()-1];
          }
          evaluateExpression(operand,relative,tempOp,lineNo,errorFile,error_flag);
        }

        SYMTAB[label].name = label;
        SYMTAB[label].address = tempOp;
        SYMTAB[label].relative = relative;
        SYMTAB[label].blockNumber = curBlockNo;
        lastDLOCCTR = LOCCTR - stringHexToInt(tempOp);
      }
      else{
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        writeData = "Line: "+to_string(lineNo)+" : Invalid OPCODE. Found " + opcode;
        writeToFile(errorFile,writeData);
        error_flag = true;
      }
      readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);
      if(opcode=="EQU" && SYMTAB[label].relative == 0){
        writeData = writeDataPrefix + to_string(lineNo) + "\t" + intToStringHex(LOCCTR-lastDLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeDataSuffix;
      }
      else{
        writeData = writeDataPrefix + to_string(lineNo) + "\t" + intToStringHex(LOCCTR-lastDLOCCTR) + "\t" + to_string(curBlockNo) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeDataSuffix;
      }
      writeDataPrefix = "";
      writeDataSuffix = "";
    }
    else{
      writeData = to_string(lineNo) + "\t" + fileLine;
    }
    writeToFile(intermediateFile,writeData);

    BLOCKS[curBlockName].LOCCTR = intToStringHex(LOCCTR);
    getline(sourceFile,fileLine); 
    lineNo += 5 + lineNoD;
    lineNoD = 0;
    index = 0;
    lastDLOCCTR = 0;
    readFirstNonWhiteSpace(fileLine,index,statusCode,label);
    readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);
  }
  readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
  readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);

  curBlockName = "";
  curBlockNo = 0;
  LOCCTR = stringHexToInt(BLOCKS[curBlockName].LOCCTR);

  LTORGhandling(writeDataSuffix,lineNoD,lineNo,LOCCTR,lastDLOCCTR,curBlockNo);
  BLOCKS[""].LOCCTR = intToStringHex(LOCCTR);
  writeData = to_string(lineNo) + "\t" + intToStringHex(LOCCTR-lastDLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeDataSuffix;
  writeToFile(intermediateFile,writeData);

  int LocctrArr[totalBlocks];
  BLocksNumToName = new string[totalBlocks];
  for(auto const& it: BLOCKS){
    LocctrArr[it.second.number] = stringHexToInt(it.second.LOCCTR);
    BLocksNumToName[it.second.number] = it.first;
  }

  for(int i = 1 ;i<totalBlocks;i++){
    LocctrArr[i] += LocctrArr[i-1];
  }

  for(auto const& it: BLOCKS){
    if(it.second.startAddress=="?"){
      BLOCKS[it.first].startAddress= intToStringHex(LocctrArr[it.second.number - 1]);
    }
  }

  program_length = LocctrArr[totalBlocks - 1] - startAddress;

  sourceFile.close();
  intermediateFile.close();
  errorFile.close();
}
