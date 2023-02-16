#pragma once

#include <bitset>
#include <initializer_list>

/**
 * @brief Collection of bitset helper functions to generate, set and reset bitsets 
 * using a number of indcies to set/reset, supporting both integral types and enum class types.
 * A generator function is also provided to support generating bitsets over 64bits succintly without 
 * strings.
 * 
 */
namespace RicCoreUtil{
namespace BitsetHelpers{

    /**
     * @brief Base implementation of the setBits function. 
     * 
     * @author Kiran de Silva
     * 
     * @tparam SIZE size of bitset
     * @tparam T1 Intializer list type
     * @tparam T2 Underlying type of T1, this is either equal to T1 or the integer type of T1
     * @param bitset Reference to bitset we want to modify
     * @param bits initializer list of bit indicies to modify
     */
    template<size_t SIZE,class T1,class T2>
    void setBits_impl(std::bitset<SIZE> &bitset,const std::initializer_list<T1> bits)
    {
        for (auto& idx : bits){
            bitset.set(static_cast<T2>(idx));         
        }
    }

    /**
     * @brief Integer specialization of setBits
     * 
     * @author Kiran de Silva
     * 
     * @tparam SIZE size of bitset
     * @tparam T type of element in initalizer list
     * @tparam std::enable_if<std::is_integral_v<T>, int>::type 
     * @param bitset 
     * @param bits 
     */
    template<size_t SIZE,class T,typename std::enable_if<std::is_integral_v<T>, int>::type = 0>
    void setBits(std::bitset<SIZE> &bitset,const std::initializer_list<T> bits)
    {
        setBits_impl<SIZE,T,T>(bitset,bits);
    }

    /**
     * @brief Enum specialization of setBits
     * 
     * @author Kiran de Silva
     * 
     * @tparam SIZE size of bitset
     * @tparam T type of element in initalizer list
     * @tparam std::enable_if<std::is_integral_v<T>, int>::type 
     * @param bitset 
     * @param bits 
     */
    template<size_t SIZE,class T,typename std::enable_if<std::is_enum_v<T>, int>::type = 0>
    void setBits(std::bitset<SIZE> &bitset,const std::initializer_list<T> bits)
    {
        using T_underlying = typename std::underlying_type_t<T>;
        setBits_impl<SIZE,T,T_underlying>(bitset,bits);
    }
    
    /**
     * @brief Base implementation of the resetBits function. 
     * 
     * @author Kiran de Silva
     * 
     * @tparam SIZE size of bitset
     * @tparam T1 Intializer list type
     * @tparam T2 Underlying type of T1, this is either equal to T1 or the integer type of T1
     * @param bitset Reference to bitset we want to modify
     * @param bits initializer list of bit indicies to modify
     */
    template<size_t SIZE,class T1,class T2>
    void resetBits_impl(std::bitset<SIZE> &bitset,const std::initializer_list<T1> bits)
    {
        for (auto& idx : bits){
            bitset.reset(static_cast<T2>(idx));         
        }
    }

    /**
     * @brief Integer specialization of setBits
     * 
     * @author Kiran de Silva
     * 
     * @tparam SIZE size of bitset
     * @tparam T type of element in initalizer list
     * @tparam std::enable_if<std::is_integral_v<T>, int>::type 
     * @param bitset 
     * @param bits 
     */
    template<size_t SIZE,class T,typename std::enable_if<std::is_integral_v<T>, int>::type = 0>
    void resetBits(std::bitset<SIZE> &bitset,const std::initializer_list<T> bits)
    {
        resetBits_impl<SIZE,T,T>(bitset,bits);
    }

    /**
     * @brief Integer specialization of resetBits
     * 
     * @author Kiran de Silva
     * 
     * @tparam SIZE size of bitset
     * @tparam T type of element in initalizer list
     * @tparam std::enable_if<std::is_integral_v<T>, int>::type 
     * @param bitset 
     * @param bits 
     */
    template<size_t SIZE,class T,typename std::enable_if<std::is_enum_v<T>, int>::type = 0>
    void resetBits(std::bitset<SIZE> &bitset,const std::initializer_list<T> bits)
    {
        using T_underlying = typename std::underlying_type_t<T>;
        resetBits_impl<SIZE,T,T_underlying>(bitset,bits);
    }
    
    /**
     * @brief Bitset generator function to easily generate bitsets of N size.
     * 
     * @tparam SIZE size of btiset
     * @tparam T 
     * @param bits 
     * @return std::bitset<SIZE> 
     */
    template<size_t SIZE,class T>
    std::bitset<SIZE> generateBitset(const std::initializer_list<T> bits)
    {
        std::bitset<SIZE> ret;
        setBits(ret,bits);
        return ret;
    }


};
};