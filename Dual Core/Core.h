#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <bitset>
#include <sstream>
#include <fstream>
#include <iomanip>
using namespace std;

enum InstType{
    R_TYPE, I_TYPE, S_TYPE, SB_TYPE, LABELS, ECALL, UNKNOWN
};

class Core{
    public:
        vector<string> parts;
        unordered_map<string, int> labels;
        unordered_map<int, string> strings;
        int div[5]={0};                      // div[0] -> opcode; div[1] -> rd; div[2] -> rs1; div[3] -> rs2; div[4] -> imm(address for branch, j)
        unordered_map<std::string, int> registers;
        Core();
        void parser(vector<string> program, int core, vector<int>& memory);             // memory  0(268437408) to x1 -> insts of c0; 1024 to x2 -> insts of c1; 2048(268435456)(0x10000000) to y1 -> .word, y1 to z1 -> sw values of c0;  3072(268439552)(0x10001000) to y2 -> .word, y2 to z2 -> sw values of c1
        void split(string line);
        string trim(string& str);
        InstType getInstType(string &inst);
        int Opcode(string op);
        void encodeInstToMem(int arr[], int core, vector<int>& memory);
        pair<int, int> execute(int core, vector<int>& memory);
        string decodeMemToInst(int decode);
        void setRegister(int n, int value);
        int getRegister(int n);
        void setMemory(int wordAddress, int value, vector<int>& memory);
        int getMemory(int wordAddress, vector<int>& memory);
};

#endif