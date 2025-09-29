
#include <iostream>
#include <cassert>
#include <sstream>
#include <iomanip>
#include "alu.hpp"

static std::string to_hex(uint16_t x){ std::ostringstream os; os<<std::hex<<std::uppercase<<std::setw(4)<<std::setfill('0')<<x; return os.str(); }
static std::string to_bin(uint16_t x){ std::string s(16,'0'); for(int i=0;i<16;++i) if(x&(1<<(15-i))) s[i]='1'; return s; }

// function that changes the unsigned integer into its signed two's complement representation
static int16_t to_twoComp(uint16_t x){ 
    // if the most significant bit is zero, then it is already positive
    if ((x & 0x8000) == 0) {
        return static_cast<int16_t>(x);
    } else {
    // else it is a negative number we do two's complement
    // this inverts the bits
    uint16_t inverted = ~x;
    // then this adds one to the value
    uint16_t twosComp = inverted + 1;
    
    // this returns the integer as the negative it is
    return -static_cast<int16_t>(twosComp); 
    }
}

// function that will display the breakdown of each test
void run_test(const std::string &name, uint16_t expected, uint16_t result, ALUFlags flags, ALUFlags expectedFlags) {
    std::cout << "[" << name << "] Result 0x" << to_hex(result) << " | Flags: Z=" << flags.Z
    << " N=" << flags.N << " C=" << flags.C << " V=" << flags.V << " | Expected: 0x" << to_hex(expected);
    std::cout<< " | Expected flags: Z=" << expectedFlags.Z << " N=" << expectedFlags.N << " C=" << expectedFlags.C 
    << " V=" << expectedFlags.V << std::endl;
    
    // assert to make sure that the result is what was expected
    assert(result == expected); 
    assert( (flags.Z == expectedFlags.Z) && (flags.N == expectedFlags.N) && (flags.C == expectedFlags.C) && (flags.V == expectedFlags.V) );
}

