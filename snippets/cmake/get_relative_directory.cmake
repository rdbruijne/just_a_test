# get directory for file
function(get_relative_directory filePath relativeDir)
	if (IS_ABSOLUTE "${filePath}")
		file(RELATIVE_PATH relSourcePath "${CMAKE_CURRENT_SOURCE_DIR}" "${filePath}")
	else()
		set(relSourcePath "${filePath}")
	endif()
	get_filename_component(pathForFile "${relSourcePath}" DIRECTORY)
	string(REPLACE "/" "\\" relDir "${pathForFile}")
	set("${relativeDir}" "${relDir}" PARENT_SCOPE)
endfunction(get_relative_directory)