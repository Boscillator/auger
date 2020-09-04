//
// Created by oscil on 09/04/2020.
//

#ifndef AUGER_PLUGIN_BLOCKSIZEADAPTER_H
#define AUGER_PLUGIN_BLOCKSIZEADAPTER_H


#include "CircularBuffer.h"


class BlockSizeAdapter {
public:
    class AdapterProcessor {
    public:
        void processChunk(float* first, size_t n);
        virtual void processChunk(std::span<float> chunk) = 0;
    };

    BlockSizeAdapter(AdapterProcessor* processor, size_t chunkSize);

    size_t getChunkSize() const;

    void setChunkSize(size_t chunkSize);

    void process(std::span<float> block);

private:
    AdapterProcessor* _processor;
    size_t _chunkSize;
    CircularBuffer<float, 2048> _preBuffer;
    CircularBuffer<float, 2048> _postBuffer;
    bool _processed = false;
};


#endif //AUGER_PLUGIN_BLOCKSIZEADAPTER_H
