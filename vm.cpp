// Author: Jake Grether
// Course: CS4380
// Date: 02/26/2019
// Project 1

#include "vm.h"
using namespace std;

map<string, int> vm::ocMap = {
    {"JMP", 1},  {"JMR", 2},  {"BNZ", 3},  {"BGT", 4},  {"BLT", 5},
    {"BRZ", 6},  {"MOV", 7},  {"LDA", 8},  {"STR", 9},  {"LDR", 10},
    {"STB", 11}, {"LDB", 12}, {"ADD", 13}, {"ADI", 14}, {"SUB", 15},
    {"MUL", 16}, {"DIV", 17}, {"AND", 18}, {"OR", 19},  {"CMP", 20},
    {"TRP", 21}, {"RUN", 26}, {"END", 27}, {"BLK", 28}, {"LCK", 29},
    {"ULK", 30}};

map<string, string> vm::divMap = {{".INT", "int"}, {".BYT", "byt"}};

map<string, int> vm::regMap = {
    {"R0", 0},  {"R1", 1},  {"R2", 2},  {"R3", 3}, {"R4", 4},
    {"R5", 5},  {"R6", 6},  {"R7", 7},  {"PC", 8}, {"SL", 9},
    {"SP", 10}, {"FP", 11}, {"SB", 12},
};

const char vm::delim[] = " ,\t";
// const regex reg("[(]");

vm::vm() {
  byteCount = 0;
  lineCount = 1;
  errorStr = "";
};

////////////// FIRST PASS - Build symbol table and syntax check
int vm::firstPass(istream &in) {
  while (getline(in, line)) {
    if (line == "") {
      ++lineCount;
    }
    hasLabel = false;
    cstr = strdup(line.c_str());
    tok = strtok(cstr, delim);
    while (tok != NULL) {
      if (tok[0] == ';') {
        ++lineCount;
        break;
      }
      if (!ocMap.count(tok) && !regMap.count(tok) && !divMap.count(tok)) {
        if (!symTable.count(tok)) {
          label = tok;
          hasLabel = true;
          tok = strtok(NULL, delim);
        } else {
          errorStr = "Invalid re-use of label at line " + to_string(lineCount) +
                     ": " + string(tok) + "\n";
          throw runtime_error(errorStr);
        }
      }
      if (ocMap.count(tok)) {
        if (validateInstruction(tok)) {
          if (hasLabel) {
            symTable.insert(make_pair(label, byteCount));
          }
          byteCount += INSTLEN;
          ++lineCount;
          break;
        } else {
          errorStr = "Invalid value in instruction at line " +
                     to_string(lineCount) + ": " + string(tok) + "\n";
          throw runtime_error(errorStr);
        }
      } else if (divMap.count(tok)) {
        if (divMap.at(tok) == "int") {
          tok = strtok(NULL, delim);
          if (tok == NULL) {
            errorStr =
                "No value assigned to label at line: " + to_string(lineCount) +
                "\n";
            throw runtime_error(errorStr);
          }
          if (!isnan(atoi(tok))) {
            symTable.insert(make_pair(label, byteCount));
            byteCount += INTLEN;
            ++lineCount;
            break;
          } else {
            errorStr = "Invalid value: " + string(tok) +
                       " at line: " + to_string(lineCount) + "\n";
            throw runtime_error(errorStr);
          }
        } else if (divMap.at(tok) == "byt") {
          if (strtok(NULL, delim) == NULL) {
            errorStr = "Invalid value: " + string(tok) +
                       " at line: " + to_string(lineCount) + "\n";
            throw runtime_error(errorStr);
          }
          symTable.insert(make_pair(label, byteCount));
          byteCount += BYTLEN;
          ++lineCount;
          break;

        } else {
          errorStr = "Invalid value at line " + to_string(lineCount) + ": " +
                     string(tok) + "\n";
          throw runtime_error(errorStr);
        }
      } else {
        errorStr = "Syntax error at line " + to_string(lineCount) + ": " +
                   string(tok) + "\n";
        throw runtime_error(errorStr);
      }
    }
  }

  return 0;
}
bool vm::validateInstruction(const char *t) {
  if (ocMap.find(t)->first == "TRP") {
    t = strtok(NULL, delim);
    if (t == NULL) {
      return false;
    }
    if (stoi(t) < 5) {
      return true;
    } else {
      return false;
    }
  } else if (ocMap.find(t)->first == "END") {
    t = strtok(NULL, delim);
    if (t == NULL) {
      return true;
    } else {
      return false;
    }
  } else if (ocMap.find(t)->first == "BLK") {
    t = strtok(NULL, delim);
    if (t == NULL) {
      return true;
    } else {
      return false;
    }
  } else if (ocMap.find(t)->first == "LCK") {
    t = strtok(NULL, delim);
    if (t == NULL) {
      return false;
    }
    return true;
  } else if (ocMap.find(t)->first == "ULK") {
    t = strtok(NULL, delim);
    if (t == NULL) {
      return false;
    }
    return true;
  } else if (ocMap.find(t)->first == "JMP") {
    t = strtok(NULL, delim);
    if (t == NULL) {
      return false;
    }
    return true;
  } else if (ocMap.find(t)->first == "JMR") {
    t = strtok(NULL, delim);
    if (t == NULL) {
      return false;
    }
    if (regMap.count(t)) {
      return true;
    }
  } else {
    t = strtok(NULL, ", ");
    if (t == NULL) {
      return 0;
    }
    if (regMap.count(t)) {
      t = strtok(NULL, delim);
      if (t == NULL) {
        return false;
      }
      if (regMap.count(t) || symTable.count(t) || !isnan(atoi(t))) {
        return true;
      } else {
        return true;
      }
    } else {
      return false;
    }
  }
  return false;
}

