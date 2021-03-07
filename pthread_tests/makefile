# List of test sources. Whenever you add a new test.c file, add it to a new
# line in this list.
SOURCES = \
	  test_pthread_create_launches_start_routine.c \
	  # End of SOURCES list

# Search for c files in the src directory
vpath %.c src

# Build to different locations depending on whether building for -pthread
# or for the project. This means you do *not* need to make clean when switching
# between the two options.
ifdef PROJECT_DIR
build_prefix=build_project
else
build_prefix=build_pthread
endif

# Generate our expected .o files and test files from the .c files
objs=$(addprefix $(build_prefix)/, $(SOURCES:.c=.o))
tests=$(addprefix $(build_prefix)/, $(SOURCES:.c=))

# 0 is an exit code that we are particularly likely to accidentally encounter
# in test failure cases Pick another arbitrary value just to reduce chance of
# accidentally encountering the success code
CFLAGS+=-DSUCCESS_EXIT_CODE=10

check: checkprogs
	SUCCESS_EXIT_CODE=10 ./scripts/run_tests.sh $(tests)

checkprogs: $(tests)

# If PROJECT_DIR has been specified, try to do a recursive make on that project.
# Otherwise, only build our test dependencies.
ifdef PROJECT_DIR
$(info Testing against $(PROJECT_DIR))

$(PROJECT_DIR)/threads.o:
	$(MAKE) -C "$(PROJECT_DIR)"

$(tests): % : %.o $(PROJECT_DIR)/threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@
else
$(info No PROJECT_DIR specified. Using -pthread)
$(info Set PROJECT_DIR to a directory containing the project you want to test.)

$(tests): CFLAGS+=-pthread
$(tests): LDFLAGS+=-pthread
$(tests): % : %.o
endif

$(objs): $(build_prefix)/%.o : %.c | $(build_prefix)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^ -o $@

$(build_prefix):
	mkdir "$@"

.PHONY: clean
clean:
	rm -f $(objs) $(tests)
	rmdir $(build_prefix)
