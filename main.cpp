#include <iostream>
#include <fstream>
#include <string>

#include "timer.h"
#include "repl.h"

int main() {
    Timer timer;
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");
    monkey::start(input, output);
    input.close();
    output.close();
    std::cout << "Elapsed time: " << timer.elapsed() << "s" << std::endl;
    return 0;
}