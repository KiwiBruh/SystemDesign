# CMakeList.txt: проект CMake для API_Poco_SQLite; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.16)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("API_Poco_SQLite")


add_executable (API_Poco_SQLite "API_Poco_SQLite.cpp" "API_Poco_SQLite.h" "Database.cpp" "Database.h" "jwtUtils.h" "Handlers.h" "Handlers.cpp")

find_package(Poco REQUIRED COMPONENTS
    Foundation
    Net
    Util
    JSON
    Data
    DataSQLite
    MongoDB
    JWT
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET API_Poco_SQLite PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(API_Poco_SQLite PRIVATE
    Poco::Foundation
    Poco::Net
    Poco::Util
    Poco::JSON
    Poco::Data
    Poco::DataSQLite
    Poco::MongoDB
    Poco::JWT
)

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
