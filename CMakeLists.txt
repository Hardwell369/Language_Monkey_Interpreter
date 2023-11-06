project(monkey)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 设置要编译的源文件
set(SOURCE_FILES main.cpp)

# 设置要编译的头文件
set(HEADER_FILES 
    ./ast/ast.h
    ./evaluator/evaluator.h
    ./lexer/lexer.h
    ./object/object.h
    ./parser/parser.h
    ./token/token.h
    repl.h
    )

# 生成可执行文件
add_executable(monkey ${SOURCE_FILES} ${HEADER_FILES})