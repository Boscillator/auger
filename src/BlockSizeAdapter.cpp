//
// Created by oscil on 09/04/2020.
//

#include <vector>
#include <iostream>
#include "BlockSizeAdapter.h"

BlockSizeAdapter::BlockSizeAdapter(BlockSizeAdapter::AdapterProcessor* processor, size_t chunkSize) : _processor(
        processor), _chunkSize(chunkSize) {}

size_t BlockSizeAdapter::getChunkSize() const {
    return _chunkSize;
}

void BlockSizeAdapter::setChunkSize(size_t chunkSize) {
    _chunkSize = chunkSize;
}

void BlockSizeAdapter::process(std::span<float> block) {
    _preBuffer.write(block);
    if(_preBuffer.size() >= _chunkSize) {
        std::vector<float> chunk;
        chunk.resize(_chunkSize);
        _preBuffer.read(chunk);
        _processor->processChunk(chunk);
        _postBuffer.write(chunk);
        _processed = true;
    }
    if(_processed) {
        _postBuffer.read(block);
    }
}

void BlockSizeAdapter::AdapterProcessor::processChunk(float* first, size_t n) {
    this->processChunk(std::span<float>(first, n));
}
