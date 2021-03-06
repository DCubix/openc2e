cmake_minimum_required(VERSION 3.1.0)
PROJECT(OPENC2E CXX C)

cmake_policy(SET CMP0071 NEW)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

SET(SRC "${OPENC2E_SOURCE_DIR}")
SET(BIN "${OPENC2E_BINARY_DIR}")
SET(GEN "${BIN}/generated")
EXECUTE_PROCESS(COMMAND mkdir -p "${GEN}")

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS 1)
SET(CMAKE_MODULE_PATH "${SRC}/cmake")

SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "The type of build to perform. Valid values are: RelWithDebInfo (default), Debug, Release, MinSizeRel")
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "The type of build to perform. Valid values are: RelWithDebInfo (default), Debug, Release, MinSizeRel" FORCE)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

SET(OPENC2E_USE_SERIALIZATION "FALSE" CACHE BOOL "Build the experimental serialization code")
MARK_AS_ADVANCED(FORCE OPENC2E_USE_SERIALIZATION)

IF(OPENC2E_USE_SERIALIZATION)
	SET(SER_SRCS src/caos/caosVM_ser_real.cpp)
ELSE (OPENC2E_USE_SERIALIZATION)
	SET(SER_SRCS src/caos/caosVM_ser_stub.cpp)
ENDIF (OPENC2E_USE_SERIALIZATION)

SET(OPENC2E_PROFILE_ALLOCATION "FALSE" CACHE BOOL "Collect allocation profile stats for DBG: SIZO")
MARK_AS_ADVANCED(FORCE OPENC2E_PROFILE_ALLOCATION)

IF (OPENC2E_PROFILE_ALLOCATION)
	ADD_DEFINITIONS("-DPROFILE_ALLOCATION_COUNT -DPROFILE_ALLOCATION_THREAD_SAFE")
ENDIF (OPENC2E_PROFILE_ALLOCATION)

SET(OPENC2E_CORE
	src/Agent.cpp
	src/AgentHelpers.cpp
	src/AgentRef.cpp
	src/alloc_count.cpp
	src/creatures/attFile.cpp
	src/Backend.cpp
	src/creatures/Biochemistry.cpp
	src/Blackboard.cpp
	src/images/blkImage.cpp
	src/images/bmpImage.cpp
	src/Bubble.cpp
	src/bytecode.cpp
	src/images/c16Image.cpp
	src/creatures/c2eBrain.cpp
	src/CallButton.cpp
	src/Camera.cpp
	src/caosScript.cpp
	src/caosVar.cpp
	src/caos/caosVM_agent.cpp
	src/caos/caosVM_camera.cpp
	src/caos/caosVM_compound.cpp
	src/caos/caosVM_core.cpp
	src/caos/caosVM_creatures.cpp
	src/caos/caosVM_debug.cpp
	src/caos/caosVM_files.cpp
	src/caos/caosVM_flow.cpp
	src/caos/caosVM_genetics.cpp
	src/caos/caosVM_history.cpp
	src/caos/caosVM_input.cpp
	src/caos/caosVM_map.cpp
	src/caos/caosVM_motion.cpp
	src/caos/caosVM_net.cpp
	src/caosVM.cpp
	src/caos/caosVM_ports.cpp
	src/caos/caosVM_resources.cpp
	src/caos/caosVM_scripts.cpp
	src/caos/caosVM_sounds.cpp
	src/caos/caosVM_time.cpp
	src/caos/caosVM_variables.cpp
	src/caos/caosVM_vectors.cpp
	src/caos/caosVM_vehicles.cpp
	src/caos/caosVM_world.cpp
	${GEN}/cataloglexer.cpp
	src/Catalogue.cpp
	${GEN}/catalogue.tab.cpp
	${GEN}/cmddata.cpp
	src/cobFile.cpp
	src/CompoundAgent.cpp
	src/creatures/CompoundCreature.cpp
	src/CompoundPart.cpp
	src/creatures/Creature.cpp
	src/creatures/CreatureAgent.cpp
	src/creatures/CreatureAI.cpp
	src/creaturesImage.cpp
	src/dialect.cpp
	src/Engine.cpp
	src/exceptions.cpp
	src/fileSwapper.cpp
	src/creatures/genomeFile.cpp
	src/historyManager.cpp
	src/imageManager.cpp
	${GEN}/mnglexer.cpp
	${GEN}/caoslexer.cpp
	src/Lift.cpp
	src/Map.cpp
	src/MetaRoom.cpp
	src/mmapifstream.cpp
	src/music/mngfile.cpp
	${GEN}/mngparser.tab.cpp
	src/MusicManager.cpp
	src/creatures/oldBrain.cpp
	src/PathResolver.cpp
	src/peFile.cpp
	src/physics.cpp
	src/PointerAgent.cpp
	src/Port.cpp
	src/pray.cpp
	src/prayManager.cpp
	src/renderable.cpp
	src/Room.cpp
	src/Scriptorium.cpp
	src/SFCFile.cpp
	src/SimpleAgent.cpp
	src/creatures/SkeletalCreature.cpp
	src/images/sprImage.cpp
	src/streamutils.cpp
	src/Vehicle.cpp
	src/VoiceData.cpp
	src/World.cpp
	src/main.cpp
	src/util.cpp
)
SET(OPENC2E_SDL
	src/backends/SDLBackend.cpp
)

