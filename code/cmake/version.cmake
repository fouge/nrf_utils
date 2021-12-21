get_filename_component(VERSION_PY_PATH ${CMAKE_CURRENT_LIST_DIR}/../version/gen_version.py ABSOLUTE)

function(version_deps ini_path output_path)
    add_custom_command(
            OUTPUT ${output_path}
            COMMAND python ${VERSION_PY_PATH} -f ${ini_path} -i ${output_path}
            DEPEND ${ini_path}
    )
endfunction()
