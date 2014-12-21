add_executable ( 
  fgmain
  main.cc
)

add_executable ( 
  pmain
  pmain.cc
)

add_executable ( 
  dumper
  dumper.cpp
)

add_library( 
  shotton2 
  shotton.cc
  edge_potential.cc
)

target_link_libraries ( 
  pmain
  shotton2
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
  fgmain
  shotton2
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
  dumper
  shotton2
  probimage
  color_map
  util
  ${OpenCV_LIBS} 
  ${OpenGMDir}/build/src/external/libexternal-library-maxflow.a 
  ${OpenGMDir}/build/src/external/libexternal-library-qpbo.a
  ${saliency_libs}
  ${QT_QTGUI_LIBRARY} 
  ${QT_QTCORE_LIBRARY}
  ${TBB_LIBRARIES}
)

