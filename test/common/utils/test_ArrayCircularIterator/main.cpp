#include <unity.h> // testing framework
#include "ArrayCircularIterator.hpp"

void setUp(void) {}

void tearDown(void) {}

void test_constructsWithoutOffsetFromCppArray()
{
	std::array<uint8_t, 10> array = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator(array);

	TEST_ASSERT_EQUAL_PTR(iter.begin, array.begin());
	TEST_ASSERT_EQUAL_PTR(iter.begin, iter.pointer);
	TEST_ASSERT_EQUAL(iter.samplesFromStart(), 0);
	TEST_ASSERT_EQUAL(iter.samplesToEnd(), 10);
	TEST_ASSERT_EQUAL(*iter, 0);
}

void test_constructsWithoutOffsetFromCArray()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array);

	TEST_ASSERT_EQUAL_PTR(iter.begin, array);
	TEST_ASSERT_EQUAL_PTR(iter.begin, iter.pointer);
	TEST_ASSERT_EQUAL(iter.samplesFromStart(), 0);
	TEST_ASSERT_EQUAL(iter.samplesToEnd(), 10);
	TEST_ASSERT_EQUAL(*iter, 0);
}

void test_constructsWithOffset()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array, 7);

	TEST_ASSERT_EQUAL_PTR(iter.begin, array);
	TEST_ASSERT_EQUAL_PTR(iter.pointer, array + 7);
	TEST_ASSERT_EQUAL(iter.samplesFromStart(), 7);
	TEST_ASSERT_EQUAL(iter.samplesToEnd(), 3);
	TEST_ASSERT_EQUAL(iter.get(), 77);
	TEST_ASSERT_EQUAL(*iter, 77);
}

void test_incrementWithoutOverflow()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array, 7);

	++iter;

	TEST_ASSERT_EQUAL_PTR(iter.pointer, array + 8);
	TEST_ASSERT_EQUAL(*iter, 88);
}

void test_incrementWithOverflow()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array, 9);

	++iter;

	TEST_ASSERT_EQUAL_PTR(iter.pointer, array);
	TEST_ASSERT_EQUAL(*iter, 0);
}

void test_decrementWithoutUnderflow()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array, 7);

	--iter;

	TEST_ASSERT_EQUAL_PTR(iter.pointer, array + 6);
	TEST_ASSERT_EQUAL(*iter, 66);
}

void test_decrementWithUnderflow()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array, std::size_t(0));

	--iter;

	TEST_ASSERT_EQUAL_PTR(iter.pointer, array + 9);
	TEST_ASSERT_EQUAL(*iter, 99);
}

void test_addWithoutOverflow()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array, 3);

	iter += 4;

	TEST_ASSERT_EQUAL_PTR(iter.pointer, array + 7);
	TEST_ASSERT_EQUAL(*iter, 77);
}

void test_addWithOverflow()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array, 8);

	iter += 4;

	TEST_ASSERT_EQUAL_PTR(iter.pointer, array + 2);
	TEST_ASSERT_EQUAL(*iter, 22);
}

void test_subtractWithoutUnderflow()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array, 7);

	iter -= 4;

	TEST_ASSERT_EQUAL_PTR(iter.pointer, array + 3);
	TEST_ASSERT_EQUAL(*iter, 33);
}

void test_subtractWithUnderflow()
{
	uint8_t array[] = { 0, 11, 22, 33, 44, 55, 66, 77, 88, 99 };
	auto iter = ArrayCircularIterator<uint8_t, 10>(array, 2);

	iter -= 4;

	TEST_ASSERT_EQUAL_PTR(iter.pointer, array + 8);
	TEST_ASSERT_EQUAL(*iter, 88);
}

int main()
{
	UNITY_BEGIN();
	RUN_TEST(test_constructsWithoutOffsetFromCppArray);
	RUN_TEST(test_constructsWithoutOffsetFromCArray);
	RUN_TEST(test_constructsWithOffset);
	RUN_TEST(test_incrementWithoutOverflow);
	RUN_TEST(test_incrementWithOverflow);
	RUN_TEST(test_decrementWithoutUnderflow);
	RUN_TEST(test_decrementWithUnderflow);
	RUN_TEST(test_addWithoutOverflow);
	RUN_TEST(test_addWithOverflow);
	RUN_TEST(test_subtractWithoutUnderflow);
	RUN_TEST(test_subtractWithUnderflow);
	UNITY_END();
}
