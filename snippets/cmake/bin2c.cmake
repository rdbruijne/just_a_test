# convert a binary file to a C file
function(bin2c srcFile dstFile)
	string(REGEX MATCH "([^/]+)$" filename ${srcFile})
    string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
    string(TOLOWER ${filename} filename)
    file(READ ${srcFile} filedata HEX)
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
	
	foreach(i RANGE 15)
		string(CONCAT foo ${foo} ".....")
	endforeach()
	string(REGEX REPLACE "(${foo})" "\\1\n" filedata ${filedata})
	
    file(WRITE ${dstFile} "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n")
	file(APPEND ${dstFile} "static uint8_t ${filename}[] = {\n${filedata}\n};\n")
	file(APPEND ${dstFile} "\n#ifdef __cplusplus\n}\n#endif\n")
endfunction(bin2c)
