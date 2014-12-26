
add_executable ( 
  mtmain 
  ./main.cc 
)
add_executable ( 
  pmtmain
  pmain.cc
)

add_library( 
  shotton1 
  shotton.cc
  edge_potential.cc
)

target_link_libraries ( 
  pmtmain
  shotton1
  probimage
  color_map
  util
  ${OpenCV_LIBS} 
  ${saliency_libs}
  ${OpenGMDir}/build/src/external/libexternal-library-maxflow.a 
  ${OpenGMDir}/build/src/external/libexternal-library-qpbo.a
  ${QT_QTGUI_LIBRARY} 
  ${QT_QTCORE_LIBRARY}
  ${TBB_LIBRARIES}
)

target_link_libraries ( 
  mtmain 
  shotton1
  probimage
  color_map
  util
  ${OpenCV_LIBS} 
  ${OpenGMDir}/build/src/external/libexternal-library-maxflow.a 
  ${OpenGMDir}/build/src/external/libexternal-library-qpbo.a
  ${QT_QTGUI_LIBRARY} 
  ${QT_QTCORE_LIBRARY}
)

