function(generate_version ini_path output_path)
    get_filename_component(PYTHON_SCRIPT_PATH ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../version/gen_version.py ABSOLUTE)
    add_custom_command(
            OUTPUT ${output_path}
            COMMAND python ${PYTHON_SCRIPT_PATH} -f ${ini_path} -i ${output_path}
            DEPEND ${ini_path}
    )
endfunction()
