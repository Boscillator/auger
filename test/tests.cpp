//
// Created by oscil on 09/03/2020.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/CircularBuffer.h"
#include "../src/BlockSizeAdapter.h"
#include "../src/utilities.h"

TEST_CASE("Sanity Check", "[sanity]") {
    REQUIRE(1 == 1);
}


SCENARIO("A circular buffer can have be written to and read from") {
    GIVEN("A blank circular buffer") {
        CircularBuffer<float, 64> buffer;
        WHEN("you write to it") {
            float block[] = {1.0, 2.0, 3.0, 4.0};
            buffer.write(block);
            THEN("it's size increases") {
                REQUIRE(buffer.size() == 4);
            }
        }

        WHEN("you perform a write along a boundary") {
            float block1[60];
            std::fill(block1, block1 + 60, 1.0f);
            float block2[8];
            std::fill(block2, block2 + 8, 2.0f);

            buffer.write(block1);
            buffer.write(block2);
            THEN("it has the correct size") {
                REQUIRE(buffer.size() == 4);
            }
            THEN("the correct data is read") {
                float result[4] = {-1.0};
                buffer.read(result);
                REQUIRE(result[0] == 2.0f);
                REQUIRE(result[1] == 2.0f);
                REQUIRE(result[2] == 2.0f);
                REQUIRE(result[3] == 2.0f);

                float result2[60];
                buffer.read(result2);
                REQUIRE(result2[59 - 4] == 1.0f);
                REQUIRE(result2[59] == 2.0f);
            }
        }
    }

    GIVEN("A circular buffer with some data") {
        CircularBuffer<float, 32> buffer;
        float block[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
        buffer.write(block);

        WHEN("you read from it") {
            float output[4];
            buffer.read(output);
            THEN("The output is correct") {
                REQUIRE(output[0] == 1.0);
                REQUIRE(output[1] == 2.0);
                REQUIRE(output[2] == 3.0);
                REQUIRE(output[3] == 4.0);
            }
            THEN("The size is updated") {
                REQUIRE(buffer.size() == 4);
            }
        }

        WHEN("you read enough data to loop around") {
            float temp[4];
            for(int i = 0; i < 32/4; i++) {
                buffer.read(temp);
            }
            THEN("It loops around") {
                float output[4];
                buffer.read(output);
                REQUIRE(output[0] == 1.0);
                REQUIRE(output[1] == 2.0);
                REQUIRE(output[2] == 3.0);
                REQUIRE(output[3] == 4.0);
            }
        }

        WHEN("you read across a boundary") {
            float temp[4];
            for(int i = 0; i < 31/4; i++) {
                buffer.read(temp);
            }

            THEN("The read contains data on both sizes") {
                float output[8];
                buffer.read(output);
                REQUIRE(output[0] == 0.0);
                REQUIRE(output[1] == 0.0);
                REQUIRE(output[2] == 0.0);
                REQUIRE(output[3] == 0.0);
                REQUIRE(output[4] == 1.0);
                REQUIRE(output[5] == 2.0);
                REQUIRE(output[6] == 3.0);
                REQUIRE(output[7] == 4.0);
            }
        }

    }
}

class TestBlockProcessor : public BlockSizeAdapter::AdapterProcessor {
public:
    int chunksProcessed = 0;
    void processChunk(std::span<float> chunk) override {
        std::fill(chunk.begin(), chunk.end(), 1.0f);
        chunksProcessed++;
    }

};

SCENARIO("BlockSizeAdapter can process data as required") {
    GIVEN("A BlockSizeAdapter with a TestBlockProcessor and size 16 chunks") {
        TestBlockProcessor p;
        BlockSizeAdapter adapter(&p, 16);

        WHEN("A group of chunks is written") {
            float chunk[] = {0.0, 0.0, 0.0, 0.0};
            for(int i = 0; i < 4; i++) {
                adapter.process(chunk);
            }
//            THEN("processing occurred") {
//                REQUIRE(p.chunksProcessed == 1);
//            }
            THEN("the result can be read") {
                // Read the next available chunk into `chunk`
                adapter.process(chunk);
                REQUIRE(chunk[0] == 1.0f);
            }
        }
    }
}

TEST_CASE("interlace()", "[utilities]") {
    SECTION("interlace samples correctly") {
        float left[] = {1.0f, 1.0f, 1.0f, 1.0f};
        float right[] = {2.0f, 2.0f, 2.0f, 2.0f};
        float result[8];
        interlace(4, left, right, result);
        for(int i = 0; i < 8; i += 2) {
            REQUIRE(result[i] == 1.0f);
            REQUIRE(result[i+1] == 2.0f);
        }
    }
    SECTION("deinterlace() works correctly") {
        float left[4];
        float right[4];
        float data[] = {1.0, 2.0, 1.0, 2.0, 1.0, 2.0, 1.0, 2.0};
        deinterlace(4, left, right, data);
        for(int i = 0; i < 4; i++) {
            REQUIRE(left[i] == 1.0f);
            REQUIRE(right[i] == 2.0f);
        }
    }
    SECTION("interlace() followed by deinterlace() returns original result") {

        float left[] = {1.0f, 1.0f, 1.0f, 1.0f};
        float right[] = {2.0f, 2.0f, 2.0f, 2.0f};
        float result[8];

        // Interlace
        interlace(4, left, right, result);

        // Clear buffers so they can be re-used
        std::fill(left, left+4, 0.0f);
        std::fill(right, right+4, 0.0f);

        // Deinterlace
        deinterlace(4, left, right, result);

        // assert
        for(int i = 0; i < 4; i++) {
            REQUIRE(left[i] == 1.0f);
            REQUIRE(right[i] == 2.0f);
        }
    }
}

