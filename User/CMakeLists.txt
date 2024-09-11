aux_source_directory(src src_dir)
target_sources(stm32cubemx
    INTERFACE ${src_dir}
)

target_include_directories(stm32cubemx
    INTERFACE include
)