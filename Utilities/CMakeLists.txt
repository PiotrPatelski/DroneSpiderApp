add_library(Utilities STATIC 
    Message.hpp 
    Message.cpp
    Serializer.hpp
    Serializer.cpp
    Deserializer.hpp
    Deserializer.cpp
    UnitContext.hpp)
target_include_directories(Utilities PUBLIC .)
set_property(TARGET Utilities PROPERTY CXX_STANDARD 23)
