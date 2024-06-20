#include "Core.h"

        Core:: Core(){
        memory=vector<int>(4096,0);
        registers = {
                {"x0", 0},
                {"x1", 0},
                {"x2", 0},
                {"x3", 0},
                {"x4", 0},
                {"x5", 0},
                {"x6", 0},
                {"x7", 0},
                {"x8", 0},
                {"x9", 0},
                {"x10", 0},
                {"x11", 0},
                {"x12", 0},
                {"x13", 0},
                {"x14", 0},
                {"x15", 0},
                {"x16", 0},
                {"x17", 0},
                {"x18", 0},
                {"x19", 0},
                {"x20", 0},
                {"x21", 0},
                {"x22", 0},
                {"x23", 0},
                {"x24", 0},
                {"x25", 0},
                {"x26", 0},
                {"x27", 0},
                {"x28", 0},
                {"x29", 0},
                {"x30", 0},
                {"x31", 0}
            };
        }

        void Core:: parser(vector<string> program){
            int index;
            int strStore = 4060;        // for storing strings base address
            for (int i = 0; i < program.size(); i++) {
                string trimmedLabel = trim(program[i]);
                if (trimmedLabel.find(":") != string::npos) {
                    // Remove leading and trailing whitespaces before storing in the map
                    if(trimmedLabel.find(".word") != string::npos){
                        split(program[i]);
                        labels[parts[0]] = index = ((stoi(parts[2].substr(0), nullptr, 0) - 268435456)/4) + 2048;        // base: 0x10000000(268435456) == 2048, 0x10000004(268435460) == 2049
                    }
                    else if(trimmedLabel.find(".string") != string::npos){
                        split(program[i]);
                        char firstQuotePos = trimmedLabel.find('"');
                        char lastQuotePos = trimmedLabel.rfind('"');
                        if (firstQuotePos != string::npos && lastQuotePos != string::npos && firstQuotePos < lastQuotePos) {
                            strings[strStore] = trimmedLabel.substr(firstQuotePos + 1, lastQuotePos - firstQuotePos - 1);       // storing string and that corresponding memory address in map
                            labels[parts[0]] = strStore;                    // storing string labels and their memory address
                            strStore++;                                     // incrementing memory address
                        }
                        else{
                            strings[strStore] = "";
                        }
                    }
                    else{
                        labels[trimmedLabel] = i;                           // Label:, Exit:, main:
                    }
                }
            }

            for (int i = 0; i < program.size(); i++) {
                if(program[i].find(".word") != string::npos){
                    split(program[i]);
                    for (int j = program[i].find(".word") == 0 ? 1:2 ; j < parts.size(); j++, index++) {     // base: .word 1 2 3 4 also works, base: .word 0x10000004 doesn't work
                        int value = stoi(parts[j]);
                        setMemory(index, value);
                    }
                }
            }

            //int pc=0                        //starts from line 1
            for(auto line : program){
                split(line);
                if (parts.empty()) {
                    continue;
                }
                fill(div, div + 5, 0);                          // default div array shd be 0
                div[0]=Opcode(parts[0]);
                InstType type = getInstType(parts[0]);
                switch(type){
                    case R_TYPE:
                        // add x1,x2,x3
                        div[1] = stoi(parts[1].substr(1));              //substr(0) if common names are implemented
                        div[2] = stoi(parts[2].substr(1));
                        div[3] = stoi(parts[3].substr(1));
                        break;
                    case I_TYPE:
                        // addi x1,x2,3     //li x1,3 
                        div[1] = stoi(parts[1].substr(1));
                        if(div[0] == 6){
                            div[4] = stoi(parts[2].substr(0));
                        }
                        else{
                            div[2] = stoi(parts[2].substr(1));
                            div[4] = stoi(parts[3].substr(0));
                        }
                        break;
                    case S_TYPE:{                                   // for string declaration, we need to have {} for case
                        if(line.find('(') != string::npos){
                            // lw x1,4(x2)      // sw x1,4(x2)
                            div[1] = stoi(parts[1].substr(1));                           // for sw, wrong
                            string offsetStr = parts[2].substr(0, parts[2].find("("));
                            div[4] = stoi(offsetStr);
                            div[2] = stoi(parts[2].substr(parts[2].find("x") + 1));     // for sw, wrong
                        }
                        else{
                            // lw x1,base
                            div[1] = stoi(parts[1].substr(1));
                            div[3] = -1;     // to differentiate in execute, but stored 15 in encoding
                            div[4] = labels[parts[2] + ":"];
                        }
                        break;
                    }
                    case SB_TYPE:
                        // bne x1,x2,Label      // j Label
                        if(div[0] == 13){
                            div[4] = labels[parts[1] + ":"];
                        }
                        else{
                            div[2] = stoi(parts[1].substr(1));
                            div[3] = stoi(parts[2].substr(1));
                            div[4] = labels[parts[3] + ":"];               // ****HERE, IT'S BRANCH ADDRESS (pc)****
                        }
                        break;
                    case LABELS:
                        div[4] = labels[parts[0]];            //div[0],1,2,3 = 0
                        break;
                    case ECALL:
                        break;
                    case UNKNOWN:
                        div[0] = -1;                // -1 stored as 15
                        break;
                }
                encodeInstToMem(div);
                //pc+=4;
            }
        }

        void Core:: split(string line){
            // vector<string> parts;
            parts.clear();
            int t=0;
            for(int i=0;i<line.length();i++){
                if(line[i]==' ' || line[i]==','){
                    if(i>t){
                        parts.push_back(line.substr(t, i-t));    // splits into parts
                    }
                    t=i+1;
                }
            }
            parts.push_back(line.substr(t));
        }

        string Core:: trim(string& str) {
            size_t first = str.find_first_not_of(' ');
            if (string::npos == first) {
                return str;
            }
            size_t last = str.find_last_not_of(' ');
            return str.substr(first, (last - first + 1));
        }

        InstType Core:: getInstType(string &inst) {
            if (inst == "add" || inst == "sub") return R_TYPE;
            if (inst == "addi" || inst == "slli" || inst == "srli" || inst == "li") return I_TYPE;
            if (inst == "lw" || inst == "sw" || inst == "la") return S_TYPE;
            if (inst == "bne" || inst == "beq" || inst == "ble" || inst == "bgt" || inst == "j") return SB_TYPE;
            if (inst.find(":") != string::npos) return LABELS;
            if (inst == "ecall") return ECALL;
            return UNKNOWN;             // .word, .text
        }

        int Core:: Opcode(string op){
            if(op=="add")           return 1;
            else if(op=="sub")      return 2;
            else if(op=="addi")     return 3;
            else if(op=="slli")     return 4;
            else if(op=="srli")     return 5;
            else if(op=="li")       return 6;
            else if(op=="lw" || op=="la")       return 7;
            else if(op=="sw")       return 8;
            else if(op=="bne")      return 9;
            else if(op=="beq")      return 10;
            else if(op=="ble")      return 11;
            else if(op=="bgt")      return 12;
            else if(op=="j")        return 13;
            else if(op=="ecall")    return 14;      // -1(means 15) for unknown like .word
            else                    return 0;       // Label:
        }

        void Core:: setMemory(int wordAddress, int value) {  
            memory[wordAddress] = value;
        }

        void Core:: encodeInstToMem(int arr[]){
            stringstream ss;
            bitset<4> bits1(arr[0]);
            ss << bits1.to_string();
            for(int i=1;i<3;i++){
                bitset<5> bits2(arr[i]);
                ss << bits2.to_string();
            }
            bitset<4> bits3(arr[3]);
            ss << bits3.to_string();
            bitset<13> bits4(arr[4]);
            ss << bits4.to_string();

            uint32_t encoded = 0; 
            encoded = stoi(ss.str(), 0, 2);     // binary to decimal
            static int index = 0;
            setMemory(index, encoded);
            index++;
            // cout<<ss.str()<<endl;
            // cout<<encoded<<endl;
        }

        pair<int, int> Core:: execute(){
            ofstream outFile("assets/output.txt");
             if (!outFile.is_open()) {
                cout << "Failed to open output.txt for writing." << endl;
                return pair<int,int>();
            }
            pair<int, int> CycInst;         // for cycles, instructions executed
            int InstBase = 0;               // memory address
            do{
                string binInst = decodeMemToInst((memory[InstBase]));
                int opcode = stoi(binInst.substr(0, 4), 0, 2);
                int rd = stoi(binInst.substr(4, 5), 0, 2);
                int rs1 = stoi(binInst.substr(9, 5), 0, 2);
                int rs2 = stoi(binInst.substr(14, 4), 0, 2);
                int imm = stoi(binInst.substr(18, 13), 0, 2);
                //cout<<opcode<<" "<<rd<<" "<<rs1<<" "<<rs2<<" "<<imm<<endl;

                switch(opcode){
                    case 1: setRegister(rd, getRegister(rs1) + getRegister(rs2));       InstBase++; (CycInst.second)++; break;
                    case 2: setRegister(rd, getRegister(rs1) - getRegister(rs2));       InstBase++; (CycInst.second)++; break;
                    case 3: setRegister(rd, getRegister(rs1) + imm);                    InstBase++; (CycInst.second)++; break;
                    case 4: setRegister(rd, getRegister(rs1) << imm);                   InstBase++; (CycInst.second)++; break;
                    case 5: setRegister(rd, getRegister(rs1) >> imm);                   InstBase++; (CycInst.second)++; break;
                    case 6: setRegister(rd, imm);                                       InstBase++; (CycInst.second)++; break;
                    case 7:
                        if(rs2 == 15){              // lw x1,base
                            setRegister(rd, imm);
                        }
                        else{
                            setRegister(rd, getMemory(getRegister(rs1) + (imm/4)));
                        }     
                        InstBase++;
                        (CycInst.second)++;
                        break;
                    case 8: setMemory((getRegister(rs1) + (imm/4)), getRegister(rd));   InstBase++; (CycInst.second)++; break;      // rd and rs1 reverse for sw
                    case 9:
                        if(getRegister(rs1) != getRegister(rs2)){
                            InstBase = imm;                 // imm has branch address for branch and jump
                        }
                        else{
                            InstBase++;
                        }
                        (CycInst.second)++;
                        break;
                    case 10:
                        if(getRegister(rs1) == getRegister(rs2)){
                            InstBase = imm;
                        }
                        else{
                            InstBase++;
                        }
                        (CycInst.second)++;
                        break;
                    case 11:
                        if(getRegister(rs1) <= getRegister(rs2)){
                            InstBase = imm;
                        }
                        else{
                            InstBase++;
                        }
                        (CycInst.second)++;
                        break;
                    case 12:
                        if(getRegister(rs1) > getRegister(rs2)){
                            InstBase = imm;
                        }
                        else{
                            InstBase++;
                        }
                        (CycInst.second)++;
                        break;
                    case 13: InstBase = imm;  (CycInst.second)++;  break;
                    case 14:
                        if(getRegister(17) == 1){
                            cout<<getRegister(10);
                        }
                        else if(getRegister(17) == 4){
                            string str = strings[getRegister(10)];
                            int i = str.length()-1;
                            if(str[i] == 'n' && str[i-1] == '\\'){
                                str.erase(i - 1, 2);        //remove \n
                                cout<<str<<endl;
                            }
                            else{
                                cout<<str;
                            }
                        }
                        InstBase++;
                        (CycInst.second)++;
                        break;
                    //case 15: setRegister(rd, imm);         InstBase++;    break;
                    default: InstBase++;        break;
                }
                outFile<<"Cycle: "<<CycInst.first<<endl;
                for (int j = 0; j < 32; j++) {
                    string reg = "x" + to_string(j);
                    outFile << registers[reg] << " ";
                }
                outFile << endl;
                for(int i=2048;i<2070;i++){
                    outFile<<i<<" "<<memory[i]<<"   ";
                }
                outFile<<endl<<endl;
                (CycInst.first)++;
            }while(memory[InstBase]!=0);

            outFile.close();
            return CycInst;
        }

        string Core:: decodeMemToInst(int decode){
            string binInst = "";
            for (int i = 30; i >= 0; i--) {
                int k = decode >> i;
                if (k & 1)
                    binInst += '1';
                else
                    binInst += '0';
            }
            //cout<<binInst<<endl;
            return binInst;
        }

        void Core:: setRegister(int n, int value) {
            if (n == 0) {
                registers["x0"] = 0;
            } else {
                string reg = "x" + to_string(n);
                registers[reg] = value;
            }
        }

        int Core:: getRegister(int n) {
            if (n == 0) {
                return 0;
            }
            string reg = "x" + to_string(n);
            return registers[reg];
        } 

        int Core:: getMemory(int wordAddress) {
            return memory[wordAddress];
        }

