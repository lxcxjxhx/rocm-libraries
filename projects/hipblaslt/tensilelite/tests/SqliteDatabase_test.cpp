// Copyright Advanced Micro Devices, Inc., or its affiliates.
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>

#include "SqliteDatabase.hpp"

#include <utility>

using namespace TensileLite::Client;

static int64_t countRows(SqliteConnection& db)
{
    SqliteStatement statement(db, "SELECT COUNT(*) FROM t");
    EXPECT_TRUE(statement.step());
    return statement.columnInt(0);
}

TEST(SqliteDatabase, FailingStatementThrowsSqliteError)
{
    SqliteConnection db(":memory:");
    EXPECT_THROW(db.execute("SELECT * FROM missing"), SqliteError);
    EXPECT_THROW(SqliteStatement(db, "NOT SQL"), SqliteError);
}

TEST(SqliteDatabase, UncommittedTransactionRollsBack)
{
    SqliteConnection db(":memory:");
    db.execute("CREATE TABLE t(x)");
    {
        SqliteTransaction transaction(db);
        db.execute("INSERT INTO t VALUES(1)");
    }
    EXPECT_EQ(countRows(db), 0);
    {
        SqliteTransaction transaction(db);
        db.execute("INSERT INTO t VALUES(1)");
        transaction.commit();
    }
    EXPECT_EQ(countRows(db), 1);
}

TEST(SqliteDatabase, MovedFromConnectionIsSafe)
{
    SqliteConnection first(":memory:");
    first.execute("CREATE TABLE t(x)");
    SqliteConnection second(std::move(first));
    second.execute("INSERT INTO t VALUES(1)");
    EXPECT_EQ(countRows(second), 1);

    SqliteConnection third(":memory:");
    third = std::move(second);
    EXPECT_EQ(countRows(third), 1);
}
