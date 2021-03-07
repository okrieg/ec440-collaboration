#!/bin/bash
# This script executes all tests in the script's arguments. It wraps each test
# case in a timeout, and outputs a colorized pass/fail message based on whether
# the test's return code matches the SUCCESS_EXIT_CODE environment variable.
TIMEOUT_SECONDS=5

if [ -z "${SUCCESS_EXIT_CODE}" ]
then
	echo 1>&2 'ERROR: SUCCESS_EXIT_CODE must be set to the expected exit '
	echo 1>&2 '       code for successful tests'
	exit 1
fi

all_tests=( "$@" )
test_count=$#
fail_count=0

for test_file in "${all_tests[@]}"
do
	printf "\033[1;39m===== %s =====\033[0m\n" "${test_file}"
	rm -f testfs # Tidy up from previous tests
	timeout ${TIMEOUT_SECONDS} "./${test_file}"
	rc=$?
	if [ ${rc} -eq "$SUCCESS_EXIT_CODE" ]
	then
		printf "\033[1;32mPASS\033[0m\n"
	elif [ ${rc} -eq 124 ]
	then
		printf "\033[1;31mFAIL (%d second timeout)\033[0m\n" "${TIMEOUT_SECONDS}"
		fail_count=$((fail_count + 1))
	else
		printf "\033[1;31mFAIL (rc = %d)\033[0m\n" "${rc}"
		fail_count=$((fail_count + 1))
	fi
done

printf "\n%d out of %d tests failed.\n" "${fail_count}" "${test_count}"
