CC = gcc
WARNINGS = -Wall -Wformat -Wno-deprecated-declarations
CFLAGS =  -I./include -O3 -g
LDLIBS = -lSDL2 -lm -lpthread -llua -framework OpenGL -lGLEW
version = 0.0.4

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
	@echo "Linking      \x1B[93m$@\x1B[0m \x1B[2m<- $(OBJDIR)/*\x1B[0m "
	@$(CC) $(WARNINGS) $(LDLIBS) -o $@ $(foreach i,$^,$(i) )
	@echo "\x1B[93mCompiled     v$(version)\x1B[0m"

cleanall:
	rm $(exe)
	rm -rf $(OBJDIR)
clean:
	rm $(exe)
	rm $(OBJDIR)/*.o
	rm $(OBJDIR)/render/*.o
	# rm -rf luajit/**/*.o


# luajit/src/libluajit.a:
# 	$(MAKE) -C luajit
