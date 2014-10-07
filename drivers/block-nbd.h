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

#ifndef __BLOCK_NBD_H__
#define __BLOCK_NBD_H__

#include <sys/socket.h>
#include <sys/un.h>
#include "tapdisk.h"
#include "tapdisk-driver.h"
#include "tapdisk-nbd.h"

#define MAX_NBD_REQS TAPDISK_DATA_REQUESTS

struct nbd_queued_io {
	char                   *buffer;
	int                     len;
	int                     so_far;
};

struct td_nbd_request {
	td_request_t            treq;
	struct nbd_request      nreq;
	int                     timeout_event;
	int                     fake;
	struct nbd_queued_io    header;
	struct nbd_queued_io    body;     /* in or out, depending on whether
					     type is read or write. */
	struct list_head        queue;
};

struct tdnbd_data
{
	int                     writer_event_id;
	struct list_head        sent_reqs;
	struct list_head        pending_reqs;
	struct list_head        free_reqs;
	struct td_nbd_request   requests[MAX_NBD_REQS];
	int                     nr_free_count;

	int                     reader_event_id;
	struct nbd_reply        current_reply;
	struct nbd_queued_io    cur_reply_qio;
	struct td_nbd_request  *curr_reply_req;

	int                     socket;
	/*
	 * TODO tapdisk can talk to an Internet socket or a UNIX domain socket.
	 * Try to group struct members accordingly e.g. in a union.
	 */
	struct sockaddr_in     *remote;
	struct sockaddr_un      remote_un;
	char                   *peer_ip;
	int                     port;
	char                   *name;

	int                     flags;
	int                     closed;
};
#endif
