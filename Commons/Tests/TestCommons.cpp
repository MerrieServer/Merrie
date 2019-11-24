#include <gtest/gtest.h>
#include <Commons/Commons.hpp>

TEST(TestCommons, TestCopyAndMoveSemantics) {
    class NonCopyableMoveable {
        public:
            NON_COPYABLE(NonCopyableMoveable);
            TRIVIALLY_MOVEABLE(NonCopyableMoveable);

            ~NonCopyableMoveable() = default;
    };

    EXPECT_TRUE(std::is_trivially_move_assignable_v<NonCopyableMoveable>);
    EXPECT_TRUE(std::is_trivially_move_constructible_v<NonCopyableMoveable>);
    EXPECT_FALSE(std::is_copy_assignable_v<NonCopyableMoveable>);
    EXPECT_FALSE(std::is_copy_constructible_v<NonCopyableMoveable>);

    class NonMoveableCopyable {
        public:
            NON_MOVEABLE(NonMoveableCopyable);
            TRIVIALLY_COPYABLE(NonMoveableCopyable);

            ~NonMoveableCopyable() = default;
    };

    EXPECT_TRUE(std::is_trivially_copy_assignable_v<NonMoveableCopyable>);
    EXPECT_TRUE(std::is_trivially_copy_constructible_v<NonMoveableCopyable>);
    EXPECT_FALSE(std::is_trivially_copyable_v<NonMoveableCopyable>);
    EXPECT_FALSE(std::is_move_assignable_v<NonMoveableCopyable>);
    EXPECT_FALSE(std::is_move_constructible_v<NonMoveableCopyable>);

    class MoveableCopyable {
        public:
            TRIVIALLY_MOVEABLE(MoveableCopyable);
            TRIVIALLY_COPYABLE(MoveableCopyable);

            ~MoveableCopyable() = default;
    };


    EXPECT_TRUE(std::is_trivially_copy_assignable_v<MoveableCopyable>);
    EXPECT_TRUE(std::is_trivially_copy_constructible_v<MoveableCopyable>);
    EXPECT_TRUE(std::is_trivially_move_assignable_v<MoveableCopyable>);
    EXPECT_TRUE(std::is_trivially_move_constructible_v<MoveableCopyable>);
    EXPECT_TRUE(std::is_trivially_copyable_v<MoveableCopyable>);


    class NonMoveableNonCopyable {
        public:
            NON_MOVEABLE(NonMoveableNonCopyable);
            NON_COPYABLE(NonMoveableNonCopyable);

            ~NonMoveableNonCopyable() = default;
    };

    EXPECT_FALSE(std::is_copy_assignable_v<NonMoveableNonCopyable>);
    EXPECT_FALSE(std::is_copy_constructible_v<NonMoveableNonCopyable>);
    EXPECT_FALSE(std::is_move_assignable_v<NonMoveableNonCopyable>);
    EXPECT_FALSE(std::is_move_constructible_v<NonMoveableNonCopyable>);
}

TEST(TestCommons, TestExceptions) {
    M_DECLARE_EXCEPTION(SimpleException);

    try {
        throw SimpleException("Hello");
        GTEST_FAIL() << "code continues after exception";
    } catch (const SimpleException& ex) {
        ASSERT_STREQ(ex.what(), "Hello");
    }

    M_DECLARE_EXCEPTION(AdvancedExceptionBase);
    M_DECLARE_EXCEPTION_EX(AdvancedExceptionDerieved, AdvancedExceptionBase);

    bool handled = false;
    try {
        throw AdvancedExceptionDerieved("Hello2");
        GTEST_FAIL() << "code continues after exception";
    } catch (const AdvancedExceptionBase& ex) {
        ASSERT_STREQ(ex.what(), "Hello2");
        handled = true;
    }

    ASSERT_TRUE(handled) << "exception not handled";
}


TEST(TestCommons, TestAssertions) {
#ifdef M_ASSERTIONS_ENABLED
    M_ASSERT(true, "This will never throw");

    bool handled = false;

    try {
        M_ASSERT(false, "This will always throw");
    }
    catch (const Merrie::AssertionError& ex) {
        ASSERT_STREQ("This will always throw", ex.what());
        handled = true;
    }

    ASSERT_TRUE(handled) << "exception not handled";
#endif
}