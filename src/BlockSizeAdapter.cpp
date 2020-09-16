//
// Created by oscil on 09/04/2020.
//

#include <vector>
#ifdef AUGER_DEBUG
#include <chrono>
#endif
#include "utilities.h"
#include "BlockSizeAdapter.h"

BlockSizeAdapter::BlockSizeAdapter(BlockSizeAdapter::AdapterProcessor* processor, size_t chunkSize) : _processor(
        processor) {
    setChunkSize(chunkSize);
}

size_t BlockSizeAdapter::getChunkSize() const {
    return _chunkSize;
}

void BlockSizeAdapter::setChunkSize(size_t chunkSize) {
    _chunkBuffer.resize(chunkSize);
    _postBuffer.forward(chunkSize);
    _chunkSize = chunkSize;
}

void BlockSizeAdapter::process(std::span<float> block) {
    _preBuffer.write(block);
    while(_preBuffer.size() > _chunkSize) {
        _preBuffer.read(_chunkBuffer);

        _processor->processChunk(_chunkBuffer);

        _postBuffer.write(_chunkBuffer);
        _processed = true;
    }
    if(_processed) {
        _postBuffer.read(block);
    }
}

void BlockSizeAdapter::AdapterProcessor::processChunk(float* first, size_t n) {
    this->processChunk(std::span<float>(first, n));
}