////////////// SECOND PASS - Generate byte-Code and load into memory
int vm::secondPass(istream &in) {
  byteCount = 0;
  lineCount = 0;
  chptr = mem;
  while (getline(in, line)) {
    // if (line == "") {
    //   ++lineCount;
    // }
    cstr = strdup(line.c_str());
    tok = strtok(cstr, delim);
    while (tok != NULL) {
      if (tok[0] == ';') {
        ++lineCount;
        break;
      }
      if (symTable.count(tok)) {
        tok = strtok(NULL, delim);
      }
      if (ocMap.count(tok)) {
        if (firstInst) {
          beg = byteCount;
          firstInst = false;
        }

        opCode = ocMap.at(tok);

        tok = strtok(NULL, delim);

        if (opCode == 21) {
          oper1 = atoi(tok);
          oper2 = 0;
        } else if (opCode == 1) {
          oper1 = symTable.at(tok);
          oper2 = 0;
        } else if (opCode == 2) {
          oper1 = regMap.at(tok);
          oper2 = 0;
        } else if (opCode == 27 || opCode == 28) {
          oper1 = 0;
          oper2 = 0;
        } else if (opCode == 29 || opCode == 30) {
          oper1 = symTable.at(tok);
          oper2 = 0;
        } else {

          oper1 = regMap.at(tok);
          tok = strtok(NULL, delim);
          if (regMap.count(tok)) {
            if (opCode == 9) {
              opCode = 22;
            } else if (opCode == 10) {
              opCode = 23;
            } else if (opCode == 11) {
              opCode = 24;
            } else if (opCode == 12) {
              opCode = 25;
            }
            oper2 = regMap.at(tok);

          } else if (symTable.count(tok)) {
            oper2 = symTable.at(tok);
          } else if (!isnan(atoi(tok))) {
            oper2 = atoi(tok);
          } else {
            errorStr = "Invalid Operand/label: " + string(tok) +
                       "at line: " + to_string(lineCount) + "\n";
            throw runtime_error(errorStr);
          }
        }
        *reinterpret_cast<int *>(chptr) = opCode;
        chptr += INTLEN;
        *reinterpret_cast<int *>(chptr) = oper1;
        chptr += INTLEN;
        *reinterpret_cast<int *>(chptr) = oper2;
        chptr += INTLEN;
        byteCount += INSTLEN;
        ++lineCount;
        break;
      } else if (divMap.count(tok)) {
        if (divMap.at(tok) == "int") {
          tok = strtok(NULL, delim);
          *reinterpret_cast<int *>(chptr) = atoi(tok);
          byteCount += INTLEN;
          chptr += INTLEN;
          ++lineCount;

          break;
        } else if (divMap.at(tok) == "byt") {
          tok = strtok(NULL, " \t");
          if (tok[1] == '\\') {
            ch = '\n';
          } else if (tok[1]) {
            ch = tok[1];
          } else {
            ch = ' ';
          }
          *chptr = ch;
          byteCount += BYTLEN;
          chptr += BYTLEN;
          ++lineCount;
          break;
        } else {
          throw runtime_error("Failed to convert directive to bytecode!\n");
        }
      } else {
        throw runtime_error("Failed to convert instruction to bytecode\n");
      }
    }
  }
  return 0;
}

