#ifndef  _WDF_H_DJ1LDEKK_
#define  _WDF_H_DJ1LDEKK_

#if defined (__cplusplus)
extern "C" {
#endif

typedef uint64_t offset_t;

typedef ssize_t (*wdf_write_cb)(off_t offset, const void *buf, size_t sz, void *ud);
typedef ssize_t (*wdf_read_cb)(off_t offset, void *buf, size_t sz, void *ud);
typedef uint32_t (*wdf_hash_cb)(const char *buf);

struct wdf_header_s;
struct wdf_item_s;
struct wdf_writer_s;
struct wdf_reader_s;

typedef struct wdf_header_s wdf_header_t;
typedef struct wdf_item_s wdf_item_t;
typedef struct wdf_writer_s wdf_writer_t;
typedef struct wdf_reader_s wdf_reader_t;

wdf_writer_t *wdf_writer_open(wdf_write_cb *cb, void *ud);
int wdf_append_file(wdf_writer_t *w,
		     const char *path,
		     const void *buf, size_t sz);
int wdf_write(wdf_writer_t *w);
void wdf_writer_close(wdf_writer_t *w);

wdf_reader_t *wdf_reader_open(wdf_read_cb *cb, void *ud);
ssize_t wdf_read(wdf_reader_t *r,
		 const char *path,
		 void *buf, size_t sz);
void wdf_reader_close(wdf_reader_t *r);

#if defined (__cplusplus)
}	/*end of extern "C"*/
#endif

#endif /* end of include guard:  _WDF_H_DJ1LDEKK_ */

