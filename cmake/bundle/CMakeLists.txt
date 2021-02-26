# This file is the last add_subdirectory() included. The install directives will
# run after all other targets are installed.

# Workaround wrong tool detection on Windows
if(WIN32 AND NOT MSVC)
  set(GP_TOOL "objdump")
endif()

install(CODE "\
  set(MAIN_APP \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/simplify${CMAKE_EXECUTABLE_SUFFIX}\")

  include(BundleUtilities)
  set(gp_tool \"${GP_TOOL}\")
  fixup_bundle(\"\${MAIN_APP}\" \"\" \"${LIBS_PATH}\")

" COMPONENT app
)
