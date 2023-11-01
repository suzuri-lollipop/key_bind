cmake_minimum_required(VERSION 3.10)

# プロジェクト名と使用するC++のバージョンを指定
project(MouseCoordinateManager VERSION 1.0 LANGUAGES CXX)

# C++の標準を指定
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 実行ファイルを生成
add_executable(MouseCoordinateManager main.cpp)

# Windows APIをリンクするために必要
target_link_libraries(MouseCoordinateManager PRIVATE user32)