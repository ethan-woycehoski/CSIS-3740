// CSIS 3740 – Project 3 (Advanced): MARIE Micro-Computer System
// Part C (Trace & Stats) is scaffolded below. Part A/B require students to fill TODOs.
// Build: g++ -std=c++17 -O2 simulator/marie_core.cpp -o marie
// Run:   ./marie examples/loop.txt --trace --save-trace trace.txt
// Programmed by: Ethan Woycehoski

#include <bits/stdc++.h>
using namespace std;

struct Marie {
    static constexpr uint16_t MEM_SIZE = 4096;
    // 16-bit registers (AC, MBR are signed view for arithmetic)
    int16_t  AC = 0, MBR = 0;
    uint16_t PC = 0, IR = 0, MAR = 0;
    array<uint16_t, MEM_SIZE> MEM{};

    bool trace_console = false;
    ofstream trace_file;
    // Stats
    unordered_map<string, uint64_t> icount;

    static uint16_t HI4(uint16_t w){ return (w >> 12) & 0xF; }
    static uint16_t LO12(uint16_t w){ return w & 0x0FFF; }
    static string HEX4(uint16_t x){ stringstream s; s<<uppercase<<hex<<setw(4)<<setfill('0')<<x; return s.str(); }

    void reset(){ AC=0;MBR=0;PC=0;IR=0;MAR=0; MEM.fill(0); icount.clear(); }

    bool loadProgram(istream& in){
        // Program format: hex word per line, @ADDR to set origin, ';' comment
        string line; uint16_t addr=0; size_t cnt=0;
        while(getline(in,line)){
            auto p = line.find_first_of(";#"); if(p!=string::npos) line=line.substr(0,p);
            auto isws=[](unsigned char c){return isspace(c);};
            line.erase(line.begin(), find_if(line.begin(), line.end(), [&](char ch){return !isws(ch);}));
            line.erase(find_if(line.rbegin(), line.rend(), [&](char ch){return !isws(ch);} ).base(), line.end());
            if(line.empty()) continue;
            if(line[0]=='@'){ addr = stoul(line.substr(1),nullptr,16)&0x0FFF; continue; }
            uint16_t w = stoul(line,nullptr,16)&0xFFFFu;
            if(addr>=MEM_SIZE) { cerr<<"Address OOR\n"; return false; }
            MEM[addr++] = w; cnt++;
        }
        return cnt>0;
    }

    void openTraceFile(const string& path){
        if(!path.empty()) trace_file.open(path);
        if(trace_console) {
            cout<<"Cycle | PC   IR   AC    MAR  MBR\n---------------------------------\n";
        }
        if(trace_file.is_open()){
            trace_file<<"Cycle | PC   IR   AC    MAR  MBR\n---------------------------------\n";
        }
    }

    void traceRow(uint64_t cyc){
        auto row = [&](ostream& os){
            os<<setw(5)<<cyc<<" | "<<HEX4(PC)<<' '<<HEX4(IR)<<' '
              <<HEX4(static_cast<uint16_t>(AC))<<' '<<HEX4(MAR)<<' '<<HEX4(static_cast<uint16_t>(MBR))<<"\n";
        };
        if(trace_console) row(cout);
        if(trace_file.is_open()) row(trace_file);
    }

