#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "wdf.h"

typedef uint64_t offset_t;

struct wdf_header_s {
	offset_t tail_offset;
	uint32_t count;
};

struct wdf_item_s {
	uint32_t hash_id;
	offset_t offset;
	offset_t content_sz;
	offset_t blank_sz;
};

struct wdf_writer_s {
	wdf_header_t header;
	wdf_write_cb write_cb;
	wdf_hash_cb hash_cb;
	void *ud;
	uint32_t item_sz;
	uint32_t item_cap;
	wdf_item_t *items;
};

struct wdf_reader_s {
	wdf_header_t header;
	wdf_read_cb read_cb;
	wdf_hash_cb hash_cb;
	void *ud;
};

uint32_t ap_hash(const char *str)
{
	uint32_t hash = 0;
	int i;

	for (i=0; *str; i++) {
		if ((i & 1) == 0) {
			hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
		} else {
			hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
		}
	}

	return (hash & 0x7FFFFFFF);
}

uint32_t djb_hash(const char *str)
{
	uint32_t hash = 5381;
	while (*str) {
		hash += (hash << 5) + (*str++);
	}
	return (hash & 0x7FFFFFFF);
}

uint32_t bkrd_hash(const char *str)
{
	uint32_t seed = 131;
	uint32_t hash = 0;
	while (*str) {
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

static int comp(const void *a, const void *b)
{
	const wdf_item_t *ia = a;
	const wdf_item_t *ib = b;
	int64_t diff = (int64_t)ia->hash_id - (int64_t)ib->hash_id;
	return (int)diff;
}

static int resize_items(wdf_writer_t *w)
{
	wdf_item_t *new_items = realloc(w->items, sizeof(wdf_item_t) * w->item_cap);
	if (new_items == NULL)
		return -1;
	w->items = new_items;
	return 0;
}

wdf_writer_t *wdf_writer_open(wdf_write_cb cb, void *ud)
{
	wdf_writer_t *w = malloc(sizeof(*w));
	if (w == NULL)
		goto finished;

	w->items = malloc(sizeof(wdf_item_t) * w->item_cap);
	if (w->items == NULL) {
		free(w);
		w = NULL;
		goto finished;
	}

	w->hash_cb = bkrd_hash;
	w->write_cb = cb;
	w->ud = ud;
	w->item_sz = 0;
	w->item_cap = 32;
	w->header.tail_offset = 0;
	w->header.tail_offset += sizeof(wdf_header_t);
finished:
	return w;
}

int wdf_append_file(wdf_writer_t *w,
		const char *path,
		const void *buf, size_t sz)
{
	uint32_t n;
	if (w->item_sz >= w->item_cap && resize_items(w) != 0)
		return -1;

	if (w->write_cb(w->header.tail_offset, buf, sz, w->ud) < 0)
		return -2;

	n = w->item_sz;
	w->items[n].hash_id = w->hash_cb(path);
	w->items[n].offset = w->header.tail_offset;
	w->items[n].content_sz = sz;
	w->items[n].blank_sz = 0;

	w->header.tail_offset += sz;
	w->item_sz++;

	return 0;
}

int wdf_write(wdf_writer_t *w)
{
	int i;
	qsort(w->items, w->item_sz, sizeof(wdf_item_t), comp);
	w->header.count = w->item_sz;
	w->write_cb(0, &w->header, sizeof(wdf_header_t), w->ud);
	// w->write_cb(w->header.tail_offset, &w->items, sizeof(wdf_item_t) * w->item_sz, w->ud);
	for (i = 0; i < w->item_sz; i++) {
		int offset = w->header.tail_offset + i * sizeof(wdf_item_t);
		w->write_cb(offset, &w->items[i], sizeof(wdf_item_t), w->ud);
	}
	return 0;
}

void wdf_writer_close(wdf_writer_t *w)
{
	free(w->items);
	free(w);
}

wdf_reader_t *wdf_reader_open(wdf_read_cb cb, void *ud)
{
	wdf_reader_t *r = malloc(sizeof(*r));
	r->hash_cb = bkrd_hash;
	r->read_cb = cb;
	r->ud = ud;
	r->read_cb(0, &r->header, sizeof(r->header), ud);
	return r;
}

#define READ_ITEM(r, idx, item) \
	r->read_cb(r->header.tail_offset + sizeof(item) * (idx), (void *)&item, sizeof(item), r->ud)

ssize_t wdf_read(wdf_reader_t *r,
		const char *path,
		void *buf, size_t sz)
{
	int is_found = 0;
	int mid;
	int head = 0;
	int tail = r->header.count - 1;
	wdf_item_t item;
	uint32_t mid_hash;
	uint32_t hash_id = r->hash_cb(path);
	assert(buf != NULL);
	memset(&item, 0, sizeof(item));

	while (head <= tail) {
		mid = (head + tail) / 2;
		READ_ITEM(r, mid, item);
		mid_hash = item.hash_id;
		if (hash_id == mid_hash) {
			is_found = 1;
			break;
		} else if (hash_id < mid_hash) {
			tail = mid - 1;
		} else if (hash_id > mid_hash) {
			head = mid + 1;
		}
	}
	if (!is_found)
		return -1;
	if (item.content_sz > sz)
		return -1;
	return r->read_cb(item.offset, buf, item.content_sz, r->ud);
}

void wdf_reader_close(wdf_reader_t *r)
{
	free(r);
}
