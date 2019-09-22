#ifndef ALGINE_CORE_UTILS_H
#define ALGINE_CORE_UTILS_H

#include <algine/types.h>
#include <vector>
#include <string>

#define delete_ptr(ptr) if (ptr != nullptr) { delete ptr; ptr = nullptr; }
#define delete_ptr_arr(ptr) if (ptr != nullptr) { delete[] ptr; ptr = nullptr; }
#define getArraySize(array) (sizeof(array) / sizeof(array[0]))

namespace algine {
// returns GPU Vendor name
// for example `NVIDIA Corporation`
const uchar* getGPUVendor();

// returns GPU Renderer name
// for example `GeForce 920MX/PCIe/SSE2`
const uchar* getGPURenderer();

// splits `in` by `delimiter`
std::vector<std::string> split(std::string &in, const std::string &delimiter);

// splits `in` by `delimiter`
std::vector<std::string> split(const std::string &in, const std::string &delimiter);

// replaces first found occurrence
std::string replace(const std::string &src, const std::string &target, const std::string &replacement);

// returns current time in milliseconds
long getTime();

}

#endif /* ALGINE_CORE_UTILS_H */
