CC = gcc
WARNINGS = -Wall -Wformat -Wno-deprecated-declarations
CFLAGS = -I./include -O3 -g
LDLIBS = -lm -lpthread
version = 0.2.0


UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	CFLAGS += -I/usr/include/lua5.3 `sdl2-config --libs --cflags`
	LDLIBS += -llua5.3 -lSDL2 -lglut -lGL -lGLU -lGLEW
endif
ifeq ($(UNAME_S),Darwin)
	LDLIBS += -framework OpenGL -llua -lGLEW -lSDL2
endif


objs = $(srcs:.c=.o)
exe = game

SRCDIR = src
OBJDIR = build

STRUCTURE := $(shell find $(SRCDIR) -type d)
CODEFILES := $(addsuffix /*,$(STRUCTURE))
CODEFILES := $(wildcard $(CODEFILES))

SRCFILES := $(filter %.c,$(CODEFILES))
OBJFILES := $(subst $(SRCDIR),$(OBJDIR),$(SRCFILES:%.c=%.o))

.PHONY: all clean

all: $(OBJDIR) $(exe)

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/%.o: $(addprefix $(SRCDIR)/,%.c)
	@printf "Compiling    $@ <- $<\n"
	@mkdir -p $(dir $@)
	@$(CC) $(WARNINGS) $(CFLAGS) -c $< -o $@

$(exe):  $(OBJFILES)
	@printf "Linking      $@ <- $(OBJDIR)/*\n"
	@$(CC) $(WARNINGS) -o $@ $(foreach i,$^,$(i) ) $(LDLIBS) 
	@printf "Compiled     v$(version) for $(UNAME_S)\\n"

clean:
	@rm -rf $(exe)
	@rm -rf $(OBJDIR)/*.o
	@rm -rf $(OBJDIR)/render/*.o


# luajit/src/libluajit.a:
# 	$(MAKE) -C luajit