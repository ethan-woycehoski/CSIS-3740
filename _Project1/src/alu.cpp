
#include "alu.hpp"
#include <cstdint>

static ALUFlags mk(uint32_t r32, uint16_t r16, bool c, bool v){
    ALUFlags f; f.Z=(r16==0); f.N=(r16&0x8000)!=0; f.C=c; f.V=v; return f;
}

std::tuple<uint16_t,ALUFlags> ALU::add(uint16_t a, uint16_t b){
    uint32_t r=(uint32_t)a+(uint32_t)b; uint16_t q=(uint16_t)r;
    bool c=r>0xFFFF; bool v=((a^q)&(b^q)&0x8000)!=0; return {q, mk(r,q,c,v)};
}
std::tuple<uint16_t,ALUFlags> ALU::sub(uint16_t a, uint16_t b){
    uint32_t r=(uint32_t)a-(uint32_t)b; uint16_t q=(uint16_t)r;
    bool c=a>=b; bool v=((a^b)&(a^q)&0x8000)!=0; return {q, mk(r,q,c,v)};
}

std::tuple<uint16_t,ALUFlags> ALU::logic_and(uint16_t a,uint16_t b){ uint16_t q=a&b; return {q,mk(q,q,false,false)}; }
std::tuple<uint16_t,ALUFlags> ALU::logic_or (uint16_t a,uint16_t b){ uint16_t q=a|b; return {q,mk(q,q,false,false)}; }
std::tuple<uint16_t,ALUFlags> ALU::logic_xor(uint16_t a,uint16_t b){ uint16_t q=a^b; return {q,mk(q,q,false,false)}; }
std::tuple<uint16_t,ALUFlags> ALU::logic_not(uint16_t a){ uint16_t q=~a; return {q,mk(q,q,false,false)}; }
std::tuple<uint16_t,ALUFlags> ALU::shl(uint16_t a,int sh){ sh&=0x1F; uint32_t r=((uint32_t)a)<<sh; uint16_t q=(uint16_t)r; bool c=(sh>0)?((a>>(16-sh))&1):0; return {q,mk(r,q,c,false)}; }
std::tuple<uint16_t,ALUFlags> ALU::shr(uint16_t a,int sh){ sh&=0x1F; uint16_t q=(uint16_t)(a>>sh); bool c=(sh>0)?((a>>(sh-1))&1):0; return {q,mk(q,q,c,false)}; }
std::tuple<uint16_t,ALUFlags> ALU::cmp(uint16_t a,uint16_t b){ auto [q,f]=sub(a,b); return {q,f}; }
