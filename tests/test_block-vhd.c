#include "unity.h"
#include <stdlib.h>

/* Header file for SUT */
#include "drivers/block-vhd.h"

/* Mocks */
#include "mock_libvhd.h"
#include "mock_tapdisk-driver.h"
#include "mock_tapdisk-interface.h"
#include "mock_tapdisk-log.h"
#include "mock_tapdisk-storage.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_aio_read_computes_size_correctly(void)
{
    // Initialisation
    struct vhd_state s;
    struct vhd_request req;
    uint64_t offset;

    int expected_size;

    req.treq.secs = 23;
    offset = 1024;

    // Expectations
    expected_size = req.treq.secs * SECTOR_SIZE;

    td_prep_read_Expect(
        &req.tiocb,
        s.vhd.fd,
        req.treq.buf,
        expected_size,
        offset,
        vhd_complete,
        &req);

    td_queue_tiocb_Ignore();

    __tlog_write_Ignore();

    // Call to the function to test
    aio_read(&s, &req, offset);
}

void test_aio_write_computes_size_correctly(void)
{
    // Initialisation
    struct vhd_state s;
    struct vhd_request req;
    uint64_t offset;

    int expected_size;

    req.treq.secs = 23;
    offset = 1024;

    // Expectations
    expected_size = req.treq.secs * SECTOR_SIZE;

    td_prep_write_Expect(
        &req.tiocb,
        s.vhd.fd,
        req.treq.buf,
        expected_size,
        offset,
        vhd_complete,
        &req);

    td_queue_tiocb_Ignore();

    __tlog_write_Ignore();

    // Call to the function to test
    aio_write(&s, &req, offset);
}

