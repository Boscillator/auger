//
// Created by oscil on 09/03/2020.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/CircularBuffer.h"

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
    }
}