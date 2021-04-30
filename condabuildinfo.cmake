set(Boost_USE_STATIC_LIBS TRUE)
find_package(Boost REQUIRED log_setup log system filesystem serialization timer)
list(APPEND EXT_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
list(APPEND EXT_LIB_DIRS ${Boost_LIBRARY_DIRS})
list(APPEND EXT_LIBS ${Boost_LIBRARIES};z)

