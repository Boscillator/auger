//
// Created by oscil on 09/03/2020.
//

#ifndef AUGER_PLUGIN_CIRCULARBUFFER_H
#define AUGER_PLUGIN_CIRCULARBUFFER_H

#include <array>
#include <span>

#ifdef AUGER_DEBUG

#include <iostream>

#endif

template<typename T, size_t BufferSize>
class CircularBuffer {

    static_assert((BufferSize & (BufferSize - 1)) == 0, "BufferSize must be a power of 2");
    static const size_t _wrap = (BufferSize - 1);

public:
    CircularBuffer();

    /**
     * Read `block.size()` samples from the buffer into `block`. Mutating block may mutate the contents of the buffer
     * @param block A place in memory to read to
     */
    void read(std::span<T> block);

    /**
     * Read `n` samples form the buffer
     * @param dst The location to write data to. At least `n*sizeof(T)` bytes in size.
     * @param n The number of samples to read
     */
    void read(T* dst, size_t n) { read(std::span<T>(dst, n)); };

    /**
     * Write samples from `block` into the buffer
     * @param block Location to write data from
     */
    void write(std::span<T> block);

    /**
     * Write n samples to the buffer
     * @param src Location to start reading from. Must be at least `n*sizeof(T)` bytes long
     * @param n Number of samples to write
     */
    void write(T* src, size_t n) { write(std::span<T>(src, n)); };

    /**
     * Get the size difference between the read head and write head
     * @return `(writePosition - readPosition) % BufferSize`
     */
    int size();

private:
    std::array<T, BufferSize> _buffer;
    size_t _readPtr = 0;
    size_t _writePtr = 0;
};

template<typename T, size_t BufferSize>
CircularBuffer<T, BufferSize>::CircularBuffer() {
    std::fill(_buffer.begin(), _buffer.end(), T());
}

template<typename T, size_t BufferSize>
void CircularBuffer<T, BufferSize>::read(std::span<T> block) {

#ifdef AUGER_DEBUG
    if (block.size() > size()) {
        std::cerr << "[ERR] " << __FILE__ << ":" << __LINE__ << "Read buffer overflow <this=" << (void*) this
                  << " _readPtr=" << _readPtr << " _writePtr=" << _writePtr << " size()=" << size() << " block_size=" << block.size() << ">" << std::endl;
    }
#endif

    size_t first_read = std::min(BufferSize - _readPtr, block.size());
    size_t second_read = block.size() - first_read;
    memcpy(block.data(), _buffer.data() + _readPtr, first_read * sizeof(T));
    memcpy(block.data() + first_read, _buffer.data(), second_read * sizeof(T));
    _readPtr = (_readPtr + block.size()) & _wrap;
}

template<typename T, size_t BufferSize>
void CircularBuffer<T, BufferSize>::write(std::span<T> block) {
    size_t rpc = _readPtr;
    size_t wpc = _writePtr;
    size_t first_read = std::min(BufferSize - _writePtr, block.size());
    size_t second_read = block.size() - first_read;
    memcpy(_buffer.data() + _writePtr, block.data(), first_read * sizeof(T));
    memcpy(_buffer.data(), block.data() + first_read, second_read * sizeof(T));
    _writePtr = (_writePtr + block.size()) & _wrap;
}

template<typename T, size_t BufferSize>
int CircularBuffer<T, BufferSize>::size() {
    return (_writePtr - _readPtr) % _wrap;
}


#endif //AUGER_PLUGIN_CIRCULARBUFFER_H