//
// Created by oscil on 09/05/2020.
//

#ifndef AUGER_PLUGIN_UTILITIES_H
#define AUGER_PLUGIN_UTILITIES_H

#include <vector>

#ifdef AUGER_DEBUG
#include <iostream>
#include <chrono>
#define DBG_LOG(level) std::cerr << "[" << level "] " << __FILE__ << ":" << __LINE__ << " "
#define DBG_ERR() DBG_LOG("ERROR")
#define DBG_INFO() DBG_LOG("INFO")
#define DBG_PROF() DBG_LOG("PROF")
#endif


/**
 * Convert from two separate stereo buffers to a single interlaced buffer.
 * e.g.
 *  Left: {1,1,1,1}
 *  Right: {2,2,2,2}
 *  Dst: {1,2,1,2,1,2,1,2}
 * @param n Number of samples in the left or right buffer. In the example this should be 4
 * @param left First component to interlace. Must be `n*sizeof(float)` bytes long.
 * @param right Second component to interlace. Must be `n*sizeof(float)` bytes long.
 * @param dst Location to write the results to. Must be`2*n*sizeof(float)` bytes long.
 */
void interlace(size_t n, const float* left, const float* right, float* dst);

/**
 * Convert from an interlaced signal to two stereo buffers.
 * e.g.
 *  Dst: {1,2,1,2,1,2,1,2}
 *  Left: {1,1,1,1}
 *  Right: {2,2,2,2}
 * @param n Number of samples in the left or right buffer.
 * @param left Write even elements here. Must be `n*sizeof(float)` bytes long.
 * @param right Write odd elements here. Must be `n*sizeof(float)` bytes long.
 * @param src Location of the interlaced buffer. Must be `2*n*sizeof(float)` bytes long.
 */
void deinterlace(size_t n, float* left, float* right, const float* src);



#endif //AUGER_PLUGIN_UTILITIES_H
