// Copyright Advanced Micro Devices, Inc., or its affiliates.
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

#include "BenchmarkRecording.hpp"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef __linux__
#include <signal.h>
#include <sys/prctl.h>
#endif

using namespace TensileLite;
using namespace TensileLite::Client;

namespace
{
    // Unique temporary directory, removed with the -wal and -shm files it holds.
    class TempDb
    {
    public:
        TempDb()
        {
            std::string pattern
                = (std::filesystem::temp_directory_path() / "tensilelite_db_XXXXXX").string();
            char* created = mkdtemp(pattern.data());
            if(!created)
                throw std::runtime_error("cannot create temporary directory");
            m_dir = created;
        }

        ~TempDb()
        {
            std::error_code error;
            std::filesystem::remove_all(m_dir, error);
        }

        TempDb(TempDb const&)            = delete;
        TempDb& operator=(TempDb const&) = delete;

        std::string path() const
        {
            return (m_dir / "bench.db").string();
        }

    private:
        std::filesystem::path m_dir;
    };

    class BenchmarkRecordingTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            database = std::make_shared<BenchmarkDatabase>(
                tmp.path(),
                std::vector<SolutionRecord>{{10, "SolA", "specA", "msgpack"},
                                            {20, "SolB", "specB", "msgpack"}},
                "count=1\n",
                std::vector<InitConfigRecord>{{0}, {1}});
            reader.emplace(tmp.path());
            reporter = std::make_shared<DatabaseReporter>(database);
        }

        std::unique_ptr<BenchmarkSession> openSession()
        {
            return std::make_unique<BenchmarkSession>(database, 1, 2, IndirectFailurePolicy());
        }

        // Starts a problem and a solution the way main does.
        void startPair(int64_t problem)
        {
            reporter->report(ResultKey::ProblemIndex, problem);
            reporter->preSolution(nullptr);
        }

        // Whether the latest session has not ended.
        bool sessionOpen()
        {
            SqliteStatement statement(
                *reader, "SELECT end_time IS NULL FROM sessions ORDER BY id DESC LIMIT 1");
            EXPECT_TRUE(statement.step());
            return statement.columnInt(0) != 0;
        }

        // Fails every update of the table.
        void failUpdatesOf(std::string const& table)
        {
            reader->execute("CREATE TRIGGER fail_update BEFORE UPDATE ON " + table
                            + " BEGIN SELECT RAISE(ABORT, 'injected'); END");
        }

        TempDb                             tmp;
        std::shared_ptr<BenchmarkDatabase> database;
        // A second connection that reads the state and injects failures.
        std::optional<SqliteConnection>   reader;
        std::shared_ptr<DatabaseReporter> reporter;
    };
}

TEST_F(BenchmarkRecordingTest, PairRecorderMarksUnknownThenRecordsVerdict)
{
    std::unique_ptr<BenchmarkSession> session = openSession();
    startPair(0);

    PairRecorder recorder(*reporter, 0, 10);
    EXPECT_EQ(database->runStatus(0, 10), RunStatus::Unknown);

    reporter->report(ResultKey::Validation, std::string("PASSED"));
    recorder.finish();
    EXPECT_EQ(database->runStatus(0, 10), RunStatus::Pass);
}

TEST_F(BenchmarkRecordingTest, ExceptionLeavesPairUnknownAndSessionOpen)
{
    try
    {
        BenchmarkSession session(database, 1, 2, IndirectFailurePolicy());
        startPair(0);
        PairRecorder recorder(*reporter, 0, 10);
        throw std::runtime_error("kernel fault");
    }
    catch(std::runtime_error const&)
    {
    }

    EXPECT_EQ(database->runStatus(0, 10), RunStatus::Unknown);
    EXPECT_TRUE(sessionOpen());
}

TEST_F(BenchmarkRecordingTest, SessionEndsOnScopeExit)
{
    {
        BenchmarkSession session(database, 1, 2, IndirectFailurePolicy());
        EXPECT_TRUE(sessionOpen());
    }
    EXPECT_FALSE(sessionOpen());
}

TEST_F(BenchmarkRecordingTest, SessionEndsOnEarlyReturn)
{
    auto earlyReturn = [&]() -> int32_t {
        BenchmarkSession session(database, 1, 2, IndirectFailurePolicy());
        return 3;
    };
    EXPECT_EQ(earlyReturn(), 3);
    EXPECT_FALSE(sessionOpen());
}

TEST_F(BenchmarkRecordingTest, SessionStaysOpenWhenAnExceptionUnwinds)
{
    auto throwing = [&]() {
        BenchmarkSession session(database, 1, 2, IndirectFailurePolicy());
        throw std::runtime_error("boom");
    };
    EXPECT_THROW(throwing(), std::runtime_error);
    EXPECT_TRUE(sessionOpen());
}

TEST_F(BenchmarkRecordingTest, ExplicitEndIsIdempotentAndDestructionDoesNotEndAgain)
{
    testing::internal::CaptureStderr();
    {
        BenchmarkSession session(database, 1, 2, IndirectFailurePolicy());
        EXPECT_NO_THROW(session.end());
        EXPECT_FALSE(sessionOpen());
        EXPECT_NO_THROW(session.end());
    }
    EXPECT_EQ(testing::internal::GetCapturedStderr(), "");
    EXPECT_FALSE(sessionOpen());
}

