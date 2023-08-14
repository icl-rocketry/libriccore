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


    /**
     * @brief Send a request to the underlying store to store this
     * 
     * @param data data to write to file, this will invalidate the provided vector 
     */
    void append(std::vector<uint8_t> &data);

    /**
     * @brief Sends a request to unerlying store to store this, however preserves the provided std vector by
     * copying this data out. 
     * 
     * @param data const reference to vector
     */
    void appendCopy(const std::vector<uint8_t> &data);

    /**
     * @brief Threadsafe read to destination
     * 
     * @param dest 
     */
    void read(std::vector<uint8_t> &dest);

    /**
     * @brief Automatically called in destructor
     * 
     * @param force force closes file by erasing append queue, potential data loss
     */
    void close(bool force); 

    virtual ~WrappedFile();

protected:
    FILE_MODE mode;
    StoreBase &store;

    const store_fd file_desc;

private:
    virtual void _read(std::vector<uint8_t> &dest) = 0;
    virtual void _close() = 0;

    // Write/Flush to the underlying file type
    virtual void file_write(const std::vector<uint8_t> &data) = 0;
    virtual void file_flush() = 0;

    friend StoreBase;
};