#include "myTests.h"
#include "../queue/Queue.h"

static Queue *queue;

TEST_GROUP(queue_group);

TEST_SETUP(queue_group)
{
    queue = Queue_create(3,5);
}

TEST_TEAR_DOWN(queue_group)
{
    Queue_destroy(queue);
}

TEST(queue_group, write_one_check_result)
{
    uint8_t writeBuffer[5] = "test";
    int result = 0;

    result = Queue_write(queue, (char *)writeBuffer, 4);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "Write first element error");
}

TEST(queue_group, write_too_much_check_result)
{
    uint8_t writeBuffer[5] = "test";
    int result = 0;

    result = Queue_write(queue, (char *)writeBuffer, 4);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "Write first element error");

    result = Queue_write(queue, (char *)writeBuffer, 4);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "Write second element error");

    result = Queue_write(queue, (char *)writeBuffer, 4);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "Write third element error");

    result = Queue_write(queue, (char *)writeBuffer, 4);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, result, "Write out of queue");

}

TEST(queue_group, write_too_long_item_check_result)
{
    uint8_t writeBuffer[9] = "test1234";
    int result = 0;

    result = Queue_write(queue, (char *)writeBuffer, 8);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, result, "Write too long item");
}

TEST(queue_group, read_from_empty)
{
    uint8_t readBuffer[5];
    int result = 0;

    result = Queue_read(queue, (char *)readBuffer);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, result, "Read from empty queue");
}

TEST(queue_group, write_read_sequence)
{
    uint8_t writeBuffer[5] = "test";
    uint8_t readBuffer[5];
    int result = 0;

    writeBuffer[4] = '1';
    result = Queue_write(queue, (char *)writeBuffer, 5);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "Write read sequence: first write");

    result = Queue_read(queue, (char *)readBuffer);
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, result, "Write read sequence result 1");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(readBuffer, "test1", 5, "Write read sequence: check read buffer");

    writeBuffer[4] = '2';
    result = Queue_write(queue, (char *)writeBuffer, 5);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "Write read sequence: second write");
    writeBuffer[4] = '3';
    result = Queue_write(queue, (char *)writeBuffer, 5);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "Write read sequence: third write");
    result = Queue_write(queue, (char *)writeBuffer, 4);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "Write read sequence: fourth write");

    result = Queue_read(queue, (char *)readBuffer);
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, result, "Write read sequence result 2");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(readBuffer, "test2", 5, "Write read sequence: check read buffer");
    result = Queue_read(queue, (char *)readBuffer);
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, result, "Write read sequence result 3");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(readBuffer, "test3", 5, "Write read sequence: check read buffer");
    result = Queue_read(queue, (char *)readBuffer);
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, result, "Write read sequence result 4");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(readBuffer, "test", 4, "Write read sequence: check read buffer");

}

TEST_GROUP_RUNNER(queue_group)
{
    RUN_TEST_CASE(queue_group, write_one_check_result);
    RUN_TEST_CASE(queue_group, write_too_much_check_result);
    RUN_TEST_CASE(queue_group, write_too_long_item_check_result);
    RUN_TEST_CASE(queue_group, read_from_empty);
    RUN_TEST_CASE(queue_group, write_read_sequence);
}

void runAllTests(void)
{
    RUN_TEST_GROUP(queue_group);
}