TEST_F(BenchmarkRecordingTest, FailingEndThrowsExplicitlyAndIsSwallowedByDestructor)
{
    {
        BenchmarkSession session(database, 1, 2, IndirectFailurePolicy());
        failUpdatesOf("sessions");
        EXPECT_THROW(session.end(), SqliteError);
    }
    EXPECT_TRUE(sessionOpen());

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW({ BenchmarkSession session(database, 1, 2, IndirectFailurePolicy()); });
    std::string const message = testing::internal::GetCapturedStderr();
    EXPECT_NE(message.find("failed to end the benchmark session"), std::string::npos) << message;
    EXPECT_TRUE(sessionOpen());
}

TEST_F(BenchmarkRecordingTest, ClassifyPair)
{
    EXPECT_EQ(classifyPair(nullptr, 0, 10), PairRecording::Disabled);

    std::unique_ptr<BenchmarkSession> session = openSession();
    EXPECT_EQ(classifyPair(database.get(), 0, 10), PairRecording::Record);

    startPair(0);
    PairRecorder recorder(*reporter, 0, 10);
    reporter->report(ResultKey::Validation, std::string("PASSED"));
    recorder.finish();
    EXPECT_EQ(classifyPair(database.get(), 0, 10), PairRecording::SkippedByDatabase);
    EXPECT_EQ(classifyPair(database.get(), 0, 20), PairRecording::Record);
}

TEST(BenchmarkRecordingCatch, ReturnsTheClientValue)
{
    EXPECT_EQ(runCatchingBenchmarkFailures([]() { return 5; }), 5);
}

TEST(BenchmarkRecordingCatch, DeviceLostReturnsItsExitCodeSilently)
{
    testing::internal::CaptureStderr();
    int32_t const result
        = runCatchingBenchmarkFailures([]() -> int32_t { throw DeviceLostError(7); });
    EXPECT_EQ(result, 7);
    EXPECT_EQ(testing::internal::GetCapturedStderr(), "");
}

TEST(BenchmarkRecordingCatch, DatabaseErrorsReturnOne)
{
    testing::internal::CaptureStderr();
    int32_t result = runCatchingBenchmarkFailures(
        []() -> int32_t { throw BenchmarkDatabaseError("inconsistent"); });
    std::string message = testing::internal::GetCapturedStderr();
    EXPECT_EQ(result, 1);
    EXPECT_NE(message.find("benchmark database error: inconsistent"), std::string::npos);

    testing::internal::CaptureStderr();
    result  = runCatchingBenchmarkFailures([]() -> int32_t { throw SqliteError("disk full"); });
    message = testing::internal::GetCapturedStderr();
    EXPECT_EQ(result, 1);
    EXPECT_NE(message.find("benchmark database error: disk full"), std::string::npos);
}

TEST(BenchmarkRecordingCatch, OtherExceptionsPropagate)
{
    EXPECT_THROW(runCatchingBenchmarkFailures([]() -> int32_t { throw std::logic_error("bug"); }),
                 std::logic_error);
    EXPECT_THROW(
        runCatchingBenchmarkFailures([]() -> int32_t { throw std::runtime_error("other"); }),
        std::runtime_error);
}

TEST_F(BenchmarkRecordingTest, FailingRunUpdateBecomesDiagnosticAndExitCodeOne)
{
    BenchmarkSession session(database, 1, 2, IndirectFailurePolicy());
    failUpdatesOf("runs");
    startPair(0);

    testing::internal::CaptureStderr();
    int32_t const     result  = runCatchingBenchmarkFailures([&]() -> int32_t {
        PairRecorder recorder(*reporter, 0, 10);
        return 0;
    });
    std::string const message = testing::internal::GetCapturedStderr();
    EXPECT_EQ(result, 1);
    EXPECT_NE(message.find("benchmark database error"), std::string::npos) << message;
}

TEST_F(BenchmarkRecordingTest, FailingSessionEndBecomesNonZeroExit)
{
    failUpdatesOf("sessions");

    testing::internal::CaptureStderr();
    int32_t const     result  = runCatchingBenchmarkFailures([&]() -> int32_t {
        BenchmarkSession session(database, 1, 2, IndirectFailurePolicy());
        session.end();
        return 0;
    });
    std::string const message = testing::internal::GetCapturedStderr();
    EXPECT_NE(result, 0);
    EXPECT_NE(message.find("benchmark database error"), std::string::npos) << message;
    EXPECT_TRUE(sessionOpen());
}

#ifdef __linux__
TEST(DieWithParentTest, RequestsSigkillOnParentDeath)
{
    EXPECT_TRUE(dieWithParent());

    int32_t signalNumber = 0;
    ASSERT_EQ(prctl(PR_GET_PDEATHSIG, &signalNumber), 0);
    EXPECT_EQ(signalNumber, SIGKILL);

    // Do not let the test binary die with its own parent.
    prctl(PR_SET_PDEATHSIG, 0);
}
#endif
