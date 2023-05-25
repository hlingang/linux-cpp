if (TARGET jsoncpp_static)
    add_library(JsonCpp::JsonCpp INTERFACE IMPORTED)
    set_target_properties(JsonCpp::JsonCpp PROPERTIES INTERFACE_LINK_LIBRARIES "jsoncpp_static")
elseif (TARGET jsoncpp_lib)
    add_library(JsonCpp::JsonCpp INTERFACE IMPORTED)
    set_target_properties(JsonCpp::JsonCpp PROPERTIES INTERFACE_LINK_LIBRARIES "jsoncpp_lib")
endif ()