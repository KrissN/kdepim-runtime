
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libmaildir
    ${kdepim-runtime_SOURCE_DIR}
)


if (BUILD_TESTING)
   add_subdirectory( autotests )
endif()
add_subdirectory( wizard )

# maildir access library
add_subdirectory(libmaildir)


add_definitions(-DTRANSLATION_DOMAIN=\"akonadi_maildir_resource\")

########### next target ###############
set( maildirresource_SRCS
  main.cpp
  maildirresource.cpp
  configdialog.cpp
  retrieveitemsjob.cpp
)
ecm_qt_declare_logging_category(maildirresource_SRCS HEADER maildirresource_debug.h IDENTIFIER MAILDIRRESOURCE_LOG CATEGORY_NAME org.kde.pim.maildirresource)

ki18n_wrap_ui(maildirresource_SRCS settings.ui)

kconfig_add_kcfg_files(maildirresource_SRCS settings.kcfgc)

kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/maildirresource.kcfg org.kde.Akonadi.Maildir.Settings)

qt5_add_dbus_adaptor(maildirresource_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.Maildir.Settings.xml settings.h Akonadi_Maildir_Resource::MaildirSettings maildirsettingsadaptor MaildirSettingsAdaptor
)

add_executable(akonadi_maildir_resource ${maildirresource_SRCS})

target_link_libraries(akonadi_maildir_resource
    maildir
    folderarchivesettings
    KF5::AkonadiCore
    KF5::AkonadiMime
    KF5::KIOWidgets
    KF5::Mime
    KF5::AkonadiAgentBase
    KF5::DBusAddons
    KF5::WindowSystem
    KF5::I18n
    KF5::ConfigWidgets
)

install(TARGETS akonadi_maildir_resource ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.Maildir.Settings.xml
        DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

install( FILES maildirresource.desktop DESTINATION "${KDE_INSTALL_DATAROOTDIR}/akonadi/agents" )

