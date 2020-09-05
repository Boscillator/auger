//
// Created by oscil on 09/05/2020.
//

#ifndef AUGER_PLUGIN_UTILITIES_H
#define AUGER_PLUGIN_UTILITIES_H

#include <vector>

void interlace(size_t n, const float* left, const float* right, float* dst);
void deinterlace(size_t n, float* left, float* right, const float* src);


#endif //AUGER_PLUGIN_UTILITIES_H
