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

void test_schedule_data_read_sets_offset_correctly_w_footer_HD_TYPE_FIXED(void)
{
    // Initialisation
    struct vhd_state s;
    td_request_t treq;
    vhd_flag_t flags;
    struct vhd_request *req;
    int expected_size;
    uint64_t expected_offset;

    treq.secs = 10;
    treq.sec = 1024;

    s.vhd.footer.type = HD_TYPE_FIXED;
    s.vreq_free_count = 1;
    s.vreq_free[0] = &(struct vhd_request){};
    req = s.vreq_free[0];

    // Expectations
    expected_size = treq.secs * SECTOR_SIZE;
    expected_offset = treq.sec * (uint64_t) SECTOR_SIZE;

    td_prep_read_Expect(
        &req->tiocb,
        s.vhd.fd,
        req->treq.buf,
        expected_size,
        expected_offset,
        vhd_complete,
        req);

    td_queue_tiocb_Ignore();

    __tlog_write_Ignore();

    // Call to the method to test
    schedule_data_read(&s, treq, flags);
}
