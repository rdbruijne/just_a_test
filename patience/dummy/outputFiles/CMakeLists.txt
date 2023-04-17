set(SOURCES
	outputFiles.cpp
	
	# test config
	test/test.yml
)

assign_source_group(${SOURCES})
add_executable(outputFiles ${SOURCES})
