// Copyright Advanced Micro Devices, Inc., or its affiliates.
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

#include "BenchmarkDatabaseSetup.hpp"

#include <Tensile/ContractionLibrary.hpp>
#include <Tensile/ContractionProblem.hpp>
#include <Tensile/FusedA2A.hpp>

#ifdef TENSILELITE_HAVE_MSGPACK_WRITER
#include <Tensile/msgpack/Loading.hpp>
#include <msgpack.hpp>
#endif
#ifdef TENSILE_YAML
#include <Tensile/llvm/Writing.hpp>
#include <Tensile/llvm/YAML.hpp>
#endif

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace TensileLite;
using namespace TensileLite::Client;

namespace
{
    std::shared_ptr<ContractionSolution> makeTestSolution()
    {
        auto solution          = std::make_shared<ContractionSolution>();
        solution->index        = 12;
        solution->solutionName = "Solution_12";
        solution->kernelName   = "Kernel_12";

        auto& mapping             = solution->sizeMapping;
        mapping.waveNum           = 4;
        mapping.workGroupSize     = {256, 1, 1};
        mapping.threadTile        = {1, 1, 1};
        mapping.macroTile         = {128, 128, 1};
        mapping.matrixInstruction = {16, 16, 16, 1};
        mapping.waveGroup         = {2, 2};
        mapping.depthU            = 64;
        mapping.globalSplitU      = 1;
        mapping.workGroupMapping  = 8;

        solution->problemType.operationIdentifier = "Contraction_l_Ajlk_Blik_Cijk_Dijk";
        solution->problemType.aType               = rocisa::DataType::Half;
        solution->problemType.bType               = rocisa::DataType::Half;
        solution->problemType.cType               = rocisa::DataType::Half;
        solution->problemType.dType               = rocisa::DataType::Half;
        return solution;
    }
}

TEST(BenchmarkDatabaseSetup, SolutionSpecUsesMsgpackWhenTheHostCanWriteIt)
{
    auto solution = makeTestSolution();

    SolutionSpec spec = serializeSolution(*solution);
#ifdef TENSILELITE_HAVE_MSGPACK_WRITER
    EXPECT_EQ(spec.format, "msgpack");
#else
    EXPECT_EQ(spec.format, "yaml");
#endif
    EXPECT_FALSE(spec.bytes.empty());
    EXPECT_EQ(spec.bytes, serializeSolution(*solution).bytes);
}

#ifdef TENSILELITE_HAVE_MSGPACK_WRITER
TEST(BenchmarkDatabaseSetup, MsgpackSolutionSpecLoadsBackToTheSameSolution)
{
    auto solution = makeTestSolution();

    SolutionSpec spec = serializeSolution(*solution);

    // The spec is an element of a library's solutions array.
    msgpack::sbuffer                  doc;
    msgpack::packer<msgpack::sbuffer> packer(doc);
    packer.pack_map(2);
    packer.pack(std::string("solutions"));
    packer.pack_array(1);
    doc.write(spec.bytes.data(), spec.bytes.size());
    packer.pack(std::string("library"));
    packer.pack_map(2);
    packer.pack(std::string("type"));
    packer.pack(std::string("Single"));
    packer.pack(std::string("index"));
    packer.pack(solution->index);

    auto data = reinterpret_cast<const uint8_t*>(doc.data());
    auto loaded = std::dynamic_pointer_cast<MasterSolutionLibrary<ContractionProblemGemm>>(
        MessagePackLoadLibraryData<ContractionProblemGemm, ContractionSolution>(
            std::vector<uint8_t>(data, data + doc.size())));
    ASSERT_NE(loaded, nullptr);
    ASSERT_EQ(loaded->solutions.size(), 1u);

    auto const& back = *loaded->solutions.at(solution->index);
    EXPECT_EQ(back.index, solution->index);
    EXPECT_EQ(back.name(), solution->name());
    EXPECT_EQ(serializeSolution(back).bytes, spec.bytes);
}
#endif

#ifdef TENSILE_YAML
TEST(BenchmarkDatabaseSetup, YamlSolutionSpecLoadsBackToTheSameSolution)
{
    auto solution = makeTestSolution();

    std::string text = LLVMWriteSolutionData(*solution);
    ASSERT_FALSE(text.empty());

    llvm::yaml::Input                    yin(text);
    std::shared_ptr<ContractionSolution> back;
    yin >> back;
    ASSERT_FALSE(yin.error()) << yin.error().message();
    ASSERT_NE(back, nullptr);
    EXPECT_EQ(back->index, solution->index);
    EXPECT_EQ(back->name(), solution->name());
    EXPECT_EQ(LLVMWriteSolutionData(*back), text);
}
#endif

namespace
{
    // The options checkSupportedOptions reads, at values the client accepts.
    po::variables_map supportedArgs()
    {
        po::variables_map vm;
        vm["num-benchmarks"].value()       = 1;
        vm["fused-a2a"].value()            = FusedA2A::Disabled;
        vm["selection-only"].value()       = false;
        vm["icache-flush-args"].value()    = std::vector<bool>(1, false);
        vm["prediction-threshold"].value() = 2.0;
        return vm;
    }
}

TEST(BenchmarkDatabaseSetup, DefaultOptionsAreSupported)
{
    EXPECT_NO_THROW(checkSupportedOptions(supportedArgs()));
}

TEST(BenchmarkDatabaseSetup, PredictionThresholdOfOneIsRejected)
{
    po::variables_map vm = supportedArgs();
    vm["prediction-threshold"].value() = 1.0;
    EXPECT_THROW(checkSupportedOptions(vm), std::runtime_error);
}

TEST(BenchmarkDatabaseSetup, PredictionThresholdBelowOneIsRejected)
{
    po::variables_map vm = supportedArgs();
    vm["prediction-threshold"].value() = 0.5;
    EXPECT_THROW(checkSupportedOptions(vm), std::runtime_error);
}

TEST(BenchmarkDatabaseSetup, PredictionThresholdAboveOneIsSupported)
{
    po::variables_map vm = supportedArgs();
    vm["prediction-threshold"].value() = 1.5;
    EXPECT_NO_THROW(checkSupportedOptions(vm));
}

TEST(BenchmarkDatabaseSetup, PredictionThresholdMessageNamesTheOption)
{
    po::variables_map vm = supportedArgs();
    vm["prediction-threshold"].value() = 1.0;
    try
    {
        checkSupportedOptions(vm);
        FAIL() << "expected an exception";
    }
    catch(std::runtime_error const& error)
    {
        EXPECT_NE(std::string(error.what()).find("prediction-threshold"), std::string::npos);
    }
}
