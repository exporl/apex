potentialdirs = $$files("$${_PRO_FILE_PWD_}/*")
for(potentialdir, potentialdirs) {
    subdirheaders += $$files("$${potentialdir}/*.h")
    subdirsources += $$files("$${potentialdir}/*.cpp") $$files("$${potentialdir}/*.c")
}
HEADERS *= $$replace(subdirheaders, "(^| )$${_PRO_FILE_PWD_}/", "\\1")
SOURCES *= $$replace(subdirsources, "(^| )$${_PRO_FILE_PWD_}/", "\\1")
