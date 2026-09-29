find_package(Doxygen REQUIRED)

include(FetchContent)
FetchContent_Declare(doxygen-awesome-css
    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/tags/v2.5.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(doxygen-awesome-css)

set(DOXYGEN_PROJECT_NAME "RSTools")
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${PROJECT_SOURCE_DIR}/README.md)
set(DOXYGEN_EXAMPLE_PATH ${PROJECT_SOURCE_DIR}/examples)
set(DOXYGEN_STRIP_FROM_PATH ${PROJECT_SOURCE_DIR})
set(DOXYGEN_STRIP_FROM_INC_PATH ${PROJECT_SOURCE_DIR}/include)

# Every public entity is listed; internals, helper macros and the unfinished crypto module are left out.
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_EXCLUDE ${PROJECT_SOURCE_DIR}/include/RST/crypto)
set(DOXYGEN_EXCLUDE_SYMBOLS
    detail RST::RSTester RST::RSTester::*
    CONCAT CONCAT_IMPL CHECK_T_COMP_OP DECOMPOSE_CMP_OP
    RST_LOG_CONCAT RST_LOG_CONCAT_IMPL RST_LOG_DISCARDED_L
)
set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
set(DOXYGEN_SORT_MEMBER_DOCS NO)
set(DOXYGEN_WARN_AS_ERROR FAIL_ON_WARNINGS)
set(DOXYGEN_QUIET YES)

# doxygen-awesome-css, sidebar-only layout.
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_DISABLE_INDEX NO)
set(DOXYGEN_FULL_SIDEBAR NO)
set(DOXYGEN_HTML_COLORSTYLE LIGHT)
set(DOXYGEN_HTML_EXTRA_STYLESHEET
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-sidebar-only.css
)

doxygen_add_docs(docs
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/README.md
    ${PROJECT_SOURCE_DIR}/CHANGELOG.md
    ${PROJECT_SOURCE_DIR}/CONTRIBUTING.md
    COMMENT "Generating the API documentation"
)
