set(token "a869897b4a98f43589ca5af1cc2b599f973b8619")
set(gh_repo "https://api.github.com/repos/NWChemEx-Project/LibChemist")

if(BUILD_PYBINDINGS)
    set(WITH_SHARED TRUE)
endif()
set(gh_repo "https://api.github.com/repos/NWChemEx-Project/Utilities")
hunter_config(
        Utilities
        VERSION 0.0.0
        URL "${gh_repo}/tarball/master?access_token=${token}"
        SHA1 "682dc47e61d0e93990a2b6cdf6520c398006bd45"
        CMAKE_ARGS "BUILD_TESTS=off;BUILD_SHARED_LIBS=${WITH_SHARED}"
)

hunter_config(
        bphash
        VERSION "v1.1"
        URL "https://github.com/bennybp/BPHash/archive/v1.1.tar.gz"
        SHA1 "88e14eda8b1f8224caeae8c43407770a651e027e"
        CMAKE_ARGS "BUILD_SHARED_LIBS=${WITH_SHARED}"
)
