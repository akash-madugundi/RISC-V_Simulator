#include "Core.h"

class Processor{
    public:
        vector<int> memory;
        vector<Core> cores;
        vector<string> program1, program2;
        Processor(){
            memory=vector<int>(4096,0);
            cores={Core(), Core()};
        }

        void run(){
            pair<int, int> CycInst;
            vector<string> program;
            for(int c=0; c<2; c++){
                cout << "=====================================================================" << endl;
                cout << "                               \033[4m\033[1mCORE:\033[0m\033[0m "<<c<<endl;
                cout << "=====================================================================" << endl;
                if(c == 0){
                    program = program1;
                }
                else{
                    program = program2;
                }
                cores[c].parser(program, c, memory);

                cout<<endl;
                cout<<"******************** \033[1mC O N S O L E\033[0m ********************"<<endl;
                CycInst = cores[c].execute(c, memory);
                cout<<endl;
                cout<<"*******************************************************"<<endl<<endl;
                cout<<"Total Instructions Retired: "<<(CycInst.second)<<endl;
                cout<<"Total Cycles: "<<(CycInst.first)<<endl;
                cout<<"CPI: "<<(float)(CycInst.first)/(CycInst.second)<<endl<<endl;

                cout<<"\033[4mFinal Memory Contents:\033[0m"<<endl;
                int start = (c == 0) ? 0 : 1024;
                int end = (c == 0) ? 25 : 1049;
                for (int i = start; i < end; i++){
                    cout << setw(2) << i << ": " << setw(12) << memory[i] << "  |  " << i + 2048 << ": " << setw(4) << memory[i + 2048] << endl;
                }
                cout<<endl;
                cout<<"\033[4mFinal Register Contents:\033[0m"<<endl;
                for (int j = 0; j < 32; j++) {
                    string reg = "x" + to_string(j);
                    cout << "Reg[" << setw(2) << j << "]:   " << setw(4) << cores[c].registers[reg] << endl;
                }
                cout<<endl;
                cout<<"---------------------------------------------------------------------------------------------------------------------------------------------"<<endl;
                cout<<"                                    <<<<<<<<<< Each Cycle Contents in `output"<<(c+1)<<".txt` in assets >>>>>>>>>>"<<endl;
                cout<<"---------------------------------------------------------------------------------------------------------------------------------------------"<<endl<<endl;
            }

            // int i=0;
            // for(auto m:memory){
            //     cout<<i<<m<<" ";
            //     i++;
            // }
        }
};

int main(){
    Processor p;
    int fileNo1, fileNo2;
    cout<<endl;
    cout << "Enter the filenumber for \033[1mCore-0\033[0m (Available- BubbleSort.txt(1), SelectionSort.txt(2), Input.txt(3), Input1.txt(4), Input2.txt(5)): "<<endl;
    cout << "\033[3m(recommended: Core-0 for BubbleSort(1) and Core-1 for SelectionSort(2))\033[0m" <<endl;
    cin>>fileNo1;
    cout << "Enter the filenumber for \033[1mCore-1\033[0m (Available- BubbleSort.txt(1), SelectionSort.txt(2), Input.txt(3), Input1.txt(4), Input2.txt(5)): "<<endl;
    cin>>fileNo2;
    cout<<endl;
    string file1, file2, filepath, line;
    switch(fileNo1){
        case 1: file1 = "BubbleSort.txt";     break;
        case 2: file1 = "SelectionSort.txt";  break;
        case 3: file1 = "Input.txt";          break;
        case 4: file1 = "Input1.txt";         break;
        case 5: file1 = "Input2.txt";         break;
        default: cout<<"unavailable"<<endl;  break;
    }
    filepath = "assets/" + file1;
    ifstream inputFile1(filepath);
    if (!inputFile1.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }    
    while (getline(inputFile1, line)) {
        p.program1.push_back(line);
    }
    inputFile1.close();

    switch(fileNo2){
        case 1: file2 = "BubbleSort.txt";     break;
        case 2: file2 = "SelectionSort.txt";  break;
        case 3: file2 = "Input.txt";          break;
        case 4: file2 = "Input1.txt";         break;
        case 5: file2 = "Input2.txt";         break;
        default: cout<<"unavailable"<<endl;  break;
    }
    filepath = "assets/" + file2;
    ifstream inputFile2(filepath);
    if (!inputFile2.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }    
    while (getline(inputFile2, line)) {
        p.program2.push_back(line);
    }
    inputFile2.close();
    
    p.run();
    return 0;
}
