#pragma once

#include <string>
#include "../token/token.h"

namespace monkey {
    class Lexer{
    public:
        Lexer(std::string input) : input(input) {
            readPosition = 0;
            readChar();
        }

        Token nextToken() {
            Token token;
            skipWhitespace();
            switch(ch) {
                case '=':
                    if (peekChar() == '=') {
                        char curChar = ch;
                        readChar();
                        token = Token(TokenType::EQ, std::string(1, curChar) + std::string(1, ch));
                    } else {
                        token = Token(TokenType::ASSIGN, std::string(1, ch));
                    }
                    break;
                case '+':
                    token = Token(TokenType::PLUS, std::string(1, ch));
                    break;
                case '-':
                    token = Token(TokenType::MINUS, std::string(1, ch));
                    break;
                case '!':
                    if (peekChar() == '=') {
                        char curChar = ch;
                        readChar();
                        token = Token(TokenType::NOT_EQ, std::string(1, curChar) + std::string(1, ch));
                    } else {
                        token = Token(TokenType::BANG, std::string(1, ch));
                    }
                    break;
                case '/':
                    token = Token(TokenType::SLASH, std::string(1, ch));
                    break;  
                case '*':
                    token = Token(TokenType::ASTERISK, std::string(1, ch));
                    break;
                case '<':
                    token = Token(TokenType::LT, std::string(1, ch));
                    break;
                case '>':
                    token = Token(TokenType::GT, std::string(1, ch));
                    break;
                case ';':
                    token = Token(TokenType::SEMICOLON, std::string(1, ch));
                    break;
                case ',':
                    token = Token(TokenType::COMMA, std::string(1, ch));
                    break;
                case ':':
                    token = Token(TokenType::COLON, std::string(1, ch));
                    break;
                case '(':
                    token = Token(TokenType::LPAREN, std::string(1, ch));
                    break;
                case ')':
                    token = Token(TokenType::RPAREN, std::string(1, ch));
                    break;
                case '[':
                    token = Token(TokenType::LBRACKET, std::string(1, ch));
                    break;
                case ']':
                    token = Token(TokenType::RBRACKET, std::string(1, ch));
                    break;
                case '{':
                    token = Token(TokenType::LBRACE, std::string(1, ch));
                    break;
                case '}':
                    token = Token(TokenType::RBRACE, std::string(1, ch));
                    break;
                case '"':
                    token = Token(TokenType::STRING, readString());
                    break;
                case 0:
                    token = Token(TokenType::EOF, "");
                    break;
                default:
                    if (isLetter(ch)) {    // 变量
                        std::string literal = readIdentifier();
                        TokenType type = lookupIdent(literal);
                        token = Token(type, literal);
                        return token;
                    } else if (isDigit(ch)) {   // 数字
                        std::string literal = readNumber();
                        token = Token(TokenType::INT, literal);
                        return token;
                    } else {    // 未知字符
                        token = Token(TokenType::ILLEGAL, std::string(1, ch));
                    }
            }
            readChar();
            return token;
        }

    private:
        // helper functions
        // 读取字符, 并更新position和readPosition
        void readChar() {
            if (readPosition >= input.length()) {
                ch = 0;
            } else {
                ch = input[readPosition];
            }
            position = readPosition;
            ++readPosition;
        }

        // 跳过空白字符
        void skipWhitespace() {
            while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
                readChar();
            }
        }

        char peekChar() {
            if (readPosition >= input.length()) {
                return 0;
            } else {
                return input[readPosition];
            }
        }

        // 读取完整的变量名
        std::string readIdentifier() {
            int pos = position;
            while (isLetter(ch)) {
                readChar();
            }
            return input.substr(pos, position - pos);
        }

        // 读取完整的数字
        std::string readNumber() {
            int pos = position;
            while (isDigit(ch)) {
                readChar();
            }
            return input.substr(pos, position - pos);
        }

        // 读取字符串
        std::string readString() {
            int pos = position + 1;
            while (true) {
                readChar();
                if (ch == '"' || ch == 0) {
                    break;
                }
            }
            return input.substr(pos, position - pos);
        }

        // 判断是否为字符
        bool isLetter(char ch) {
            return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
        }

        // 判断是否为数字
        bool isDigit(char ch) {
            return '0' <= ch && ch <= '9';
        }

    private:
        std::string input;
        int position; // current position in input (points to current char)
        int readPosition; // current reading position in input (after current char)
        char ch; // current char under examination
    };
    
}; // namespace monkey