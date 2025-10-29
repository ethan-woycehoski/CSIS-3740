// CSIS 3740 – Project 3 (Advanced): Minimal MARIE Assembler (Starter)
// Goal: Students complete encoding in pass2() for all required mnemonics.
// Build: g++ -std=c++17 -O2 assembler/assembler_starter.cpp -o marie_asm
// Run:   ./marie_asm examples/loop.asm > examples/loop.txt
// Programmed by: Ethan Woycehoski

#include <bits/stdc++.h>
using namespace std;

// Supported baseline mnemonics: LOAD, STORE, ADD, SUBT, INPUT, OUTPUT, HALT, JUMP, SKIPCOND
// Extra (choose ONE and implement in both assembler and simulator): CLEAR/ADDI/JUMPI
// Labels end with ':'; DEC defines constants:  NAME, DEC 5
// Address field is 12 bits. Output is 16-bit hex words, one per line. '@ADDR' for ORG emitted automatically at start.

struct AsmLine { string label, op, arg, raw; int lineno; };

static string trim(const string& s){
    size_t i=0,j=s.size();
    while(i<j && isspace((unsigned char)s[i])) i++;
    while(j>i && isspace((unsigned char)s[j-1])) j--;
    return s.substr(i,j-i);
}

vector<AsmLine> tokenize(istream& in){
    vector<AsmLine> v; string line; int ln=0;
    while(getline(in,line)){
        ln++;
        auto p=line.find_first_of(";#"); if(p!=string::npos) line=line.substr(0,p);
        line=trim(line); if(line.empty()) continue;
        AsmLine a; a.raw=line; a.lineno=ln;
        // label?
        if(line.find(':')!=string::npos){
            auto pos=line.find(':');
            a.label=trim(line.substr(0,pos)); line=line.substr(pos+1);
        }
        stringstream ss(line);
        ss>>a.op; string rest; getline(ss, rest); a.arg=trim(rest);
        // support "NAME, DEC 5"
        if(a.op.empty() && !a.label.empty()){ // label-only line
            // leave op empty
        }
        v.push_back(a);
    }
    return v;
}

struct Assembler {
    unordered_map<string,uint16_t> sym;
    vector<AsmLine> IR;
    uint16_t loc=0;

    bool pass1(){
        loc=0;
        for(auto& a: IR){
            if(!a.label.empty()){
                if(sym.count(a.label)){ cerr<<"Duplicate label: "<<a.label<<"\n"; return false; }
                sym[a.label]=loc;
            }
            if(a.op=="DEC"){
                loc += 1;
            }else{
                loc += 1; // each instruction/data occupies one word
            }
        }
        return true;
    }

    static uint16_t addr12(uint16_t x){ return x & 0x0FFF; }
    static uint16_t op4(uint16_t x){ return (x & 0xF) << 12; }

