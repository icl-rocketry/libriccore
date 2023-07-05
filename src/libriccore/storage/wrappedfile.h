#pragma once
/**
 * @file wrappedfile.h
 * @author Akshat + Kiran
 * @brief A wrapped file provides an interface to a 'file' on a given file system or store. This allows us to wrap any underlying file objects
 *  like arduino files or sdfile etc allowing writes to the each file to be handled asynchornously from the main thread. This is particularly important
 * when writing to devices such as sd cards on the spi bus, as the writes block, resulting in a pause in the main loop if used nativley. 
 * @version 0.1
 * @date 2023-07-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "storetypes.h"

class StoreBase;


class WrappedFile
{
public:
    WrappedFile(StoreBase &store,FILE_MODE mode = FILE_MODE::RW);

    // Send a request to the underlying store to store this
    void append(const std::vector<char> &data, bool *done);

    void read(std::vector<char> &dest);
    void close(); // Should this be done automatically on delete?

protected:
    FILE_MODE mode;
    StoreBase &store;

    const store_fd file_desc;

private:
    virtual void _read(std::vector<char> &dest) = 0;
    virtual void _close() = 0;

    // Write/Flush to the underlying file type
    virtual void file_write(const std::vector<char> &data) = 0;
    virtual void file_flush() = 0;

    friend StoreBase;
};