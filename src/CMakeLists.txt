project(priveos-placeholder)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( priveos-placeholder priveos-placeholder priveos-placeholder.cpp )
target_include_directories( priveos-placeholder PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( priveos-placeholder ${CMAKE_SOURCE_DIR}/../ricardian )