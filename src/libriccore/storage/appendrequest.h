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
     * @brief Pointer to flag to indicate write complete
     * 
     */
    bool* done;

    AppendRequest(const std::vector<uint8_t>& data,WrappedFile* file,bool* done):
    data(data),
    file(file),
    done(done)
    {};

    AppendRequest(std::vector<uint8_t>& data,WrappedFile* file,bool* done):
    data(std::move(data)),
    file(file),
    done(done)
    {};
    

};