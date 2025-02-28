PATH_LUA=../lua-5.4.7/src
OUT=.

LUA=\
$(PATH_LUA)/lapi.c \
$(PATH_LUA)/lauxlib.c \
$(PATH_LUA)/lbaselib.c \
$(PATH_LUA)/lcode.c \
$(PATH_LUA)/lcorolib.c \
$(PATH_LUA)/lctype.c \
$(PATH_LUA)/ldblib.c \
$(PATH_LUA)/ldebug.c \
$(PATH_LUA)/ldo.c \
$(PATH_LUA)/ldump.c \
$(PATH_LUA)/lfunc.c \
$(PATH_LUA)/lgc.c \
$(PATH_LUA)/linit.c \
$(PATH_LUA)/liolib.c \
$(PATH_LUA)/llex.c \
$(PATH_LUA)/lmathlib.c \
$(PATH_LUA)/lmem.c \
$(PATH_LUA)/loadlib.c \
$(PATH_LUA)/lobject.c \
$(PATH_LUA)/lopcodes.c \
$(PATH_LUA)/loslib.c \
$(PATH_LUA)/lparser.c \
$(PATH_LUA)/lstate.c \
$(PATH_LUA)/lstring.c \
$(PATH_LUA)/lstrlib.c \
$(PATH_LUA)/ltable.c \
$(PATH_LUA)/ltablib.c \
$(PATH_LUA)/ltm.c \
$(PATH_LUA)/lundump.c \
$(PATH_LUA)/lutf8lib.c \
$(PATH_LUA)/lvm.c \
$(PATH_LUA)/lzio.c

flags_mac=-Wall -std=c++17 -D_GLIBCXX_USE_NANOSLEEP -I$(PATH_LUA) -DLUA_ADAPTER_DEBUG 

$(OUT)/libLua.so: $(LUA)
	g++ -shared $(LUA) -o $(OUT)/libLua.so $(flags_mac)

app_mac.o: app.cpp
	g++ -c app.cpp -o app_mac.o $(flags_mac)

mac: app.o  $(OUT)/libLua.so
	g++ -o $(OUT)/app app_mac.o $(OUT)/libLua.so $(flags_mac) -lSDL3main -lSDL3 -Wl,-rpath,/usr/local/lib

