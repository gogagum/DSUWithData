add_library(DSUWithData DSUWithData.hpp DefaultDSUData.hpp impl/impl.hpp)
set_target_properties(DSUWithData PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(DSUWithData PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
