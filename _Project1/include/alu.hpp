
#pragma once
#include <cstdint>
#include <tuple>
struct ALUFlags { bool Z=false, N=false, C=false, V=false; };
class ALU {
public:
    static std::tuple<uint16_t,ALUFlags> add(uint16_t a, uint16_t b);
    static std::tuple<uint16_t,ALUFlags> sub(uint16_t a, uint16_t b);
    static std::tuple<uint16_t,ALUFlags> logic_and(uint16_t a, uint16_t b);
    static std::tuple<uint16_t,ALUFlags> logic_or(uint16_t a, uint16_t b);
    static std::tuple<uint16_t,ALUFlags> logic_xor(uint16_t a, uint16_t b);
    static std::tuple<uint16_t,ALUFlags> logic_not(uint16_t a);
    static std::tuple<uint16_t,ALUFlags> shl(uint16_t a, int sh);
    static std::tuple<uint16_t,ALUFlags> shr(uint16_t a, int sh);
    static std::tuple<uint16_t,ALUFlags> cmp(uint16_t a, uint16_t b);
};
