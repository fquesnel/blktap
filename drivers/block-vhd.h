/*
 * Copyright (C) 2012      Citrix Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#ifndef __BLOCK_VHD_H__
#define __BLOCK_VHD_H__

#include "libvhd.h"
#include "tapdisk.h"
#include "tapdisk-queue.h"

#define VHD_CACHE_SIZE               32
#define VHD_REQS_DATA                TAPDISK_DATA_REQUESTS

typedef uint8_t vhd_flag_t;

struct vhd_state;
struct vhd_request;

struct vhd_req_list {
	struct vhd_request       *head;
	struct vhd_request       *tail;
};

struct vhd_transaction {
	int                       error;
	int                       closed;
	int                       started;
	int                       finished;
	vhd_flag_t                status;
	struct vhd_req_list       requests;
};

struct vhd_request {
	int                       error;
	uint8_t                   op;
	vhd_flag_t                flags;
	td_request_t              treq;
	struct tiocb              tiocb;
	struct vhd_state         *state;
	struct vhd_request       *next;
	struct vhd_transaction   *tx;
};

struct vhd_bat_state {
	vhd_bat_t                 bat;
	vhd_batmap_t              batmap;
	vhd_flag_t                status;
	uint32_t                  pbw_blk;     /* blk num of pending write */
	uint64_t                  pbw_offset;  /* file offset of same */
	struct vhd_request        req;         /* for writing bat table */
	struct vhd_request        zero_req;    /* for initializing bitmaps */
	char                     *bat_buf;
};

struct vhd_bitmap {
	uint32_t                  blk;
	uint64_t                  seqno;       /* lru sequence number */
	vhd_flag_t                status;

	char                     *map;         /* map should only be modified
					        * in finish_bitmap_write */
	char                     *shadow;      /* in-memory bitmap changes are 
					        * made to shadow and copied to
					        * map only after having been
					        * flushed to disk */
	struct vhd_transaction    tx;          /* transaction data structure
						* encapsulating data, bitmap, 
						* and bat writes */
	struct vhd_req_list       queue;       /* data writes waiting for next
						* transaction */
	struct vhd_req_list       waiting;     /* pending requests that cannot
					        * be serviced until this bitmap
					        * is read from disk */
	struct vhd_request        req;
};

struct vhd_state {
	vhd_flag_t                flags;

        /* VHD stuff */
	vhd_context_t             vhd;
	uint32_t                  spp;         /* sectors per page */
	uint32_t                  spb;         /* sectors per block */
	uint64_t                  first_db;    /* pointer to datablock 0 */

	/**
	 * Pointer to the next (unallocated) datablock. If greater than UINT_MAX,
	 * there are no more blocks available.
	 */
	uint64_t                  next_db;

	struct vhd_bat_state      bat;

	uint64_t                  bm_lru;      /* lru sequence number */
	uint32_t                  bm_secs;     /* size of bitmap, in sectors */
	struct vhd_bitmap        *bitmap[VHD_CACHE_SIZE];

	int                       bm_free_count;
	struct vhd_bitmap        *bitmap_free[VHD_CACHE_SIZE];
	struct vhd_bitmap         bitmap_list[VHD_CACHE_SIZE];

	int                       vreq_free_count;
	struct vhd_request       *vreq_free[VHD_REQS_DATA];
	struct vhd_request        vreq_list[VHD_REQS_DATA];

	/* for redundant bitmap writes */
	int                       padbm_size;
	char                     *padbm_buf;
	long int                  debug_skipped_redundant_writes;
	long int                  debug_done_redundant_writes;

	td_driver_t              *driver;

	uint64_t                  queued;
	uint64_t                  completed;
	uint64_t                  returned;
	uint64_t                  reads;
	uint64_t                  read_size;
	uint64_t                  writes;
	uint64_t                  write_size;
};

void vhd_complete(void *, struct tiocb *, int);
#endif
