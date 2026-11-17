// Copyright Advanced Micro Devices, Inc., or its affiliates.
// SPDX-License-Identifier: MIT

// Warm-up probe run by the benchmark supervisor after a client crashed. After a GPU memory fault the
// first process doing real GPU work can compute wrong results; this process does that work (allocate,
// run a kernel, read back and verify) so the next client starts on a healthy device.

#include <hip/hip_runtime.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define HIP_CHECK(expr)                                                                          \
    do                                                                                           \
    {                                                                                            \
        hipError_t hipCheckError = (expr);                                                       \
        if(hipCheckError != hipSuccess)                                                          \
        {                                                                                        \
            std::ostringstream hipCheckMsg;                                                      \
            hipCheckMsg << "hip error " << static_cast<int>(hipCheckError) << " ("               \
                        << hipGetErrorName(hipCheckError) << ") at " << __FILE__ << ":"          \
                        << __LINE__ << ": " << #expr << ": " << hipGetErrorString(hipCheckError); \
            throw std::runtime_error(hipCheckMsg.str());                                         \
        }                                                                                        \
    } while(0)

static constexpr int64_t bufferBytes   = 128LL * 1024 * 1024;
static constexpr int64_t elementCount  = bufferBytes / static_cast<int64_t>(sizeof(uint32_t));
static constexpr int32_t blockSize     = 256;
static constexpr int32_t usageExitCode = 2;

struct HipFree
{
    void operator()(void* pointer) const
    {
        // Nothing useful can be done if freeing fails while unwinding.
        (void)hipFree(pointer);
    }
};

using DeviceBuffer = std::unique_ptr<uint32_t, HipFree>;

static __global__ void copyKernel(const uint32_t* src, uint32_t* dst, int64_t count)
{
    int64_t index = static_cast<int64_t>(blockIdx.x) * blockDim.x + threadIdx.x;
    if(index < count)
        dst[index] = src[index];
}

static uint32_t pattern(int64_t index)
{
    return static_cast<uint32_t>(index) * 2654435761u + 12345u;
}

static bool parseArgs(int argc, char** argv, int32_t& deviceIdx)
{
    deviceIdx = 0;
    for(int32_t i = 1; i < argc; ++i)
    {
        if(std::strcmp(argv[i], "--device-idx") != 0 || i + 1 >= argc)
            return false;
        char*   end   = nullptr;
        int64_t value = std::strtoll(argv[++i], &end, 10);
        if(end == argv[i] || *end != '\0' || value < 0 || value > INT32_MAX)
            return false;
        deviceIdx = static_cast<int32_t>(value);
    }
    return true;
}

static DeviceBuffer allocateBuffer()
{
    void* pointer = nullptr;
    HIP_CHECK(hipMalloc(&pointer, bufferBytes));
    return DeviceBuffer(static_cast<uint32_t*>(pointer));
}

static int64_t countMismatches(const std::vector<uint32_t>& result, int64_t& firstIndex)
{
    int64_t mismatches = 0;
    firstIndex         = -1;
    for(int64_t i = 0; i < elementCount; ++i)
    {
        if(result[i] == pattern(i))
            continue;
        if(mismatches == 0)
            firstIndex = i;
        ++mismatches;
    }
    return mismatches;
}

static void copyOnDevice(const std::vector<uint32_t>& input, std::vector<uint32_t>& output)
{
    DeviceBuffer src = allocateBuffer();
    DeviceBuffer dst = allocateBuffer();
    HIP_CHECK(hipMemcpy(src.get(), input.data(), bufferBytes, hipMemcpyHostToDevice));
    HIP_CHECK(hipMemset(dst.get(), 0xFF, bufferBytes));

    int64_t blocks = (elementCount + blockSize - 1) / blockSize;
    hipLaunchKernelGGL(copyKernel,
                       dim3(static_cast<uint32_t>(blocks)),
                       dim3(blockSize),
                       0,
                       0,
                       static_cast<const uint32_t*>(src.get()),
                       dst.get(),
                       elementCount);
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipDeviceSynchronize());
    HIP_CHECK(hipMemcpy(output.data(), dst.get(), bufferBytes, hipMemcpyDeviceToHost));
}

// Returns an empty string when the copy was verified, otherwise the mismatch description.
static std::string runProbe(int32_t deviceIdx)
{
    HIP_CHECK(hipSetDevice(deviceIdx));
    std::vector<uint32_t> input(elementCount);
    for(int64_t i = 0; i < elementCount; ++i)
        input[i] = pattern(i);
    std::vector<uint32_t> output(elementCount);
    copyOnDevice(input, output);

    int64_t firstIndex = -1;
    int64_t mismatches = countMismatches(output, firstIndex);
    if(mismatches == 0)
        return "";
    std::ostringstream message;
    message << mismatches << " of " << elementCount << " elements differ (first at index "
            << firstIndex << ")";
    return message.str();
}

int main(int argc, char** argv)
{
    int32_t deviceIdx = 0;
    if(!parseArgs(argc, argv, deviceIdx))
    {
        std::cerr << "usage: " << argv[0] << " [--device-idx N]" << std::endl;
        return usageExitCode;
    }
    try
    {
        std::string mismatch = runProbe(deviceIdx);
        if(!mismatch.empty())
        {
            std::cerr << "gpu probe failed on device " << deviceIdx << ": " << mismatch
                      << std::endl;
            return 1;
        }
    }
    catch(const std::exception& error)
    {
        std::cerr << "gpu probe failed on device " << deviceIdx << ": " << error.what()
                  << std::endl;
        return 1;
    }
    std::cout << "gpu probe passed on device " << deviceIdx << " (128 MiB copy verified)"
              << std::endl;
    return 0;
}
