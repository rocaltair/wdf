#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>

#if LUA_VERSION_NUM < 502
# ifndef luaL_newlib
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
# endif
# ifndef lua_setuservalue
#  define lua_setuservalue(L, n) lua_setfenv(L, n)
# endif
# ifndef lua_getuservalue
#  define lua_getuservalue(L, n) lua_getfenv(L, n)
# endif
#endif

#define CLS_READER "lwdf.reader"
#define CLS_WRITER "lwdf.writer"

#define check_reader(L, idx) luaL_checkudata(L, idx, CLS_READER)
#define check_writer(L, idx) luaL_checkudata(L, idx, CLS_WRITER)

static int l_new_writer(lua_State *L)
{
	return 0;
}

static int l_write_file(lua_State *L)
{
	return 0;
}

static int l_write_tail(lua_State *L)
{
	return 0;
}

static int l_writer_gc(lua_State *L)
{
	return 0;
}

static int l_new_reader(lua_State *L)
{
	return 0;
}

static int l_read_file(lua_State *L)
{
	return 0;
}

static int l_reader_gc(lua_State *L)
{
	return 0;
}

static int l_open_cls_reader(lua_State *L)
{
	return 0;
}

static int l_open_cls_writer(lua_State *L)
{
	return 0;
}

int luaopen_lwdf(lua_State* L)
{
	luaL_Reg lfuncs[] = {
		{"new_reader", l_new_reader},
		{"new_writer", l_new_writer},
		{NULL, NULL},
	};
	l_open_cls_reader(L);
	l_open_cls_writer(L);
	luaL_newlib(L, lfuncs);
	return 1;
}
