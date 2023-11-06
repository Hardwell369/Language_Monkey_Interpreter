#pragma once

#include <string>
#include <vector>
#include <map>

namespace monkey { 
    // #undef EOF to avoid conflict with stdlib
    #ifdef EOF
    #undef EOF
    #endif

    // Token types
    enum TokenType {
        ILLEGAL = 0, // unknown token
        EOF,    // end of file

        IDENT,  // identifier
        INT,    // integer
        STRING, // string

        ASSIGN, // operator =
        PLUS,   // operator +
        MINUS,  // operator -
        BANG,   // operator !
        ASTERISK, // operator *
        SLASH,  // operator /

        LT,     // operator <
        GT,     // operator >

        EQ,     // operator ==
        NOT_EQ, // operator !=

        COMMA,  // operator ,
        SEMICOLON, // operator ;
        COLON, // operator :

        LPAREN, // operator (
        RPAREN, // operator )
        LBRACKET, // operator [
        RBRACKET, // operator ]
        LBRACE, // operator {
        RBRACE, // operator }

        FUNCTION, // keyword fn
        LET,    // keyword let
        TRUE,   // keyword true
        FALSE,  // keyword false
        IF,     // keyword if
        ELSE,   // keyword else
        RETURN, // keyword return
        MACRO // keyword macro
    };
    
    std::vector<std::string> TokenTypeString = {
        "ILLEGAL",
        "EOF",
        "IDENT",
        "INT",
        "STRING",
        "ASSIGN",
        "PLUS",
        "MINUS",
        "BANG",
        "ASTERISK",
        "SLASH",
        "LT",
        "GT",
        "EQ",
        "NOT_EQ",
        "COMMA",
        "SEMICOLON",
        "COLON",
        "LPAREN",
        "RPAREN",
        "LBRACKET",
        "RBRACKET",
        "LBRACE",
        "RBRACE",
        "FUNCTION",
        "LET",
        "TRUE",
        "FALSE",
        "IF",
        "ELSE",
        "RETURN",
        "MACRO"
    };

    class Token {
    public:
        Token() {}
        Token(TokenType type, std::string literal) : type(type), literal(literal) {}

        TokenType getType() { return type; }
        std::string getTypeString() { return TokenTypeString[type]; }
        std::string getLiteral() { return literal; }

    private:
        TokenType type;
        std::string literal;
    };


    // keywords maps: keywords -> TokenType
    static std::map<std::string, TokenType> keywords = {
        {"fn", TokenType::FUNCTION},
        {"let", TokenType::LET},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"return", TokenType::RETURN},
        {"macro", TokenType::MACRO}
    };
    
    // lookupIdent checks the keywords table to see whether the given
    TokenType lookupIdent(std::string ident) {
        if (keywords.find(ident) != keywords.end()) {
            return keywords[ident];
        }
        return TokenType::IDENT;
    }
}; // namespace monkey