    bool step(uint64_t cyc){
        // FETCH
        MAR = PC; if(MAR>=MEM_SIZE){ cerr<<"PC out of range\n"; return false; }
        MBR = static_cast<int16_t>(MEM[MAR]); IR = static_cast<uint16_t>(MBR);
        PC = static_cast<uint16_t>((PC+1)&0x0FFF);
        uint16_t op = HI4(IR), adr = LO12(IR);
        bool cont = true;

        // EXECUTE
        switch(op){
            case 0x1: icount["LOAD"]++;  MAR=adr; MBR=static_cast<int16_t>(MEM[MAR]); AC=MBR; break;
            case 0x2: icount["STORE"]++; MAR=adr; MEM[MAR]=static_cast<uint16_t>(AC); MBR=AC; break;
            case 0x3: icount["ADD"]++;   MAR=adr; MBR=static_cast<int16_t>(MEM[MAR]); AC=static_cast<int16_t>(AC+MBR); break;
            case 0x4: icount["SUBT"]++;  MAR=adr; MBR=static_cast<int16_t>(MEM[MAR]); AC=static_cast<int16_t>(AC-MBR); break;
            case 0x5: icount["INPUT"]++; cout<<"Input: "; cin>>AC; MBR=AC; break;
            case 0x6: icount["OUTPUT"]++; cout<<"Output: "<<AC<<"\n"; MBR=AC; break;
            case 0x7: icount["HALT"]++;  cont=false; break;
            case 0x9: icount["JUMP"]++;  PC = adr; break;
            case 0x8: { 
            	// SKIPCOND —— TODO (Phase A)
                // Top 3 bits of address field (bits 11..9) are condition code:
                //   000 -> AC < 0
                //   400 -> AC == 0
                //   800 -> AC > 0
                // If condition holds, skip next instruction: PC = PC + 1
                // ---- YOUR CODE HERE ----

                // set MAR and MBR for tracing
                MAR = PC - 1;
                MBR = static_cast<int16_t>(IR);

                uint16_t cond = adr & 0x0E00;
                bool take = false;

                // check accumulator against condition
                if (cond == 0x000)       take = (AC < 0);   // LT
                else if (cond == 0x0400) take = (AC == 0);  // ZE
                else if (cond == 0x0800) take = (AC > 0);   // GT
                else {
                    cerr << "Warning: invalid SKIPCOND condition 0x" << hex << cond << dec << endl;
                }

                // Skip next instruction if condition holds
                if (take) PC = (PC + 1) & 0x0FFF;

                icount["SKIPCOND"]++; // keep stats even if not implemented yet
                break;
            }
            // ===== Extra instruction (Phase A extension) =====
            // Choose ONE and implement:
            //   CLEAR (opcode B): AC <- 0
            //   ADDI  (opcode C): AC <- AC + M[M[adr]]  (indirect)
            //   JUMPI (opcode D): PC <- M[adr]
            // Add encoding in your assembler as well.
            case 0xB: // CLEAR function 
            	 // keeps track of how much CLEAR is run
                 icount["CLEAR"]++;
                 
                 // reset the accumulator
                 AC = 0;
                 
                 // set MBR to 0 for trace output
                 MBR = 0;
                 
                 // set MAR to instruction address for trace
                 MAR = PC - 1;
                 break;
            case 0xC: /* ADDI function  performs indirect addition */ 
            	 icount["ADDI"]++;	// keeps track of how much ADDI is run
            	 MAR = adr;		// address part of instruction
            	 MBR = static_cast<int16_t>(MEM[MAR]);	// address stored at MAR
            	 MAR = MBR;		// use it as the address
            	 MBR = static_cast<int16_t>(MEM[MAR]);	// load the actual value indirectly
            	 AC = static_cast<int16_t>(AC+MBR);		// add to accumulator
            
                 break;
            case 0xD: /* JUMPI function allows indirect jumps */ 
            	 icount["JUMPI"]++;	// track how many times JUMPI runs
            	 MAR = adr;		// hold the MBR address
            	 MBR = static_cast<int16_t>(MEM[MAR]);	// MBR now holds the real address to jump to
            	 PC = MBR;		// update PC to the address found in memory
            
            	 break;

            default: cerr<<"Unsupported opcode 0x"<<hex<<uppercase<<op<<dec<<"\n"; return false;
        }

        traceRow(cyc);
        return cont;
    }

    void run(){
        uint64_t cyc=1;
        openTraceFile(""); // will be re-opened from main if --save-trace was passed
        while(step(cyc++)) { if(cyc>1000000){ cerr<<"Cycle limit.\n"; break; } }
        cout<<"Program halted.\n";
        if(trace_file.is_open()) trace_file.close();
    }
};

int main(int argc, char** argv){
    if(argc<2){ cerr<<"Usage: "<<argv[0]<<" program.txt [--trace] [--save-trace file]\n"; return 1; }
    string program = argv[1];
    bool trace = false;
    string outTrace = "";
    for(int i=2;i<argc;i++){
        string a = argv[i];
        if(a=="--trace") trace=true;
        if(a=="--save-trace" && i+1<argc) { outTrace = argv[++i]; }
    }

    ifstream fin(program);
    if(!fin){ cerr<<"Cannot open "<<program<<"\n"; return 1; }

    Marie m; m.reset(); m.trace_console = trace;
    if(!m.loadProgram(fin)){ cerr<<"Load failed.\n"; return 1; }

    // Set trace file if requested
    if(!outTrace.empty()) m.openTraceFile(outTrace);
    else m.openTraceFile("");

    uint64_t cyc=1;
    if(m.trace_console || m.trace_file.is_open()){
        // Print header once
        if(m.trace_console) { cout<<"Cycle | PC   IR   AC    MAR  MBR\n---------------------------------\n"; }
        if(m.trace_file.is_open()) { m.trace_file<<"Cycle | PC   IR   AC    MAR  MBR\n---------------------------------\n"; }
    }
    while(m.step(cyc++)) { if(cyc>1000000){ cerr<<"Cycle limit.\n"; break; } }
    cout<<"Program halted.\n";
    if(m.trace_file.is_open()) m.trace_file.close();
    return 0;
}
