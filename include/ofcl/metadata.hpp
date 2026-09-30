#pragma once

#include <string>

namespace ofcl {

struct ProjectMetadata {
    static constexpr const char* name = "OrderFlow Continuum Lab";
    static constexpr const char* short_name = "ofcl";
    static constexpr const char* author = "Nabil Khondaker";
    static constexpr const char* license = "MIT";
    static constexpr const char* description =
        "Hybrid discrete LOB + continuum-mechanics high-frequency market microstructure simulator";
};

}  // namespace ofcl
