cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Atividade_U4C4O12T C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adicionar o executável
add_executable(Atividade_U4C4O12T Atividade_U4C4O12T.c)

# Gere o arquivo de cabeçalho a partir do arquivo PIO
pico_generate_pio_header(Atividade_U4C4O12T ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

# Definir nome e versão do programa
pico_set_program_name(Atividade_U4C4O12T "Atividade_U4C4O12T")
pico_set_program_version(Atividade_U4C4O12T "0.1")

# Configurar UART/USB para debug
pico_enable_stdio_uart(Atividade_U4C4O12T 0)
pico_enable_stdio_usb(Atividade_U4C4O12T 0)

# Adicionar bibliotecas padrão ao projeto
target_link_libraries(Atividade_U4C4O12T
        pico_stdlib
        hardware_pio               # Biblioteca necessária para usar o PIO (Programmable Input/Output)
        # ws2812_program removido, pois o pico_generate_pio_header já inclui o código gerado
)

# Definir diretórios de inclusão
target_include_directories(Atividade_U4C4O12T PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Gerar saídas adicionais como o arquivo .uf2
pico_add_extra_outputs(Atividade_U4C4O12T)
