# copy multiple files
function(copy_files_post targetProject targetDirectory)
	foreach(f ${ARGN})
		add_custom_command(
			TARGET ${targetProject}
			POST_BUILD
				#COMMAND echo "${f} => ${targetDirectory}"
				COMMAND ${CMAKE_COMMAND} -E copy \"${f}\" \"${targetDirectory}\"
		)
	endforeach()
endfunction(copy_files_post)
