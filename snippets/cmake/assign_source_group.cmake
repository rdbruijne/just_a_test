# filters for source files
function(assign_source_group)
    foreach(f ${ARGN})
		get_relative_directory("${f}" dir)
		source_group("${dir}" FILES "${f}")
    endforeach()
endfunction(assign_source_group)
