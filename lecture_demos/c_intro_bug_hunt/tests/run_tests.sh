#!/bin/sh
TIMEOUT_SECONDS=5

all_tests=$@
test_count=$#
fail_count=0

for test_file in $all_tests
do
	echo "\033[1;39m===== ${test_file} =====\033[0m"
	rm -f testfs # Tidy up from previous tests
	timeout ${TIMEOUT_SECONDS} ${test_file}
	rc=$?
	if [ ${rc} -eq 0 ]
	then
		echo "\033[1;32mPASS\033[0m"
	elif [ ${rc} -eq 124 ]
	then
		echo "\033[1;31mFAIL (${TIMEOUT_SECONDS} second timeout)\033[0m"
		fail_count=$((fail_count + 1))
	else
		echo "\033[1;31mFAIL (rc = ${rc})\033[0m"
		fail_count=$((fail_count + 1))
	fi
done

echo "\n${fail_count} out of ${test_count} tests failed."
