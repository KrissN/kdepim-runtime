
add_definitions(-DTRANSLATION_DOMAIN=\"akonadi_openxchange_resource\")

set( openxchangeresource_SRCS
  oxa/connectiontestjob.cpp
  oxa/contactutils.cpp
  oxa/davmanager.cpp
  oxa/davutils.cpp
  oxa/folder.cpp
  oxa/foldercreatejob.cpp
  oxa/folderdeletejob.cpp
  oxa/foldermodifyjob.cpp
  oxa/foldermovejob.cpp
  oxa/folderrequestjob.cpp
  oxa/foldersrequestdeltajob.cpp
  oxa/foldersrequestjob.cpp
  oxa/folderutils.cpp
  oxa/incidenceutils.cpp
  oxa/object.cpp
  oxa/objectcreatejob.cpp
  oxa/objectdeletejob.cpp
  oxa/objectmodifyjob.cpp
  oxa/objectmovejob.cpp
  oxa/objectrequestjob.cpp
  oxa/objectsrequestdeltajob.cpp
  oxa/objectsrequestjob.cpp
  oxa/objectutils.cpp
  oxa/oxutils.cpp
  oxa/oxerrors.cpp
  oxa/updateusersjob.cpp
  oxa/user.cpp
  oxa/users.cpp
  oxa/useridrequestjob.cpp
  oxa/usersrequestjob.cpp

  configdialog.cpp
  openxchangeresource.cpp
)

ki18n_wrap_ui( openxchangeresource_SRCS configdialog.ui )

install( FILES openxchangeresource.desktop DESTINATION "${KDE_INSTALL_DATAROOTDIR}/akonadi/agents" )

kconfig_add_kcfg_files(openxchangeresource_SRCS settings.kcfgc)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/openxchangeresource.kcfg org.kde.Akonadi.OpenXchange.Settings)
qt5_add_dbus_adaptor(openxchangeresource_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.OpenXchange.Settings.xml settings.h Settings
)

add_executable(akonadi_openxchange_resource ${openxchangeresource_SRCS})

if( APPLE )
  set_target_properties(akonadi_openxchange_resource PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../Info.plist.template)
  set_target_properties(akonadi_openxchange_resource PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.Akonadi.OpenXchange")
  set_target_properties(akonadi_openxchange_resource PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KDE Akonadi OpenXchange Resource")
endif ()


target_link_libraries(akonadi_openxchange_resource
    KF5::AkonadiAgentBase
    KF5::AkonadiCore
    KF5::Contacts
    KF5::KIOCore
    KF5::CalendarCore 
    KF5::AkonadiContact
    KF5::I18n
    KF5::Completion
    KF5::WindowSystem
)

install(TARGETS akonadi_openxchange_resource ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(icons)