SET(OPENC2E_QT
	src/backends/qtgui/qtopenc2e.cpp
	src/backends/qtgui/openc2eview.cpp
	src/backends/qtgui/AgentInjector.cpp
	src/backends/qtgui/BrainViewer.cpp
	src/backends/qtgui/imagepreview.cpp
	src/backends/qtgui/c1cobfile.cpp
	src/backends/qtgui/QtBackend.cpp
	src/backends/qtgui/GraphWidget.cpp
	src/backends/qtgui/CreatureGrapher.cpp
	src/backends/qtgui/ChemicalSelector.cpp
	src/backends/qtgui/Hatchery.cpp
	src/tools/braininavat/brainview.cpp)

SET(OPENC2E_QT_MOC_HDRS
	src/backends/qtgui/qtopenc2e.h
	src/backends/qtgui/openc2eview.h
	src/backends/qtgui/AgentInjector.h
	src/backends/qtgui/BrainViewer.h
	src/backends/qtgui/imagepreview.h
	src/backends/qtgui/GraphWidget.h
	src/backends/qtgui/CreatureGrapher.h
	src/backends/qtgui/ChemicalSelector.h
	src/backends/qtgui/Hatchery.h
	src/tools/braininavat/brainview.h)

SET(OPENC2E_QT_UIS
	src/backends/qtgui/agents.ui)
# 

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(ZLIB REQUIRED)

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

# find_package(Qt4 REQUIRED)
# include(${QT_USE_FILE})
# add_definitions(${QT_DEFINITIONS})
# include_directories(${CMAKE_BINARY_DIR})

# SET(SAVED_CMAKE_CURRENT_BINARY_DIR CMAKE_CURRENT_BINARY_DIR)
# SET(CMAKE_CURRENT_BINARY_DIR ${GEN})
# QT4_WRAP_UI(OPENC2E_QT_UIS_H ${OPENC2E_QT_UIS})
# QT4_WRAP_CPP(OPENC2E_QT_MOC_SRCS ${OPENC2E_QT_MOC_HDRS})
# SET(CMAKE_CURRENT_BINARY_DIR SAVED_CMAKE_CURRENT_BINARY_DIR)
SET(QT_DEF "-DQT_SUPPORT")

find_package(Boost 1.46.0 COMPONENTS program_options filesystem thread regex serialization system REQUIRED)

# Forced SDL_mixer, simple == better.
SET(SDLMIXER_SRC "src/backends/SDLMixerBackend.cpp")
SET(SDLMIXER_DEF "-DSDLMIXER_SUPPORT")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pass flags to the compiler
ADD_DEFINITIONS("-W -D_REENTRANT -DYYERROR_VERBOSE ${QT_DEF} ${SDLMIXER_DEF}")

SET(FRONTEND_SRCS ${OPENC2E_SDL} ${OPENC2E_QT} ${OPENC2E_QT_UIS} ${OPENC2E_QT_MOC_HDRS})
# SET(FRONTEND_SRCS ${OPENC2E_SDL} ${OPENC2E_QT} ${OPENC2E_QT_UIS_H} ${OPENC2E_QT_MOC_SRCS})

ADD_EXECUTABLE(openc2e ${OPENC2E_CORE} ${FRONTEND_SRCS} ${SDLMIXER_SRC} ${SER_SRCS})

TARGET_LINK_LIBRARIES(
	openc2e
	PRIVATE
	SDL2
	SDL2_net
	SDL2_ttf
	SDL2_gfx
	SDL2_mixer
	${ZLIB_LIBRARIES}
	${FRONTEND_LIBS}
	ws2_32
	Boost::program_options
	Boost::filesystem
	Boost::system
	Boost::thread
	Boost::regex
	Boost::serialization
	# ${QT_LIBRARIES}
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets
)

INCLUDE_DIRECTORIES(BEFORE
	${GEN}
	${SRC}/src
	${ZLIB_INCLUDE_DIRS}
)

FILE(GLOB CAOSVM_FILES ${SRC}/src/caos/caosVM_*.cpp)

