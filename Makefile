CC = gcc
WARNINGS = -Wall -Wformat -Wno-deprecated-declarations
CFLAGS =  -I./include -O3 -g
LDLIBS = -lSDL2 -lm -lpthread -llua -lGLEW
version = 0.0.4


UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	LDLIBS += -D LINUX
endif
ifeq ($(UNAME_S),Darwin)
	LDLIBS += -framework OpenGL
endif

$(shell uname -s)
@echo   "Buildingggggggggggg for $(UNAME_S)"

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
	@echo "Compiling    \x1B[92m$@\x1B[0m \x1B[2m<- $<\x1B[0m"
	@mkdir -p $(dir $@)
	@$(CC) $(WARNINGS) $(CFLAGS) -c $< -o $@

$(exe):  $(OBJFILES)
	@echo $(OS)
	@echo "Linking      \x1B[93m$@\x1B[0m \x1B[2m<- $(OBJDIR)/*\x1B[0m "
	@$(CC) $(WARNINGS) $(LDLIBS) -o $@ $(foreach i,$^,$(i) )
	@echo "\x1B[93mCompiled     v$(version)\x1B[0m"

clean:
	@rm -rf $(exe)
	@rm -rf $(OBJDIR)/*.o
	@rm -rf $(OBJDIR)/render/*.o


# luajit/src/libluajit.a:
# 	$(MAKE) -C luajit
