#-------------------------------------------------
#
# Project created by QtCreator 2020-03-14T23:16:43
#
#-------------------------------------------------

QT += core gui \
      network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ZARD_L8000
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

DEFINES += \
        QT_DEPRECATED_WARNINGS \
        WPCAP \
        HAVE_REMOTE

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
#CONFIG += conFsole

INCLUDEPATH += \
    $$PWD/common \
    $$PWD/core_thread \
    $$PWD/protocol/WpdPack/Include \
    $$PWD/protocol \
    $$PWD/track_displayer\
    $$PWD/ui

# WinPcap
win32: LIBS += -L$$PWD/protocol/WpdPack/Lib/x64/ -lPacket -lwpcap
INCLUDEPATH += $$PWD/protocol/WpdPack/Lib/x64
DEPENDPATH += $$PWD/protocol/WpdPack/Lib/x64
win32: LIBS += -liphlpapi

SOURCES += \
    common/global_functions.cpp \
    common/global_settings.cpp \
    core_thread/radar_data_processing_module.cpp \
    core_thread/communicate_with_fpga_thread.cpp \
    core_thread/control_and_communicate_with_controller_thread.cpp \
    core_thread/radar_data_processing_thread.cpp \
    protocol/get_interface_friendly_name.cpp \
    protocol/packet_cap.cpp \
    protocol/protocol_with_fpga.cpp \
    track_displayer/my_graphics_view.cpp \
    track_displayer/my_track_info_item.cpp \
    track_displayer/original_point_and_track_item.cpp \
    track_displayer/track_displayer_scene.cpp\
    ui/adapters_select_dialog.cpp \
    ui/calculate_range_and_doppler_unit_dialog.cpp \
    ui/convert_lon_lat_to_range_azim_dialog.cpp \
    ui/main_window.cpp \
    ui/radar_hardware_settings_dialog.cpp \
    ui/radar_software_settings_dialog.cpp \
    ui/radar_data_processing_settings_dialog.cpp \
    main.cpp

HEADERS += \
    common/global_definitions.h \
    common/global_functions.h \
    common/global_settings.h \
    core_thread/radar_data_processing_module.h \
    core_thread/radar_data_processing_thread.h \
    core_thread/communicate_with_fpga_thread.h \
    core_thread/control_and_communicate_with_controller_thread.h \
    protocol/get_interface_friendly_name.h \
    protocol/packet_cap.h \
    protocol/protocol_with_fpga.h \
    protocol/protocol_with_controller.h \
    track_displayer/my_graphics_view.h \
    track_displayer/my_track_info_item.h \
    track_displayer/original_point_and_track_item.h \
    track_displayer/track_displayer_scene.h \
    ui/adapters_select_dialog.h \
    ui/calculate_range_and_doppler_unit_dialog.h \
    ui/convert_lon_lat_to_range_azim_dialog.h \
    ui/main_window.h \
    ui/radar_hardware_settings_dialog.h \
    ui/radar_software_settings_dialog.h \
    ui/radar_data_processing_settings_dialog.h

FORMS += \
    ui/adapters_select_dialog.ui \
    ui/calculate_range_and_doppler_unit_dialog.ui \
    ui/convert_lon_lat_to_range_azim_dialog.ui \
    ui/main_window.ui \ 
    ui/radar_hardware_settings_dialog.ui \
    ui/radar_software_settings_dialog.ui \
    ui/radar_data_processing_settings_dialog.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TRANSLATIONS += translations_to_EN.ts
