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
        vector<int> memory;             // memory  0(268437408) to x -> insts; 2048(268435456) to y -> .word, y to z -> sw values;
        vector<string> parts;
        unordered_map<string, int> labels;
        unordered_map<int, string> strings;
        int div[5]={0};                 // div[0] -> opcode; div[1] -> rd; div[2] -> rs1; div[3] -> rs2; div[4] -> imm(address for branch, j)
        unordered_map<std::string, int> registers;
        Core();
        void parser(vector<string> program);
        void split(string line);
        string trim(string& str);
        InstType getInstType(string &inst);
        int Opcode(string op);
        void encodeInstToMem(int arr[]);
        pair<int, int> execute();
        string decodeMemToInst(int decode);
        void setRegister(int n, int value);
        int getRegister(int n);
        void setMemory(int wordAddress, int value);
        int getMemory(int wordAddress);
};