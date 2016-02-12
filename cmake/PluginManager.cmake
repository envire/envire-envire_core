
# Installs a plugin info .xml file to INSTALL_FOLDER/share/plugin_manager/
function(install_plugin_info TARGET_NAME)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.xml)
        file(GLOB plugin_files "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.xml")
        install(FILES ${plugin_files}
                DESTINATION share/plugin_manager)
    else()
        message("plugin export: ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.xml is not available for export")
    endif()
endfunction()