// function that runs all the test functions
void tests(){
    // this tests the ALU wrap-around addition  
    auto [r1,f1]=ALU::add(0xFFFF,0x0001); 
    // this creates expected flags that I can pass into run_test
    ALUFlags expected1 = {true, false, true, false}; // Z, N, C, V
    // this runs the function that will display the breakdown of the test.
    run_test("ADD 0xFFFF + 0x0001", 0x0000, r1,f1, expected1);
    
    // this tests the ALU's basic addition
    auto [r2,f2]=ALU::add(0x0001,0x0001);
    ALUFlags expected2 = {false, false, false, false};
    run_test("ADD 0x0001 + 0x0001",0x0002,r2,f2, expected2);
    
    // this tests the ALU negative addition 
    auto [r3,f3]=ALU::add(0x8000,0xFFFF);
    ALUFlags expected3 = {false, false, true, true};
    run_test("ADD 0x8000 + 0xFFFF",0x7FFF,r3,f3, expected3);
   
    // this tests the ALU signed overflow when adding
    auto [r4,f4]=ALU::add(0x7FFF,0x0001); 
    ALUFlags expected4 = {false, true, false, true};
    run_test("ADD 0x7FFF + 0x0001",0x8000, r4,f4, expected4);
    
    // this tests the ALU wrap-around subtraction (when borrowing is required)
    auto [r5,f5]=ALU::sub(0x0000,0x0001); 
    ALUFlags expected5 = {false, true, false, false};
    run_test("SUB 0x0000 - 0x0001", 0xFFFF, r5, f5, expected5);
    
    // this tests the ALU when it subtracts the same values
    auto [r6,f6]=ALU::sub(0x0001,0x0001); 
    ALUFlags expected6 = {true, false, true, false};     // Z, N, C, V
    run_test("SUB 0x0001 - 0x0001", 0x0000, r6, f6, expected6);
    
    // this tests the ALU when there is signed underflow
    auto [r7,f7]=ALU::sub(0x8000,0x0001);
    ALUFlags expected7 = {false,false, true, true};
    run_test("SUB 0x8000 - 0x0001", 0x7FFF, r7,f7, expected7);
    
    // this tests the ALU when no borrow happened
    auto [r8,f8]=ALU::sub(0x0005,0x0003); 
    ALUFlags expected8 = {false, false, true, false};
    run_test("SUB 0x0005 - 0x0003", 0x0002, r8,f8, expected8);
    
    // this tests the ALU's AND logic 
    auto [r9,f9]=ALU::logic_and(0xF0F0, 0x0FF0);
    ALUFlags expected9 = {false, false, false, false};
    run_test("0xF0F0 AND 0x0FF0", 0x00F0, r9, f9, expected9);
     
    // this tests the ALU's OR logic
    auto [r10,f10]=ALU::logic_or(0xF0F0, 0x0FF0); 
    ALUFlags expected10 = {false,true,false,false};
    run_test("0xF0F0 OR 0x0FF0", 0xFFF0, r10,f10, expected10);
    
    // this tests the ALU's XOR logic
    auto [r11,f11]=ALU::logic_xor(0xAAAA, 0x5555); 
    ALUFlags expected11 = {false,true,false,false};
    run_test("0xAAAA XOR 0x5555", 0xFFFF, r11, f11, expected11);
    
    // this tests the ALU's NOT logic
    auto [r12,f12]=ALU::logic_not(0x0000); 
    ALUFlags expected12 = {false, true, false, false};
    run_test("NOT 0x0000", 0xFFFF, r12,f12, expected12);
    
    // this tests the ALU's left shift no carry
    auto [r13,f13]=ALU::shl(0x0001,1);
    ALUFlags expected13 = {false, false, false, false};
    run_test("SHL 0x0001, 1", 0x0002, r13,f13, expected13);
   
    // this tests the ALU's left shift with carry
    auto [r14,f14]=ALU::shl(0x8000,1);
    ALUFlags expected14 = {true, false, true, false};    // Z, N, C, V
    run_test("SHL 0x8000, 1", 0x0000, r14,f14, expected14);
    
    // this tests the ALU's right shift with carry
    auto [r15,f15]=ALU::shr(0x8000,1); 
    ALUFlags expected15 = {false, false, false, false};
    run_test("SHR 0x8000, 1", 0x4000, r15,f15, expected15);
    
    // this tests the ALU's right shift of one
    auto [r16,f16]=ALU::shr(0x0001,1); 
    ALUFlags expected16 = {true, false, true, false};
    run_test("SHR 0x0001, 1", 0x0000, r16,f16, expected16);
    
    // this tests the ALU's compare when comparing same integer
    auto [r17,f17]=ALU::cmp(0x0005,0x0005); 
    ALUFlags expected17 = {true, false, true, false};
    run_test("CMP 0x0005 TO 0x0005", 0x0000, r17,f17, expected17);
   
    // this tests the ALU's compare when borrowing is required for subtraction
    auto [r18,f18]=ALU::cmp(0x0001,0x0002); 
    ALUFlags expected18 = {false, true, false, false};
    run_test("CMP 0x0001 TO 0x0002", 0xFFFF, r18,f18, expected18);
    
    // this tests the ALU's compare when no borrowing is required for subtraction
    auto [r19,f19]=ALU::cmp(0x0002,0x0001); 
    ALUFlags expected19 = {false, false, true, false};
    run_test("CMP 0x0002 TO 0x0001", 0x0001, r19,f19, expected19);
    
    // edge cases
    
    // this tests the ALU when two maximums are added
    auto [r20,f20]=ALU::add(0xFFFF,0xFFFF); 
    ALUFlags expected20 = {false, true, true, false};
    run_test("ADD 0xFFFF + 0xFFFF", 0xFFFE, r20,f20, expected20);
    
    // this tests the ALU when minimum negative is subtracted
    auto [r21,f21]=ALU::sub(0x8000,0x0001);
    ALUFlags expected21 = {false, false, true, true};
    run_test("SUB 0x8000 - 0x0001", 0x7FFF, r21,f21, expected21);
    
    // this tests when the ALU shifts left by zero
    auto [r22,f22]=ALU::shl(0x0001,0);
    ALUFlags expected22 = {false, false, false, false};
    run_test("SHL 0x0001, 0", 0x0001, r22,f22, expected22);
    
    // this test when the ALU shifts right by zero
    auto [r23,f23]=ALU::shr(0x0001,0);
    ALUFlags expected23 = {false, false, false, false};
    run_test("SHR 0x0001, 0", 0x0001, r23,f23, expected23);
    
    // this tests when the ALU NOT all 1's
    auto [r24,f24]=ALU::logic_not(0xFFFF); 
    ALUFlags expected24 = {true, false, false, false};
    run_test("NOT 0xFFFF", 0x0000, r24,f24, expected24);
    
    // this tests when the ALU shifts zero to the left
    auto [r25,f25]=ALU::shl(0x0000,5); 
    ALUFlags expected25 = {true, false, false, false};
    run_test("SHL 0x0000, 5", 0x0000, r25,f25, expected25);
    
    // this tests when the ALU shifts zero to the right
    auto [r26,f26]=ALU::shr(0x0000,3); 
    ALUFlags expected26 = {true, false, false, false};
    run_test("SHR 0x0000, 3", 0x0000, r26,f26, expected26);
    
    std::cout<<"[P1] ALU smoke tests passed.\n";
}



int main(int argc,char** argv){
    if(argc>1 && std::string(argv[1])=="--test"){ tests(); return 0; }
    std::cout<<"Enter number (dec or 0x/0b): ";
    std::string s; if(!std::getline(std::cin,s)) return 0;
    uint16_t v=0;
    if(s.rfind("0x",0)==0||s.rfind("0X",0)==0) v=(uint16_t)std::stoul(s,nullptr,16);
    else if(s.rfind("0b",0)==0||s.rfind("0B",0)==0){ for(char c: s.substr(2)){ v=(v<<1)|(c=='1'); } }
    else v=(uint16_t)std::stoul(s,nullptr,10);
    std::cout<<"HEX = "<<to_hex(v)<<" BIN = "<<to_bin(v)<<" Unsigned DEC = "<<std::dec<<v << std::endl;
    
    // added a extra cout to show the signed integer as well (for greater readability)
    std::cout << "Signed DEC (two's complement) = "<<to_twoComp(v)<<"\n";
    return 0;
}
