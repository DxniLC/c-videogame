
#pragma once

#include "FunctionIDs.hpp"

#include <utility>

#include <unordered_map>

template <typename TypeRulesResult, typename RulesIDType>
struct RulesManager
{
    using RuleFunction = void (*)(TypeRulesResult &);

    explicit RulesManager(RuleFunction default_func)
    {
        standardFunc = default_func;
    }

    void addRule(const RulesIDType &ID_A, const FunctionID &ID_B, RuleFunction ruleFunc)
    {
        rulesFunctions[sortPair(ID_A, ID_B)] = ruleFunc;
    }

    void addRule(const RulesIDType &ID_A, const RulesIDType &ID_B, RuleFunction ruleFunc)
    {

        auto keyMap = std::pair(ID_A, ID_B);

        rules[keyMap] = ruleFunc;

        for (RulesIDType i = 0; i < number_bits; i++)
        {
            RulesIDType maskA = 1 << i;
            if (ID_A & maskA)
            {
                for (RulesIDType j = 0; j < number_bits; j++)
                {
                    RulesIDType maskB = 1 << j;
                    if (ID_B & maskB)
                    {
                        // Save bits pair
                        rulesBitsPair.emplace(sortPair(maskA, maskB), keyMap);
                    }
                }
            }
        }
    }

    void applyRule(TypeRulesResult &pts)
    {
        RulesIDType typeA{pts.entity_A->type};
        RulesIDType typeB{pts.entity_B->type};
        if (pts.functionID != FunctionID::NONEFUNC)
        {
            auto itr = rulesFunctions.find(sortPair(typeA, pts.functionID));
            if (itr != rulesFunctions.end())
            {
                itr->second(pts);
                return;
            }
        }

        if (pts.entity_B != nullptr)
        {
            auto itrBits = rulesBitsPair.find(sortPair(typeA, typeB));
            if (itrBits != rulesBitsPair.end())
            {
                rules[itrBits->second](pts);
            }
            else
            {
                standardFunc(pts);
            }
        }
    }

private:
    struct customHash
    {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &p) const
        {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);

            return h1 ^ h2;
        }
    };

    std::pair<RulesIDType, RulesIDType> sortPair(const RulesIDType &bitA, const RulesIDType &bitB)
    {

        if (bitA < bitB)
            return std::pair<RulesIDType, RulesIDType>(bitA, bitB);
        else
            return std::pair<RulesIDType, RulesIDType>(bitB, bitA);
    }

    std::unordered_map<std::pair<RulesIDType, RulesIDType>, RuleFunction, customHash> rulesFunctions{};

    // Almacenamos que pares de bits hay en cada combinacion y su clave completa
    std::unordered_map<std::pair<RulesIDType, RulesIDType>, RuleFunction, customHash> rules{};
    std::unordered_map<std::pair<RulesIDType, RulesIDType>, std::pair<RulesIDType, RulesIDType>, customHash> rulesBitsPair{};

    RulesIDType number_bits{sizeof(RulesIDType) * 8};

    RuleFunction standardFunc{nullptr};
};