#
#  Copyright (C) 2015-2017 Krzysztof Nowicki <krissn@op.pl>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Library General Public
#  License as published by the Free Software Foundation; either
#  version 2 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Library General Public License for more details.
#
#  You should have received a copy of the GNU Library General Public License
#  along with this library; see the file COPYING.LIB.  If not, write to
#  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#  Boston, MA 02110-1301, USA.

kde_enable_exceptions()

add_library(uttesthelpers STATIC faketransferjob.cpp)
target_link_libraries(uttesthelpers Qt5::Core KF5::KIOCore)

macro(akonadi_ews_add_ut utname)
  add_executable(${utname} ${utname}.cpp)
  target_link_libraries(${utname} Qt5::Test uttesthelpers ewsclient)
  add_test(NAME ${utname} COMMAND ${utname})
endmacro(akonadi_ews_add_ut utname)

akonadi_ews_add_ut(ewsmoveitemrequest_ut)
akonadi_ews_add_ut(ewsdeleteitemrequest_ut)
akonadi_ews_add_ut(ewsgetitemrequest_ut)
akonadi_ews_add_ut(ewsunsubscriberequest_ut)
akonadi_ews_add_ut(ewsattachment_ut)

set(ewssettings_ut_SRCS ewssettings_ut.cpp)
qt5_wrap_cpp(ewssettings_ut_SRCS ewssettings_ut_mock.h)
add_executable(ewssettings_ut
  ${ewssettings_ut_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ewssettings.cpp)
target_link_libraries(ewssettings_ut
  Qt5::Core
  Qt5::Test
  KF5::WidgetsAddons
  KF5::I18n
  KF5::ConfigCore
  KF5::ConfigGui
  KF5::CoreAddons
  KF5::Wallet)
target_compile_definitions(ewssettings_ut PUBLIC -DEWSSETTINGS_UNITTEST)
add_test(NAME ewssettings_ut COMMAND ewssettings_ut)


