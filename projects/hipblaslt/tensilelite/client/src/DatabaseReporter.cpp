// Copyright Advanced Micro Devices, Inc., or its affiliates.
// SPDX-License-Identifier: MIT

#include "DatabaseReporter.hpp"

#include <sstream>
#include <stdexcept>

using namespace TensileLite;
using namespace TensileLite::Client;

static bool isPassingValidation(std::string const& validation)
{
    return validation == "PASSED" || validation == "NO_CHECK";
}

static KeyValues toKeyValues(std::map<std::string, std::string> const& values)
{
    return KeyValues(values.begin(), values.end());
}

template <typename T>
static std::string formatValue(T const& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

static std::string sizeKey(int32_t dimension)
{
    return "Size" + std::string(1, static_cast<char>('I' + dimension));
}

static std::string formatSizes(std::vector<size_t> const& sizes)
{
    std::string text = "(";
    bool        first = true;
    for(size_t size : sizes)
    {
        if(!first)
            text += ",";
        text += formatValue(size);
        first = false;
    }
    return text + ")";
}

DatabaseReporter::DatabaseReporter(std::shared_ptr<BenchmarkDatabase> database)
    : m_database(std::move(database))
{
}

void DatabaseReporter::beginRun(int64_t problemIndex, int64_t solutionIndex)
{
    // Stored at the first launch so that a crash on the last pair still leaves the values.
    if(!m_problemValuesStored)
    {
        m_database->setProblemValues(problemIndex, toKeyValues(m_problemValues));
        m_problemValuesStored = true;
    }
    m_database->beginRun(problemIndex, solutionIndex);
}

void DatabaseReporter::endRun(int64_t problemIndex, int64_t solutionIndex)
{
    std::string validation;
    RunStatus   status = RunStatus::Pass;
    auto        found  = m_runValues.find(ResultKey::Validation);
    if(found != m_runValues.end())
    {
        validation = found->second;
        // Like the client CSV, only a reported verdict can fail a pair; a pair that reports none
        // (e.g. NumElementsToValidate: 0 without warmups, or a granularity-threshold skip) passes.
        if(!isPassingValidation(validation))
            status = RunStatus::Failure;
    }
    m_database->endRun(
        problemIndex, solutionIndex, status, validation, toKeyValues(m_runValues), m_iterations);
}

void DatabaseReporter::store(std::string const& key, std::string const& value)
{
    // Values reported before a problem's index (problem count, run number) are not problem
    // values. The listener hooks cannot mark this point: they run before ProgressListener
    // reports the run number.
    if(key == ResultKey::ProblemIndex)
    {
        m_problemValues.clear();
        m_problemValuesStored = false;
        m_inSolution          = false;
    }

    if(!m_inSolution)
    {
        m_problemValues[key] = value;
        return;
    }

    // A failing verdict is sticky within a solution, like in ResultFileReporter.
    if(key == ResultKey::Validation)
    {
        auto found = m_runValues.find(key);
        if(found != m_runValues.end() && !isPassingValidation(found->second))
            return;
    }
    m_runValues[key] = value;
}

void DatabaseReporter::storeProblemSizes(std::vector<size_t> const& sizes)
{
    for(int32_t i = 0; i < static_cast<int32_t>(sizes.size()); ++i)
        store(sizeKey(i), formatValue(sizes[i]));
}

void DatabaseReporter::reportValue_string(std::string const& key, std::string const& value)
{
    store(key, value);
}

void DatabaseReporter::reportValue_uint(std::string const& key, uint64_t value)
{
    store(key, formatValue(value));
}

void DatabaseReporter::reportValue_int(std::string const& key, int64_t value)
{
    if(key == ResultKey::SyncEnqueues)
    {
        m_pendingEnqueues = value;
        return;
    }
    store(key, formatValue(value));
}

void DatabaseReporter::reportValue_double(std::string const& key, double value)
{
    if(key == ResultKey::SyncTimeUS)
    {
        m_iterations.push_back(
            IterationRecord{static_cast<int64_t>(m_iterations.size()), value, m_pendingEnqueues});
        return;
    }
    store(key, formatValue(value));
}

void DatabaseReporter::reportValue_sizes(std::string const& key, std::vector<size_t> const& value)
{
    store(key, formatSizes(value));

    if(key == ResultKey::ProblemSizes)
        storeProblemSizes(value);
}

void DatabaseReporter::reportValue_vecOfSizes(std::string const&                      key,
                                              std::vector<std::vector<size_t>> const& value)
{
    std::string text = "(";
    bool        first = true;
    for(std::vector<size_t> const& sizes : value)
    {
        if(!first)
            text += ",";
        text += formatSizes(sizes);
        first = false;
    }
    store(key, text + ")");

    if(key == ResultKey::ProblemSizes && !value.empty())
        storeProblemSizes(value[0]);
}

void DatabaseReporter::preSolution(ContractionSolution* const)
{
    m_runValues.clear();
    m_iterations.clear();
    m_pendingEnqueues = 0;
    m_inSolution      = true;
}

void DatabaseReporter::finalizeReport() {}
