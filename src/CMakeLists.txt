cmake_minimum_required(VERSION 3.16)
add_executable(${PROJECT_NAME} WIN32 main.c
                                     draw.c
                                     event.c
                                     state.c
                                     tetromino.c)

