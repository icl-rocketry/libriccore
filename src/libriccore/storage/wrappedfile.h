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
#include <atomic>
#include <exception>

#include "storetypes.h"

class StoreBase;


class WrappedFile
{
public:
    WrappedFile(StoreBase &store,store_fd fileDesc, FILE_MODE mode = FILE_MODE::RW, size_t maxQueueSize = 10);


    /**
     * @brief Send a request to the underlying store to store this. Throws WrappedFileCLosedException if file is already closed
     * 
     * @param data data to write to file, this will invalidate the provided vector 
     */
    void append(std::vector<uint8_t> &data);

    /**
     * @brief Sends a request to unerlying store to store this, however preserves the provided std vector by
     * copying this data out.  Throws WrappedFileCLosedException if file is already closed
     * 
     * @param data const reference to vector
     */
    void appendCopy(const std::vector<uint8_t> &data);

    /**
     * @brief Threadsafe read to destination.  Throws WrappedFileCLosedException if file is already closed
     * 
     * @param dest 
     */
    void read(std::vector<uint8_t> &dest);

    /**
     * @brief Close current file, file must be re-opened from store base
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
    
    /**
     * @brief Read underlying file, throws WrappedFileReadException on error
     * 
     * @param dest 
     */
    virtual void _read(std::vector<uint8_t> &dest) = 0;

    /**
     * @brief Underlying Close file, throws WrappedFileCloseException on error
     * 
     */
    virtual void _close(){_closed = true;};

    /**
     * @brief Write to underlying file, throws WrappedFileWriteException on any error
     * 
     * @param data 
     */
    virtual void file_write(const std::vector<uint8_t> &data) = 0;
    
    /**
     * @brief Force flush, throws WrappedFileFlushException on any error
     * 
     */
    virtual void file_flush() = 0;

    friend StoreBase;

    std::atomic<bool> _closed;

    public:

        class WriteException : public std::exception{};

        class FlushException : public std::exception{};

        class ReadException : public std::exception{};

        class CloseException : public std::exception{};

};

