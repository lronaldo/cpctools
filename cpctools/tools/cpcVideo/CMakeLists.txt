Find_Package ( SDL REQUIRED )

if ( NOT SDL_FOUND )
   message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )

include_directories(${SDL_INCLUDE_DIR} libVideoCPC ../../../Reloaded/src/core ../../../Reloaded/src)

add_executable (cpcVideo
	cpcFileViewer.cpp
	CCPCScreenView.cpp
	CCPCFontView.cpp
	CCPCDataView.cpp
	CCPCWindowView.cpp
	ScriptRunner.cpp

	libVideoCPC/CCPCFileManager.cpp
	libVideoCPC/CCPCVideo.cpp
	libVideoCPC/ptcVideo.cpp

	../../../Reloaded/src/core/crtc.cpp
	../../../Reloaded/src/core/gatearray.cpp
	../../../Reloaded/src/core/glfuncs.cpp
	../../../Reloaded/src/misc/log.cpp
	../../../Reloaded/src/core/render.cpp
	../../../Reloaded/src/core/renderData.cpp
	../../../Reloaded/src/misc/synchro.cpp
	../../../Reloaded/src/core/vdu.cpp
	../../../Reloaded/src/core/video.cpp
)

set(CXXFLAGS -Wall -O3)

if ( BEOS )
	target_link_libraries(cpcVideo be game)
endif ( BEOS )

set_target_properties(cpcVideo PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(cpcVideo PROPERTIES COMPILE_DEFINITIONS NO_Z80)
set_target_properties(cpcVideo PROPERTIES COMPILE_FLAGS "-Wall -Wno-multichar -O3")

target_link_libraries(cpcVideo ${SDL_LIBRARY} tools TinyPTC lua)
