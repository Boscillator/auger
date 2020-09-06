//
// Created by oscil on 09/03/2020.
//

#ifndef AUGER_PLUGIN_CIRCULARBUFFER_H
#define AUGER_PLUGIN_CIRCULARBUFFER_H

#include <array>
#include <span>

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
    for(size_t i = 0; i < block.size(); i++) {
        block[i] = _buffer[ (_readPtr + i) & _wrap];
    }
    _readPtr = (_readPtr + block.size()) & _wrap;
}

template<typename T, size_t BufferSize>
void CircularBuffer<T, BufferSize>::write(std::span<T> block) {
    for(size_t i = 0; i < block.size(); i++) {
        _buffer[(_writePtr + i) & _wrap] = block[i];
    }
    _writePtr = (_writePtr + block.size()) & _wrap;
}

template<typename T, size_t BufferSize>
int CircularBuffer<T, BufferSize>::size() {
    return (_writePtr - _readPtr) % BufferSize;
}


#endif //AUGER_PLUGIN_CIRCULARBUFFER_H