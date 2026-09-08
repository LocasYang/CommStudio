cmake_minimum_required(VERSION 3.21)

project(CommStudio VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    QuickControls2
    Network
    SerialPort
    SerialBus
    Bluetooth
)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(CommStudio
    src/main.cpp
    src/windowchrome.cpp
    src/windowchrome.h

    src/core/trafficmodel.cpp
    src/core/trafficmodel.h
    src/core/checksumengine.cpp
    src/core/checksumengine.h
    src/core/hexcodec.cpp
    src/core/hexcodec.h
    src/core/modbuscodec.cpp
    src/core/modbuscodec.h

    src/managers/studio.cpp
    src/managers/studio.h
    src/managers/serialmanager.cpp
    src/managers/serialmanager.h
    src/managers/networkmanager.cpp
    src/managers/networkmanager.h
    src/managers/bluetoothmanager.cpp
    src/managers/bluetoothmanager.h
    src/managers/modbusmanager.cpp
    src/managers/modbusmanager.h
    src/managers/toolboxmanager.cpp
    src/managers/toolboxmanager.h
)

set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(CommStudio
    URI CommStudio
    VERSION 1.0
    RESOURCES
        qml/assets/commstudio-mark.svg
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/components/AppButton.qml
        qml/components/AppCard.qml
        qml/components/AppComboBox.qml
        qml/components/NavItem.qml
        qml/components/StatusPill.qml
        qml/components/DataUnavailableState.qml
        qml/components/AppTextField.qml
        qml/components/AppCheckBox.qml
        qml/components/FormRow.qml
        qml/components/TerminalView.qml
        qml/components/SendPanel.qml
        qml/dialogs/ConfirmDialog.qml
        qml/pages/SerialPage.qml
        qml/pages/ModbusPage.qml
        qml/pages/BluetoothPage.qml
        qml/pages/NetworkPage.qml
        qml/pages/ToolboxPage.qml
        qml/pages/LogPage.qml
        qml/pages/SettingsPage.qml
)

target_include_directories(CommStudio PRIVATE src)

target_link_libraries(CommStudio PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::SerialPort
    Qt6::SerialBus
    Qt6::Bluetooth
)

# GUI 子系统：启动不弹控制台窗口。
set_target_properties(CommStudio PROPERTIES WIN32_EXECUTABLE TRUE)

if (WIN32)
    target_link_libraries(CommStudio PRIVATE dwmapi)
endif()

if (BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    add_executable(CommStudioCoreTest
        tests/comstudiotests.cpp
        src/core/trafficmodel.cpp
        src/core/checksumengine.cpp
        src/core/hexcodec.cpp
        src/core/modbuscodec.cpp
    )
    target_include_directories(CommStudioCoreTest PRIVATE src)
    set_target_properties(CommStudioCoreTest PROPERTIES WIN32_EXECUTABLE FALSE)
    target_link_libraries(CommStudioCoreTest PRIVATE Qt6::Core Qt6::Gui Qt6::Test)
    add_test(NAME CommStudioCoreTest COMMAND CommStudioCoreTest)
    set_tests_properties(CommStudioCoreTest PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>")
endif()

set(COMM_STUDIO_DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist/CommStudio")
add_custom_command(TARGET CommStudio POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${COMM_STUDIO_DIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:CommStudio>"
            "${COMM_STUDIO_DIST_DIR}/$<TARGET_FILE_NAME:CommStudio>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
            "${COMM_STUDIO_DIST_DIR}/README.md"
    COMMENT "Synchronizing CommStudio.exe to dist"
    VERBATIM
)

install(TARGETS CommStudio
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