if(LINUX OR MINGW OR APPLE)
        if (LINUX OR APPLE)
            ADD_CUSTOM_COMMAND(
                    OUTPUT ${GEN}/commandinfo.yml
                    COMMAND perl parsedocs.pl ${CAOSVM_FILES} > ${GEN}/commandinfo.yml
                    DEPENDS parsedocs.pl ${CAOSVM_FILES}
                    WORKING_DIRECTORY ${SRC})

            ADD_CUSTOM_COMMAND(
                    OUTPUT ${BIN}/caosdocs.html
                    COMMAND perl docs/writehtml.pl ${GEN}/commandinfo.yml > ${BIN}/caosdocs.html
                    DEPENDS docs/writehtml.pl ${GEN}/commandinfo.yml
                    WORKING_DIRECTORY ${SRC})
            ADD_CUSTOM_COMMAND(
                    OUTPUT ${BIN}/docs.css
                    COMMAND cp docs/docs.css ${BIN}/docs.css
                    DEPENDS docs/docs.css
                    WORKING_DIRECTORY ${SRC})
            ADD_CUSTOM_COMMAND(
		OUTPUT ${BIN}/openc2e.6
		COMMAND pod2man --center="Openc2e Documentation" --section=6 ${SRC}/docs/openc2e.pod ${BIN}/openc2e.6
		DEPENDS docs/openc2e.pod
            )
            ADD_CUSTOM_TARGET(docs ALL DEPENDS ${BIN}/caosdocs.html ${BIN}/docs.css ${BIN}/openc2e.6)
        endif(LINUX OR APPLE)
	# it seems these ADD_CUSTOM_COMMANDS for flex and bison could be made
	# into a macro... e.g. FLEX(file1) FLEX(file2) etc...
	ADD_CUSTOM_COMMAND(
		OUTPUT ${GEN}/cataloglexer.cpp
		COMMAND re2c -o ${GEN}/cataloglexer.cpp ${SRC}/src/cataloglexer.re2c
		DEPENDS ${SRC}/src/cataloglexer.re2c
		WORKING_DIRECTORY ${SRC})

	ADD_CUSTOM_COMMAND(
		OUTPUT ${GEN}/catalogue.tab.cpp ${GEN}/catalogue.tab.hpp
		COMMAND bison -d --name-prefix=cata --file-prefix="${GEN}/catalogue" src/catalogue.ypp
		DEPENDS src/catalogue.ypp
		WORKING_DIRECTORY ${SRC})

	ADD_CUSTOM_COMMAND(
		OUTPUT ${GEN}/cmddata.cpp
		COMMAND perl writecmds.pl ${GEN}/commandinfo.yml > ${GEN}/cmddata.cpp
		DEPENDS ${GEN}/commandinfo.yml writecmds.pl
		WORKING_DIRECTORY ${SRC})

	ADD_CUSTOM_COMMAND(
		OUTPUT ${GEN}/caoslexer.cpp
		COMMAND re2c -o ${GEN}/caoslexer.cpp ${SRC}/src/caoslexer.re2c
		DEPENDS ${SRC}/src/caoslexer.re2c
		WORKING_DIRECTORY ${SRC})

	ADD_CUSTOM_COMMAND(
		OUTPUT ${GEN}/mnglexer.cpp
		COMMAND re2c -o ${GEN}/mnglexer.cpp ${SRC}/src/music/mnglexer.re2c
		DEPENDS ${SRC}/src/music/mnglexer.re2c
		WORKING_DIRECTORY ${SRC})

	ADD_CUSTOM_COMMAND(
		OUTPUT ${GEN}/mngparser.tab.cpp ${GEN}/mngparser.tab.hpp
		COMMAND bison -d --name-prefix=mng --file-prefix="${GEN}/mngparser" src/music/mngparser.ypp
		DEPENDS src/music/mngparser.ypp
		WORKING_DIRECTORY ${SRC})

        ADD_CUSTOM_TARGET(test DEPENDS openc2e COMMAND perl ${SRC}/runtests.pl ${SRC}/tests)

	ADD_CUSTOM_TARGET(generated DEPENDS
		${GEN}/catalogue.tab.cpp ${GEN}/catalogue.tab.hpp
		${GEN}/caoslexer.cpp
		${GEN}/mngparser.tab.cpp ${GEN}/mngparser.tab.hpp
		${GEN}/mnglexer.cpp
		${GEN}/cataloglexer.cpp
		${GEN}/cmddata.cpp
	)
else(LINUX OR MINGW OR APPLE)
	message(ERROR "This platform is currently not supported.")
endif(LINUX OR MINGW OR APPLE)