//////////// EXECUTE PROGRAM - run bytecode from memory
int vm::execute() {
  // cout << symTable.at("NewThread") << endl;
  int offset = 0;
  memptr = mem + beg;
  tid = 0;
  reg[8] = beg; // PC
  // cout << "p1" <<  reg[8] << endl;
  reg[12] = MEMSIZE;
  // STACK BASE (SB)
  reg[10] = reg[12];          // INITIALIZE STACK POINTER (SP)
  reg[9] = reg[12] - STKSIZE; // STACK LIMIT (SL)
                              // cout << reg[8] << endl;
  while (memptr < &mem[byteCount]) {

    // Round Robin Scheduling Algorithm
    if (threads.size() > 1) {
      offset = MEMSIZE - (tid * TS);
      for (int i = 0; i < 13; i++) {
        *reinterpret_cast<int *>(mem + offset) = reg[i];
        offset -= 4;
      }
      // for (auto m : threads) {
      //   cout << m << ", ";
      // }
      // cout << endl;
      // GET ID OF NEXT ACTIVE THREAD
      if (threads[0] == 'F') {
        // cout << "blocked" << endl;
        for (int i = 1; i < threads.size(); i++) {
          if (threads[i] == 'T')
            threadf = true;
        }
        if (!threadf) {
          threads[0] = 'T';
        }
        threadf = false;
      }
      do {
        ++tid;
        if (tid == threads.size()) {
          tid = 0;
        }
        if (threads[tid] == 'T') {
          break;
        }
      } while (threads[tid] == 'F');

      // cout << "tid: " << tid << endl;

      // LOAD REGISTERS FROM THREAD STATE
      offset = MEMSIZE - (tid * TS);
      for (int i = 0; i < (sizeof(reg) / sizeof(reg[0])); i++) {
        reg[i] = *reinterpret_cast<int *>(mem + offset);
        if (i == 8) {
          // cout << *reinterpret_cast<int *>(mem  offset) << endl;
        }
        // cout << reg[i] << endl;
        offset -= 4;
      }
      // cout << reg[8] << endl;
      memptr = mem + reg[8];
      // for reg[0]
    }
    reg[8] += INSTLEN;
    // FETCH INSTRUCTION:
    // LOAD INSTRUCTION
    // cout << reg[8] << endl;
    opCode = *reinterpret_cast<int *>(memptr);
    memptr += INTLEN;
    oper1 = *reinterpret_cast<int *>(memptr);
    memptr += INTLEN;
    oper2 = *reinterpret_cast<int *>(memptr);
    memptr += INTLEN;
    // cout <<  " Opcode: " << opCode << " oper1: " << oper1 << " oper2: " <<
    // oper2 << endl;
    //  for(auto i: reg){
    //       cout << i << " ";
    //     }
    //     cout << endl;

    // INCRETMENT PC
    // cout << int(mem[1999996]) << endl;
    // reg[8] += INSTLEN;
    // cout << reg[8] << endl;
    // DECODE OPCODE
    switch (opCode) {
    case 1:
      jmp(oper1);
      break;
    case 2:
      jmr(oper1);
      break;
    case 3:
      bnz(oper1, oper2);
      break;
    case 4:
      bgt(oper1, oper2);
      break;
    case 5:
      blt(oper1, oper2);
      break;
    case 6:
      brz(oper1, oper2);
      break;
    case 7:
      mov(oper1, oper2);
      break;
    case 8:
      lda(oper1, oper2);
      break;
    case 9:
      str(opCode, oper1, oper2);
      break;
    case 10:
      ldr(opCode, oper1, oper2);
      break;
    case 11:
      stb(opCode, oper1, oper2);
      break;
    case 12:
      ldb(opCode, oper1, oper2);
      break;
    case 13:
      add(oper1, oper2);
      break;
    case 14:
      adi(oper1, oper2);
      break;
    case 15:
      sub(oper1, oper2);
      break;
    case 16:
      mult(oper1, oper2);
      break;
    case 17:
      div(oper1, oper2);
      break;
    case 18:
      and_(oper1, oper2);
      break;
    case 19:
      or_(oper1, oper2);
      break;
    case 20:
      cmp(oper1, oper2);
      break;
    case 21:
      if (oper1 == 0) {
        return 0;
      } else {
        trp(oper1);
        break;
      }
      break;
    case 22:
      str(opCode, oper1, oper2);
      break;
    case 23:
      ldr(opCode, oper1, oper2);
      break;
    case 24:
      stb(opCode, oper1, oper2);
      break;
    case 25:
      ldb(opCode, oper1, oper2);
      break;
    case 26:
      run(oper1, oper2);
      break;
    case 27:
      end();
      break;
    case 28:
      blk();
      break;
    case 29:
      lck(oper1);
      break;
    case 30:
      ulk(oper1);
      break;
    }
  }

  return 0;
}

