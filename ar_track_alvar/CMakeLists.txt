cmake_minimum_required(VERSION 2.8.3)
project(ar_track_alvar)

set(MSG_DEPS
    ar_track_alvar_msgs
    std_msgs
    sensor_msgs
    geometry_msgs
    visualization_msgs)

find_package(catkin COMPONENTS
        genmsg
        roscpp
        tf
        tf2
        image_transport
        resource_retriever
        cv_bridge
        pcl_ros
        pcl_conversions
        message_generation
        ${MSG_DEPS}
        dynamic_reconfigure
        cmake_modules
        REQUIRED)

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(TinyXML REQUIRED)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

cmake_policy(SET CMP0046 OLD)

# dynamic reconfigure support
generate_dynamic_reconfigure_options(cfg/Params.cfg)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ar_track_alvar
  CATKIN_DEPENDS
        ar_track_alvar_msgs
        std_msgs
        roscpp
        tf
        tf2
        message_runtime
        image_transport
        sensor_msgs
        geometry_msgs
        visualization_msgs
        resource_retriever
        cv_bridge
        pcl_ros
        pcl_conversions
        dynamic_reconfigure
)

include_directories(include 
                    ${catkin_INCLUDE_DIRS}
                    ${OpenCV_INCLUDE_DIRS}
                    ${TinyXML_INCLUDE_DIRS}

)

set(GENCPP_DEPS ar_track_alvar_msgs_gencpp std_msgs_gencpp sensor_msgs_gencpp geometry_msgs_gencpp visualization_msgs_gencpp)

add_library(ar_track_alvar
    src/Camera.cpp
    src/CaptureDevice.cpp
    src/Pose.cpp
    src/Marker.cpp
    src/MarkerDetector.cpp
    src/Bitset.cpp
    src/Rotation.cpp
    src/CvTestbed.cpp
    src/CaptureDevice.cpp
    src/CaptureFactory.cpp
    src/CaptureFactory_unix.cpp
    src/FileFormatUtils.cpp
    src/Threads.cpp
    src/Threads_unix.cpp
    src/Mutex.cpp
    src/Mutex_unix.cpp
    src/ConnectedComponents.cpp
    src/Line.cpp src/Plugin.cpp
    src/Plugin_unix.cpp
    src/DirectoryIterator.cpp
    src/DirectoryIterator_unix.cpp
    src/Draw.cpp
    src/Util.cpp
    src/Filter.cpp
    src/Kalman.cpp
    src/kinect_filtering.cpp
    src/Optimization.cpp
    src/MultiMarker.cpp
    src/MultiMarkerBundle.cpp
    src/MultiMarkerInitializer.cpp)
target_link_libraries(ar_track_alvar ${OpenCV_LIBS} ${TinyXML_LIBRARIES} ${catkin_LIBRARIES})
add_dependencies(ar_track_alvar ${GENCPP_DEPS})

# Kinect filtering code
set(KINECT_FILTERING_TARGETS kinect_filtering medianFilter)

add_library(kinect_filtering src/kinect_filtering.cpp)
target_link_libraries(kinect_filtering ${catkin_LIBRARIES})
add_dependencies(kinect_filtering ${GENCPP_DEPS})

add_library(medianFilter src/medianFilter.cpp)
target_link_libraries(medianFilter ar_track_alvar ${catkin_LIBRARIES})
add_dependencies(medianFilter ${GENCPP_DEPS})

set(ALVAR_TARGETS ar_track_alvar individualMarkers individualMarkersNoKinect trainMarkerBundle findMarkerBundles findMarkerBundlesNoKinect createMarker ar_track_alvar)

add_executable(individualMarkers nodes/IndividualMarkers.cpp)
target_link_libraries(individualMarkers ar_track_alvar kinect_filtering ${catkin_LIBRARIES})
add_dependencies(individualMarkers ${PROJECT_NAME}_gencpp ${GENCPP_DEPS} ${PROJECT_NAME}_gencfg)

add_executable(individualMarkersNoKinect nodes/IndividualMarkersNoKinect.cpp)
target_link_libraries(individualMarkersNoKinect ar_track_alvar ${catkin_LIBRARIES})
add_dependencies(individualMarkersNoKinect ${PROJECT_NAME}_gencpp  ${GENCPP_DEPS} ${PROJECT_NAME}_gencfg)

add_executable(trainMarkerBundle nodes/TrainMarkerBundle.cpp)
target_link_libraries(trainMarkerBundle ar_track_alvar ${catkin_LIBRARIES})
add_dependencies(trainMarkerBundle ${PROJECT_NAME}_gencpp ${GENCPP_DEPS})

add_executable(findMarkerBundles nodes/FindMarkerBundles.cpp)
target_link_libraries(findMarkerBundles ar_track_alvar kinect_filtering medianFilter ${catkin_LIBRARIES})
add_dependencies(findMarkerBundles ${PROJECT_NAME}_gencpp ${GENCPP_DEPS})

add_executable(findMarkerBundlesNoKinect nodes/FindMarkerBundlesNoKinect.cpp)
target_link_libraries(findMarkerBundlesNoKinect ar_track_alvar ${catkin_LIBRARIES})
add_dependencies(findMarkerBundlesNoKinect ${PROJECT_NAME}_gencpp ${GENCPP_DEPS})

add_executable(createMarker src/SampleMarkerCreator.cpp)
target_link_libraries(createMarker ar_track_alvar ${catkin_LIBRARIES})
add_dependencies(createMarker ${PROJECT_NAME}_gencpp ${GENCPP_DEPS})

install(TARGETS ${ALVAR_TARGETS} ${KINECT_FILTERING_TARGETS}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(DIRECTORY launch bundles
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/
)

if (CATKIN_ENABLE_TESTING)
  find_package(catkin REQUIRED COMPONENTS roslaunch rostest)
  
  file(GLOB LAUNCH_FILES launch/*.launch test/*.test)
  foreach(LAUNCH_FILE ${LAUNCH_FILES})
    roslaunch_add_file_check(${LAUNCH_FILE} USE_TEST_DEPENDENCIES)
  endforeach()
  
  catkin_download_test_data(
    ${PROJECT_NAME}_4markers_tork.bag
    http://download.ros.org/data/ar_track_alvar/ar_track_alvar_4markers_tork_2017-02-08-11-21-14.bag
    # Workaround the issue http://answers.ros.org/question/253787/accessing-data-downloaded-via-catkin_download_test_data/
    # by downloading into source folder.
    #DESTINATION ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_SHARE_DESTINATION}/test
    DESTINATION ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_SHARE_DESTINATION}/test
    MD5 627aa0316bbfe4334e06023d7c2b4087
   )
  add_rostest(test/marker_arg_config-basic.test DEPENDENCIES ${PROJECT_NAME}_4markers_tork.bag)
  add_rostest(test/marker_arg_config-full.test DEPENDENCIES ${PROJECT_NAME}_4markers_tork.bag)
  add_rostest(test/marker_param_config-basic.test DEPENDENCIES ${PROJECT_NAME}_4markers_tork.bag)
  add_rostest(test/marker_param_config-full.test DEPENDENCIES ${PROJECT_NAME}_4markers_tork.bag)
endif()
