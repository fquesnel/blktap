#include "unity.h"
#include <stdlib.h>

/* Header file for SUT */
#include "block-nbd.h"

/* Mocks */
#include "mock_tapdisk-fdreceiver.h"
#include "mock_tapdisk-interface.h"
#include "mock_tapdisk-log.h"
#include "mock_tapdisk-server.h"
#include "mock_tapdisk-utils.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_tdnbd_queue_read_computes_size_and_offset_correctly(void)
{
    // Initialisation
    td_driver_t driver;
    td_request_t treq;

    struct tdnbd_data prv;
    int expected_size;
    uint64_t expected_offset;

    driver.data = &prv;
    treq.secs = 10;
    treq.sec = 512;
    driver.info.sector_size = 2048;

    prv.flags = 0;
    prv.nr_free_count = 1;
    INIT_LIST_HEAD(&prv.free_reqs);
    INIT_LIST_HEAD(&prv.pending_reqs);
    struct td_nbd_request free_req;
    INIT_LIST_HEAD(&free_req.queue);
    list_move_tail(&free_req.queue, &prv.free_reqs);

    // Expectations
    expected_size = treq.secs * SECTOR_SIZE;
    expected_offset = treq.sec * SECTOR_SIZE;

    tapdisk_server_register_event_IgnoreAndReturn(0);
    ntohll_ExpectAndReturn(expected_offset, expected_offset);

    // Call to the method to test
    tdnbd_queue_read(&driver, treq);

    // Checks
    struct td_nbd_request *req = list_entry(
        prv.pending_reqs.next,
        struct td_nbd_request,
        queue);

    TEST_ASSERT_EQUAL_UINT(htonl(expected_size), req->nreq.len);
    TEST_ASSERT_EQUAL_UINT(expected_offset, req->nreq.from);
}

void test_tdnbd_queue_write_computes_size_and_offset_correctly(void)
{
    // Initialisation
    td_driver_t driver;
    td_request_t treq;

    struct tdnbd_data prv;
    int expected_size;
    uint64_t expected_offset;

    driver.data = &prv;
    treq.secs = 10;
    treq.sec = 512;
    driver.info.sector_size = 2048;

    prv.flags = 0;
    prv.nr_free_count = 1;
    INIT_LIST_HEAD(&prv.free_reqs);
    INIT_LIST_HEAD(&prv.pending_reqs);
    struct td_nbd_request free_req;
    INIT_LIST_HEAD(&free_req.queue);
    list_move_tail(&free_req.queue, &prv.free_reqs);

    // Expectations
    expected_size = treq.secs * SECTOR_SIZE;
    expected_offset = treq.sec * SECTOR_SIZE;

    tapdisk_server_register_event_IgnoreAndReturn(0);
    ntohll_ExpectAndReturn(expected_offset, expected_offset);

    // Call to the method to test
    tdnbd_queue_write(&driver, treq);

    // Checks
    struct td_nbd_request *req = list_entry(
        prv.pending_reqs.next,
        struct td_nbd_request,
        queue);

    TEST_ASSERT_EQUAL_UINT(htonl(expected_size), req->nreq.len);
    TEST_ASSERT_EQUAL_UINT(expected_offset, req->nreq.from);
}