// MOVE INSTRUCTIONS
void vm::jmp(int label) {
  memptr = mem + label;
  reg[8] = label;
}
void vm::jmr(int rs) {
  memptr = mem + reg[rs];
  reg[8] = reg[rs];
  // cout << "fp" <<  *reinterpret_cast<int*>(mem + reg[11]) << endl;
}
void vm::bnz(int rs, int label) {
  if (reg[rs] != 0) {
    memptr = mem + label;
    reg[8] = label;
  }
}
void vm::bgt(int rs, int label) {
  if (reg[rs] > 0) {
    memptr = mem + label;
    reg[8] = label;
  }
}
void vm::blt(int rs, int label) {
  if (reg[rs] < 0) {
    memptr = mem + label;
    reg[8] = label;
  }
}
void vm::brz(int rs, int label) {
  if (reg[rs] == 0) {
    memptr = mem + label;
    reg[8] = label;
  }
}
void vm::mov(int rd, int rs) { reg[rd] = reg[rs]; }
void vm::lda(int rd, int label) { reg[rd] = label; }
void vm::str(int oc, int rs, int label) {
  if (oc == 9) {
    int data = reg[rs];
    *reinterpret_cast<int *>(mem + label) = data;
  }
  if (oc == 22) {
    int data = reg[rs];
    *reinterpret_cast<int *>(mem + reg[label]) = data;
  }
}
void vm::ldr(int oc, int rd, int label) {
  if (oc == 10) {
    reg[rd] = *reinterpret_cast<int *>(mem + label);
  } else if (oc == 23) {
    reg[rd] = *reinterpret_cast<int *>(mem + reg[label]);
    // cout << "\n" << *reinterpret_cast<int *>(mem + reg[label]) << endl;
  }
}
void vm::stb(int oc, int rs, int label) {
  if (oc == 11) {
    char byte = char(reg[rs]);
    mem[label] = byte;
  } else if (oc == 24) {
    *reinterpret_cast<int *>(mem + reg[label]) = reg[rs];
  }
}
void vm::ldb(int oc, int rd, int label) {
  if (oc == 12) {
    char c = mem[label];
    reg[rd] = int(c);
  } else if (oc == 25) {
    reg[rd] = int(*(mem + reg[label]));
  }
}

