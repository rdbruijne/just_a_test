set(SOURCES
	failingTest.cpp

	# test config
	test/test.yml
)

assign_source_group(${SOURCES})
add_executable(failingTest ${SOURCES})