int main(){
    Core c;
    pair<int, int> CycInst;
    vector<string> program;
    int fileNo;
    cout << "Enter the filename (Available- BubbleSort.txt(1), SelectionSort.txt(2), Input.txt(3), Input1.txt(4), Input2.txt(5)): "<<endl;
    cin>>fileNo;
    string file;
    switch(fileNo){
        case 1: file = "BubbleSort.txt";     break;
        case 2: file = "SelectionSort.txt";  break;
        case 3: file = "Input.txt";          break;
        case 4: file = "Input1.txt";         break;
        case 5: file = "Input2.txt";         break;
        default: cout<<"unavailable"<<endl;  break;
    }
    string filepath = "assets/" + file;
    ifstream inputFile(filepath);
     if (!inputFile.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }    
    string line;
    while (getline(inputFile, line)) {
        program.push_back(line);
    }
    inputFile.close();

    c.parser(program);
    cout<<endl;
    cout<<"******************** \033[1mC O N S O L E\033[0m ********************"<<endl;
    CycInst = c.execute();
    cout<<endl;
    cout<<"*******************************************************"<<endl<<endl;
    cout<<"Total Instructions Retired: "<<(CycInst.second)<<endl;
    cout<<"Total Cycles: "<<(CycInst.first)<<endl;
    cout<<"CPI: "<<(float)(CycInst.first)/(CycInst.second)<<endl<<endl;

    // int i=0;
    // for(auto m:c.memory){
    //     cout<<i<<m<<" ";
    //     i++;
    // }
    cout<<"\033[4mFinal Memory Contents:\033[0m"<<endl;
    for(int i=0;i<25;i++){
        cout << setw(2) << i << ": " << setw(12) << c.memory[i] << "  |  " << i + 2048 << ": " << setw(4) << c.memory[i + 2048] << endl;
    }
    cout<<endl;
    cout<<"\033[4mFinal Register Contents:\033[0m"<<endl;
    for (int j = 0; j < 32; j++) {
        string reg = "x" + to_string(j);
        cout << "Reg[" << setw(2) << j << "]:   " << setw(4) << c.registers[reg] << endl;
    }
    cout<<endl<<endl;
    return 0;
}