// ARITHMETIC INSTRUCTIONS
void vm::add(int rd, int rs) {
  int temp = reg[rd];
  reg[rd] = reg[rs] + temp;
}
void vm::adi(int rd, int imm) {
  int temp = reg[rd];
  reg[rd] = temp + imm;
}
void vm::sub(int rd, int rs) {
  int temp = reg[rd];
  reg[rd] = temp - reg[rs];
}
void vm::mult(int rd, int rs) {
  int temp = reg[rd];
  reg[rd] = reg[rs] * temp;
}
void vm::div(int rd, int rs) {
  int temp = reg[rd];
  if (reg[rs] == 0) {
    throw runtime_error("Cannot divide by 0");
  }
  reg[rd] = temp / reg[rs];
}
void vm::and_(int rd, int rs) {}
void vm::or_(int rd, int rs) {}
void vm::cmp(int rd, int rs) {
  if (reg[rd] == reg[rs]) {
    reg[rd] = 0;
  } else if (reg[rd] > reg[rs]) {
    reg[rd] = 1;
  } else if (reg[rd] < reg[rs]) {
    reg[rd] = -1;
  }
}

// TRAPS
void vm::trp(int r) {
  char c, *cst;
  int intIn;
  if (r == 1) {
    cout << reg[3] << flush;
  } else if (r == 2) {
    cin >> intIn;
    reg[3] = intIn;
  } else if (r == 3) {
    cout << static_cast<char>(reg[3]) << flush;
  } else if (r == 4) {
    char c;
    c = getchar();
    // cout << c << endl;
    reg[3] = c;
  }
}

void vm::run(int r, int label) {
  if (threads.size() < THREADMAX) {
    reg[r] = threads.size();
    threads.push_back('T');

    int offset = MEMSIZE - (reg[r] * TS);
    // cout << "offset: " << offset << endl;
    for (int i = 0; i < 13; i++) {
      if (i == 8) {
        // cout << "PC: " << label << endl;
        *reinterpret_cast<int *>(mem + offset) = label;
        // cout << "PC: " << *reinterpret_cast<int *>(mem - offset) << endl;
      } else if (i == 9) {
        *reinterpret_cast<int *>(mem + offset) = (MEMSIZE - (reg[r] * TS) - TS);
        // cout << "SL: " << (MEMSIZE - (reg[r] * TS) - TS) << endl;
      } else if (i == 10 || i == 12) {
        *reinterpret_cast<int *>(mem + offset) = (MEMSIZE - (reg[r] * TS) - 52);
        // cout << "SB: " << (MEMSIZE - (reg[r] * TS) - 52) << endl;
      } else {
        *reinterpret_cast<int *>(mem + offset) = reg[i];
      }
      offset -= 4;
    }

  } else {
    throw runtime_error("All Thread Identifiers in Use!\n");
  }
}
void vm::end() {
  if (tid != 0) {
    threads[tid] = 'F';
    // for (auto m : threads) {
    //   cout << m << ", ";
    // }
    // cout << endl;
  }
}
void vm::blk() { threads[0] = 'F'; }
void vm::lck(int label) {
  if (*reinterpret_cast<int *>(mem + label) == -1) {
    *reinterpret_cast<int *>(mem + label) = tid;
    //cout << tid << " locked " << endl;
  } else {
    //cout << tid << " is Blocked " << endl;
    reg[8] = reg[8] - INSTLEN;
  }
}
void vm::ulk(int label) {
  if (tid == *reinterpret_cast<int *>(mem + label)) {
    *reinterpret_cast<int *>(mem + label) = -1;
    //cout << tid << " unlocked " << endl;
  } else {
    //cout << tid << " is Blocked " << endl;
    reg[8] = reg[8] - INSTLEN;
  }
}
int main(int argc, char *argv[]) {
  vm v;
  if (argv[1] != NULL) {
    fstream asmIn(argv[1]);
    if (asmIn) {
      try {
        // cout << "First Pass..." << endl;
        v.firstPass(asmIn);
        asmIn.clear();
        asmIn.seekg(0, ios::beg);
        // cout << "Second Pass..." << endl;

        v.secondPass(asmIn);
        // cout << "Executing Program..." << endl << endl;
        v.execute();

      } catch (runtime_error e) {
        std::cout << e.what();
        return -1;
      }
      asmIn.close();
    } else {
      cout << "Could not open file" << endl;
    }
  } else {
    cout << "Invalid command line argument!" << endl;
  }
  return 0;
}