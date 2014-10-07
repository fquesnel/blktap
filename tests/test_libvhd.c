#include "unity.h"
#include <stdlib.h>

/* Header file for SUT */
#include "libvhd.h"
#include "canonpath.h"
#include "relative-path.h"

/* Mocks */
#include "mock_tapdisk-driver.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void assert_sector_to_bytes_ok(uint64_t sectors)
{
    TEST_ASSERT_EQUAL_UINT(
        sectors * SECTOR_SIZE,
        vhd_sectors_to_bytes(sectors));
}

void test_sectors_to_bytes_returns_the_right_value(void)
{
    assert_sector_to_bytes_ok(0);
    assert_sector_to_bytes_ok(23);
    assert_sector_to_bytes_ok((uint64_t) 1 << 22);
    assert_sector_to_bytes_ok((uint64_t) 1 << 54);
}