    bool pass2(ostream& out){
        uint16_t pc=0;
        for(auto& a: IR){
            uint16_t word=0;
            string op=a.op;
            // handle empty op with only label -> treat as NOP (0x0000) or error
            if(op.empty()){ word=0x0000; out<<uppercase<<hex<<setw(4)<<setfill('0')<<word<<"\n"; pc++; continue; }

            // data
            if(op=="DEC"){
                int val = stoi(a.arg); // allow negative
                word = static_cast<uint16_t>(val);
                out<<uppercase<<hex<<setw(4)<<setfill('0')<<word<<"\n"; pc++; continue;
            }

            // resolve address if arg is a symbol or hex number
            auto resolve = [&](const string& s)->uint16_t{
                if(s.empty()) return 0;
                if(sym.count(s)) return sym[s];
                // hex like 0x00AB or plain decimal
                try{
                    if(s.size()>2 && (s[0]=='0') && (s[1]=='x' || s[1]=='X'))
                        return static_cast<uint16_t>(stoul(s,nullptr,16));
                    if(all_of(s.begin(), s.end(), ::isxdigit) && !isdigit(s[0])) return 0; // conservative
                    // allow plain hex without 0x
                    if(all_of(s.begin(), s.end(), ::isxdigit)) return static_cast<uint16_t>(stoul(s,nullptr,16));
                    return static_cast<uint16_t>(stoul(s,nullptr,10));
                }catch(...){
                    cerr<<"Cannot resolve address: '"<<s<<"' (line "<<a.lineno<<")\n"; return 0;
                }
            };

            // map opcodes
            auto A = [&](uint16_t opc, const string& arg){ return op4(opc) | addr12(resolve(arg)); };

            if(op=="LOAD")   word = A(0x1, a.arg);
            else if(op=="STORE")  word = A(0x2, a.arg);
            else if(op=="ADD")    word = A(0x3, a.arg);
            else if(op=="SUBT")   word = A(0x4, a.arg);
            else if(op=="INPUT")  word = A(0x5, "0");
            else if(op=="OUTPUT") word = A(0x6, "0");
            else if(op=="HALT")   word = A(0x7, "0");
            else if(op=="JUMP")   word = A(0x9, a.arg);
            else if(op=="SKIPCOND"){
                // TODO (Phase A): encode condition from arg tokens to bits 11..9
                // Accept forms:
                //   SKIPCOND LT     -> 0x8 000
                //   SKIPCOND ZE     -> 0x8 400
                //   SKIPCOND GT     -> 0x8 800
                // or numeric forms: SKIPCOND 0 / 400 / 800
                // ---- YOUR CODE HERE ----

                uint16_t condBits = 0;

                string input = a.arg;

                // for loop to handle lowercase to uppercase 
                for (auto &c : input) c = toupper(c);

                // if AC < 0
                if(input == "LT" || input == "000" || input == "0") {
                    // AC < 0
                    condBits = 0x000; 
                } else if (input == "ZE" || input == "400" || input == "0400") {
                    // AC = 0
                    condBits = 0x400; 
                } else if (input == "GT" || input == "800" || input == "0800") {
                    // AC > 0
                    condBits = 0x800;
                } else {
                    // error message to display a invalid argument inputted
                    cerr << "Invalid SKIPCOND argument " << a.arg << "at line " << a.lineno << "\n";
                }
                // opcode in top 4 bits, cond bits in low 12
                word = op4(0x8) | condBits;
                // encode condition into bits 11..9 (as low 12-bit field)
                string s = a.arg;
                for (auto &c : s) c = toupper((unsigned char)c);

                if (s == "LT") {
                    condBits = 0x000; // skip if AC < 0
                } else if (s == "ZE") {
                    condBits = 0x0400; // skip if AC == 0
                } else if (s == "GT") {
                    condBits = 0x0800; // skip if AC > 0
               } else {
                    // allow numeric forms (decimal or hex like 0x400)
                    try {
                        uint16_t v = 0;
                        if (!s.empty() && s.size() > 2 && s[0]=='0' && (s[1]=='x' || s[1]=='X'))
                            v = static_cast<uint16_t>(stoul(s,nullptr,16));
                        else
                            v = static_cast<uint16_t>(stoul(s,nullptr,10));
                        // accept only the three valid encodings
                       if (v==0 || v==0x0400 || v==0x0800) condBits = v;
                       else cerr<<"Invalid SKIPCOND numeric value '"<<a.arg<<"' at line "<<a.lineno<<"\n";
                   } catch(...) {
                       cerr<<"Invalid SKIPCOND argument '"<<a.arg<<"' at line "<<a.lineno<<"\n";
                    }
                }
                word = op4(0x8) | addr12(condBits);
            }
            // CLEAR sets the accumulator (AC) to zero 
            else if(op=="CLEAR"){ 
                word = A(0xB, "0"); 
            }
            // ADDI adds indirectly 
            else if(op=="ADDI"){  
                word = A(0xC, a.arg);
            }
            // JUMPI indirectly jumps 
            else if(op=="JUMPI"){ 
                word = A(0xD, a.arg);
            }
            else {
                cerr<<"Unknown opcode '"<<op<<"' at line "<<a.lineno<<"\n";
            }

            out<<uppercase<<hex<<setw(4)<<setfill('0')<<word<<"\n";
            pc++;
        }
        return true;
    }
};

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if(argc<2){ cerr<<"Usage: "<<argv[0]<<" program.asm\n"; return 1; }
    ifstream fin(argv[1]); if(!fin){ cerr<<"Cannot open "<<argv[1]<<"\n"; return 1; }
    auto toks = tokenize(fin);
    Assembler as; as.IR = std::move(toks);
    if(!as.pass1()) return 1;
    if(!as.pass2(cout)) return 1;
    return 0;
}
