#pragma once

#include <vector>
#include <unistd.h>

#include "wrappedfile.h"

/**
 * @brief Append request class to write in a non-blocking way
 * 
 */
struct AppendRequest
{
    /**
     * @brief Container for data to write 
     * 
     */
    std::vector<uint8_t> data; 
    /**
     * @brief Pointer to the wrapped file object to write to
     * 
     */
    WrappedFile* file;
    /**
     * @brief Explicit const reference constructor with copy
     * 
     * @param data 
     * @param file 
     */
    AppendRequest(const std::vector<uint8_t>& data,WrappedFile* file):
    data(data),
    file(file)
    {};

    /**
     * @brief Explict non-const reference constructor implementing move semantics
     * 
     * @param data 
     * @param file 
     */
    AppendRequest(std::vector<uint8_t>& data,WrappedFile* file):
    data(std::move(data)),
    file(file)
    {};
    

};