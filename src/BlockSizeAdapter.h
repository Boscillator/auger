//
// Created by oscil on 09/04/2020.
//

#ifndef AUGER_PLUGIN_BLOCKSIZEADAPTER_H
#define AUGER_PLUGIN_BLOCKSIZEADAPTER_H


#include "CircularBuffer.h"

/**
 * A utility class to convert blocks of sample of one size to another.
 * Receives samples of <input size> until enough samples are built up in it's input buffer to reach <chunk size>.
 * Then, it calls `processChunk()` on the listener, and reads the result back over multiple calls to `process()`
 */
class BlockSizeAdapter {
public:
    /**
     * Interface with a callback to process audio
     */
    class AdapterProcessor {
    public:
        void processChunk(float* first, size_t n);

        /**
         * Perform some audio process by mutating `chunk`
         * @param chunk A span pointing to the data to be processed
         */
        virtual void processChunk(std::span<float> chunk) = 0;
    };

    /**
     * Constructor
     * @param processor A pointer to an instance of a class with a callback to process data
     * @param chunkSize The size the chunk passed to processor will be in samples
     */
    BlockSizeAdapter(AdapterProcessor* processor, size_t chunkSize);

    /**
     * @return The size the chunk passed to processor will be in samples
     */
    size_t getChunkSize() const;

    /**
     * @param chunkSize The size the chunk passed to processor will be in samples
     */
    void setChunkSize(size_t chunkSize);

    /**
     * Load samples into the block size adapter. `block` will be mutated to contain the most resent results
     * @param block
     */
    void process(std::span<float> block);

private:
    AdapterProcessor* _processor;
    size_t _chunkSize;
    CircularBuffer<float, 2*8192> _preBuffer;
    CircularBuffer<float, 2*8192> _postBuffer;
    std::vector<float> _chunkBuffer;
    bool _processed = false;
};


#endif //AUGER_PLUGIN_BLOCKSIZEADAPTER_H
