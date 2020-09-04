//
// Created by oscil on 09/03/2020.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/CircularBuffer.h"

TEST_CASE("Sanity Check", "[sanity]") {
    REQUIRE(1 == 1);
}


SCENARIO("A circular buffer can have be written and read from in arbitrary sizes") {
    GIVEN("A circular buffer") {
        CircularBuffer<int, 64, 8, 16> buffer;

        WHEN("16 data points are written") {
            int data[] = {1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,0};
            memcpy(buffer.write().data(), data, sizeof(int) * 16);
            THEN("The size should update") {
                REQUIRE(buffer.size() == 16);
            }
        }
    }

    GIVEN("A circular buffer with some data") {
        CircularBuffer<int, 64, 8, 16> buffer;
        int data[] = {1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,0};
        memcpy(buffer.write().data(), data, sizeof(int) * 16);

        WHEN("8 data points are read") {
            std::span<int, 8> result = buffer.read();
            THEN("The values should be correct") {
                REQUIRE(result[0] == 1);
                REQUIRE(result[1] == 2);
                REQUIRE(result[2] == 3);
            }
            THEN("The size is updated") {
                REQUIRE(buffer.size() == 8);
            }
        }

        WHEN("the data is read so as to overrun the end of the buffer") {
            for(int i = 0; i < 64/8; i++) {
                buffer.read();
            }
            THEN("the read pointer resets to the beginning of the buffer") {
                std::span<int, 8> result = buffer.read();
                REQUIRE(result[0] == 1);
            }
        }
    }
}