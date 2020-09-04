//
// Created by oscil on 09/03/2020.
//

#ifndef AUGER_PLUGIN_CIRCULARBUFFER_H
#define AUGER_PLUGIN_CIRCULARBUFFER_H

#include <array>
#include <span>
#include <type_traits>

template<typename T, size_t BufferSize, size_t ReadSize, size_t WriteSize>
class CircularBuffer {
    static_assert((BufferSize & (BufferSize - 1)) == 0, "Buffer size must be a power of 2");
    static_assert(BufferSize % ReadSize == 0, "Read size must be a factor of BufferSize");
    static_assert(BufferSize % WriteSize == 0, "Write size must be a factor of BufferSize");

public:
    CircularBuffer();

    /**
     * Get the distance between the read and write pointers. Effectively, how lagged is data reading
     * @return (write_ptr - read_ptr) % BufferSize
     */
    int size() { return (_writePtr - _readPtr) % BufferSize; };

    /**
     * Get an area to write to
     * @return
     */
    std::span<T, WriteSize> write();

    std::span<T, ReadSize> read();

private:
    size_t _writePtr = 0;
    size_t _readPtr = 0;

    std::array<T, BufferSize> _buffer;
};

template<typename T, size_t BufferSize, size_t ReadSize, size_t WriteSize>
CircularBuffer<T, BufferSize, ReadSize, WriteSize>::CircularBuffer() {
    std::fill(_buffer.begin(), _buffer.end(), T());
}

template<typename T, size_t BufferSize, size_t ReadSize, size_t WriteSize>
std::span<T, WriteSize> CircularBuffer<T, BufferSize, ReadSize, WriteSize>::write() {
    auto r = std::span<T, WriteSize>(_buffer.begin() + _writePtr, _buffer.begin() + _writePtr + WriteSize);
    _writePtr += WriteSize;
    _writePtr &= BufferSize - 1;
    return r;
}

template<typename T, size_t BufferSize, size_t ReadSize, size_t WriteSize>
std::span<T, ReadSize> CircularBuffer<T, BufferSize, ReadSize, WriteSize>::read() {
    auto r = std::span<T, ReadSize>(_buffer.begin() + _readPtr, _buffer.begin() + _readPtr + ReadSize);
    _readPtr += ReadSize;
    _readPtr &= BufferSize - 1;
    return r;
}


#endif //AUGER_PLUGIN_CIRCULARBUFFER_H