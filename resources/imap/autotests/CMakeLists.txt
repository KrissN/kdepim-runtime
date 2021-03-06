#if the kimap project wasn't built with tests then we can't build anything in here
if(NOT TARGET KF5::kimaptest)
  message(STATUS "Warning: Unable to build the imap autotests since the kimap project did not build the kimaptest library. You need to rebuild kimap with the -DBUILD_TESTING=on option")
  return()
endif()

kde_enable_exceptions()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include(ECMMarkAsTest)

find_package(Qt5Test CONFIG REQUIRED)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

MACRO(IMAP_RESOURCE_UNIT_TESTS)
  FOREACH(_testname ${ARGN})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/..)
    add_executable( ${_testname} ${_testname}.cpp dummypasswordrequester.cpp dummyresourcestate.cpp imaptestbase.cpp )
    add_test(NAME ${_testname} COMMAND ${_testname} )
    ecm_mark_as_test(imap-${_testname})

    target_link_libraries(${_testname}
      KF5::IMAP KF5::kimaptest Qt5::Gui Qt5::Core Qt5::Test imapresource akonadi-singlefileresource)
    add_definitions(-DTEST_DATA="\\"${CMAKE_CURRENT_SOURCE_DIR}\\"")
  ENDFOREACH(_testname)
ENDMACRO(IMAP_RESOURCE_UNIT_TESTS)

IMAP_RESOURCE_UNIT_TESTS(
  testresourcetask
  testsessionpool

  testaddcollectiontask
  testadditemtask
  testchangecollectiontask
  testchangeitemtask
  testexpungecollectiontask
  testmovecollectiontask
  testmoveitemstask
  testremovecollectionrecursivetask
  testretrievecollectionmetadatatask
  testretrievecollectionstask
  testretrieveitemtask
  testretrieveitemstask
)
