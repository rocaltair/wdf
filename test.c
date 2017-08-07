#include <stdio.h>
#include <string.h>
#include "wdf.h"

ssize_t my_write(off_t offset, const void *buf, size_t sz, void *ud)
{
	memcpy((((char *)ud) + offset), buf, sz);
	return sz;
}

void write_test(char *mm, size_t sz)
{
	int i;
	wdf_writer_t *w = wdf_writer_open(my_write, mm);
	struct {
		const char *s;
		const char *content;
	} list[] = {
		{ "111.lua", "hello 11"},
		{ "222.lua", "hello 222 fjkdska"},
		{ "333.lua", "hello 333 fs7u8234"},
		{ "444.lua", "hello 444 2"},
	};
	for (i = 0; i < sizeof(list)/sizeof(list[0]); i++) {
		wdf_append_file(w, list[i].s, list[i].content, strlen(list[i].content) + 1);
	}
	wdf_write(w);
	wdf_writer_close(w);
}

ssize_t my_read(off_t offset, void *buf, size_t sz, void *ud)
{
	memcpy(buf, ((char *)ud) + offset, sz);
	return sz;
}

void read_test(char *mm, size_t sz)
{
	int i;
	char buf[128] = {0};
	char *list[] = {
		"111.lua",
		"222.lua",
		"333.lua",
		"444.lua",
	};
	wdf_reader_t *r = wdf_reader_open(my_read, mm);
	for (i = 0; i < sizeof(list)/sizeof(list[0]); i++) {
		int sz = wdf_read(r, list[i], buf, sizeof(buf));
		printf("%s %s %d\n", list[i], buf, sz);
	}
	wdf_reader_close(r);
}

int main(int argc, char **argv)
{
	char mm[1024 * 8];
	write_test(mm, sizeof(mm));
	read_test(mm, sizeof(mm));
	return 0;
}
