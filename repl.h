#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "lexer/lexer.h"
#include "token/token.h"
#include "parser/parser.h"
#include "evaluator/evaluator.h"

namespace monkey{
    const std::string PROMPT = ">> ";

    const std::string WELCOME = R"(                         __                          
 /'\_/`\                /\ \                         
/\      \    ___     ___\ \ \/'\      __   __  __    
\ \ \__\ \  / __`\ /' _ `\ \ , <    /'__`\/\ \/\ \   
 \ \ \_/\ \/\ \L\ \/\ \/\ \ \ \\`\ /\  __/\ \ \_\ \  
  \ \_\\ \_\ \____/\ \_\ \_\ \_\ \_\ \____\\/`____ \ 
   \/_/ \/_/\/___/  \/_/\/_/\/_/\/_/\/____/ `/___/> \
                                               /\___/
                                               \/__/ )";

    const std::string MONKEY_FACE = R"(            __,__
   .--.  .-"     "-.  .--.
  / .. \/  .-. .-.  \/ .. \
 | |  '|  /   Y   \  |'  | |
 | \   \  \ 0 | 0 /  /   / |
  \ '- ,\.-"""""""-./, -' /
   ''-' /_   ^ ^   _\ '-''
       |  \._   _./  |
       \   \ '~' /   /
        '._ '-=-' _.'
           '-----')";


    void printParserErrors(std::ofstream& output, std::string errors) {
        output << MONKEY_FACE << "\n";
        output << "Woops! We ran into some monkey business here!\n";
        output << "parser errors:\n";
        output << errors;
    }

    // repl
    void start(std::ifstream& input, std::ofstream& output) {
        std::string line;
        std::string program;
        static Evaluator evaluator;

        while (getline(input, line)) {
            program += line;
            program += "\n";
        }
        
        std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(program);
        std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
        
        auto program_ast = parser->parseProgram();
        if (parser->getErrors().size() != 0) {
            printParserErrors(output, parser->getErrors());
            return;
        }
        
        output << WELCOME << "\n" << std::endl;
        static std::shared_ptr<Environment> env = std::make_shared<Environment>();
        static std::shared_ptr<Environment> macroEnv = std::make_shared<Environment>();
        evaluator.defineMacros(program_ast, macroEnv);
        auto expanded = evaluator.expandMacros(program_ast, macroEnv);
        auto evaluated = evaluator.eval(expanded, env);
        if (evaluated != nullptr) {
            output << evaluated->inspect() << "\n" << std::endl;
        }
    }

}; // namespace monkey