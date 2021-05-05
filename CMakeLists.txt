cmake_minimum_required(VERSION 3.5)

project(extfs LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(extfs
        main.c
        type.h
        util.c
        util.h
        cd_ls_pwd.h
        cd_ls_pwd.c
        globals.h
        globals.c
        mkdir_create.h
        mkdir_create.c
	rmdir.c
	rmdir.h
        link_unlink.c 
        link_unlink.h
        stat.c
        stat.h
        miscl.h
        miscl.c
        )
set_target_properties(extfs PROPERTIES COMPILE_OPTIONS "-m32" LINK_FLAGS "-m32")
target_compile_options(extfs PRIVATE -Wall -W -pedantic -pipe -Wno-unused-parameter -ggdb)
