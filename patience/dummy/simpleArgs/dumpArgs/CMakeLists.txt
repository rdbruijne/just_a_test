set(SOURCES
	dumpArgs.cpp

	# test config
	test/test.yml
)

assign_source_group(${SOURCES})
add_executable(dumpArgs ${SOURCES})
