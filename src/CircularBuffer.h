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
    void read(std::span<T> block);
    void write(std::span<T> block);
    int size();

private:
    std::array<T, BufferSize> _buffer;
    size_t _readPtr, _writePtr;
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