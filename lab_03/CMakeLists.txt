# CMakeList.txt: проект CMake для API_Poco_SQLite; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.16)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("API_Poco_PG")


add_executable (API_Poco_PG "API_Poco_PG.cpp" "API_Poco_PG.h" "Database.cpp" "Database.h" "jwtUtils.h" "Handlers.h" "Handlers.cpp")

find_package(PostgreSQL REQUIRED)

# пакет POCO в Ubuntu ссылается на PostgreSQL::client,
# но современный CMake создаёт только PostgreSQL::PostgreSQL
if (NOT TARGET PostgreSQL::client)
    add_library(PostgreSQL::client ALIAS PostgreSQL::PostgreSQL)
endif()

find_package(Poco REQUIRED COMPONENTS
    Foundation
    Net
    Util
    JSON
    Data
    DataPostgreSQL
    DataSQLite
    MongoDB
    JWT
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET API_Poco_PG PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(API_Poco_PG PRIVATE
    Poco::Foundation
    Poco::Net
    Poco::Util
    Poco::JSON
    Poco::Data
    Poco::DataSQLite
    Poco::DataPostgreSQL
    Poco::MongoDB
    Poco::JWT
)

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
