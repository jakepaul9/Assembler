//Author: Jake Grether
//Course: CS4380
//Date: 02/26/2019
//Project 1

#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <cstdio>
#include <cmath>
#include <regex>

class vm {
  static const int INSTLEN = 12;
  static const int THREADMAX = 10;
  static const int TS = 2000;
  static const int BYTLEN = 1;
  static const int INTLEN = 4;
  static const int MEMSIZE = 2000000;
  static const int STKSIZE = 20000;
  static const char delim[];
  static std::map<std::string, int> ocMap;
  static std::map<std::string, int> regMap;
  static std::map<std::string, std::string> divMap;
  std::map<std::string, int> symTable;

  int beg, asc, opCode, oper1, oper2, byteCount, lineCount, tid, mutex = -1, reg[13];
  char *memptr, *chptr, *cstr, *tok, *label, mem[MEMSIZE], ch;
  std::vector<char>threads = {'T'};
  std::string line;
  bool hasLabel, firstInst = true, threadf = false;

public:
  std::string errorStr;
  vm();
  int firstPass(std::istream &);
  int secondPass(std::istream &);
  int execute();

private:
  void jmp(int);
  void jmr(int);
  void bnz(int, int);
  void bgt(int, int);
  void blt(int, int);
  void brz(int, int);
  void mov(int, int);
  void lda(int, int);
  void str(int, int, int);
  void ldr(int, int, int);
  void stb(int, int, int);
  void ldb(int, int, int);
  void add(int, int);
  void adi(int, int);
  void sub(int, int);
  void mult(int, int);
  void div(int, int);
  void and_(int, int);
  void or_(int, int);
  void cmp(int, int);
  void trp(int);
  void run(int, int);
  void end();
  void blk();
  void lck(int);
  void ulk(int);
  bool validateInstruction(const char *);
  char* instToByteCode(const char *);
  bool writeToMem(const char*, int);
};
