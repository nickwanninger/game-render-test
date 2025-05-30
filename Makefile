# CC = gcc
WARNINGS = -Wall -Wformat -Wno-deprecated-declarations -Wno-string-plus-int
CFLAGS =  -I./include -O3 -g
LDLIBS = -lSDL2 -lm -lpthread -llua -framework OpenGL
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
	@echo "CC   $@ <- $<"
	@mkdir -p $(dir $@)
	@$(CC) $(WARNINGS) $(CFLAGS) -c $< -o $@

$(exe):  $(OBJFILES)
	@echo "LD   $@ <- $(OBJDIR)/*"
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
