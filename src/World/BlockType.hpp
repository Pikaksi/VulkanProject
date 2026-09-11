#pragma once

#include <stdint.h>
#include <functional>

struct BlockType {
    enum Value : int8_t
    {
        air = 0,
        stone = 1,
        grass = 2,
        dirt = 3,
        oakLog = 4,
        oakLeaf = 5,
        grassPlant = 6,
        furnace = 7,
        drill = 8,
        pipe = 9,
        pipeIn = 10,
        pipeOut = 11,

        maxEnum = 12,
    };
    Value v;
    constexpr BlockType() : v((Value)0) {}
    constexpr BlockType(Value val) : v(val) {}
    constexpr BlockType(int val) : v((Value)val) {}
    constexpr operator Value() const { return v; }
};

namespace std {
    template <>
    struct hash<BlockType> {
        size_t operator()(const BlockType& a) const {
            return std::hash<BlockType::Value>()(a.v);
        }
    };
}
