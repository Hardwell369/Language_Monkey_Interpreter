#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "../token/token.h"

namespace monkey{
    // 基类抽象语法树节点
    struct Node{
        virtual std::string TokenLiteral() = 0;
        virtual std::string String() = 0;
        virtual ~Node() = default;
    };

    // 语句节点
    struct Statement : Node{
        virtual void statementNode() = 0;
    };

    // 表达式节点
    struct Expression : Node{
        virtual void expressionNode() = 0;
    };

    // 程序树——根节点
    struct Program : Node{
        std::vector<std::shared_ptr<Statement>> statements;

        std::string TokenLiteral() override{
            if(statements.size() > 0){
                return statements[0]->TokenLiteral();
            }
            return "";
        }

        std::string String() override{
            std::string out;
            for(auto& stmt : statements){
                out += stmt->String() + "\n";
            }   
            return out;
        }
    };

    /*** 基础表达式 ***/
    // 标识符
    struct Identifier : Expression{
        Token token;
        std::string value;

        Identifier(const Token& token, const std::string& value) : token(token), value(value){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            return value;
        }
    };

    // 布尔值
    struct Boolean : Expression{
        Token token;
        bool value;

        Boolean(const Token& token, bool value) : token(token), value(value){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            return token.getLiteral();
        }
    };

    // 整数
    struct IntegerLiteral : Expression{
        Token token;
        int64_t value;

        IntegerLiteral(const Token& token) : token(token) {}
        IntegerLiteral(const Token& token, int64_t value) : token(token), value(value){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            return token.getLiteral();
        }
    };

    // 字符串
    struct StringLiteral : Expression{
        Token token;  // the '"' token
        std::string value;

        StringLiteral(const Token& token, const std::string& value) : token(token), value(value){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            return token.getLiteral();
        }
    };

    // 数组
    struct ArrayLiteral : Expression{
        Token token;  // the '[' token
        std::vector<std::shared_ptr<Expression>> elements;

        ArrayLiteral(const Token& token) : token(token){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += "[";
            for(int i = 0; i < elements.size(); ++i){
                out += elements[i]->String();
                if(i != elements.size() - 1){
                    out += ", ";
                }
            }
            out += "]";
            return out;
        }
    };

    // 索引表达式
    struct IndexExpression : Expression{
        Token token; // the '[' token
        std::shared_ptr<Expression> left; // 被索引的对象
        std::shared_ptr<Expression> index; // 索引

        IndexExpression(const Token& token, std::shared_ptr<Expression> left) : token(token), left(left){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += "(";
            out += left->String();
            out += "[";
            out += index->String();
            out += "])";
            return out;
        }
    };

    // hash字面量
    struct HashLiteral : Expression{
        Token token; // the '{' token
        std::map<std::shared_ptr<Expression>, std::shared_ptr<Expression>> pairs;

        HashLiteral(const Token& token) : token(token){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += "{";
            int i = 0;
            for(auto& pair : pairs){
                out += pair.first->String();
                out += ": ";
                out += pair.second->String();
                if(i != pairs.size() - 1){
                    out += ", ";
                }
                ++i;
            }
            out += "}";
            return out;
        }
    };

    /*** 语句结构 ***/
    // let 语句
    struct LetStatement : Statement{
        Token token; // the 'LET' token
        std::shared_ptr<Identifier> name;
        std::shared_ptr<Expression> value;

        LetStatement(const Token& token) : token(token){}

        void statementNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += token.getLiteral() + " ";
            out += name->String();
            out += " = ";
            if(value != nullptr){
                out += value->String();
            }
            out += ";";
            return out;
        }
    };

    // return 语句
    struct ReturnStatement : Statement{
        Token token; // the 'return' token
        std::shared_ptr<Expression> returnValue;

        ReturnStatement(const Token& token) : token(token){}

        void statementNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += token.getLiteral() + " ";
            if(returnValue != nullptr){
                out += returnValue->String();
            }
            out += ";";
            return out;
        }
    };

    // 表达式语句
    struct ExpressionStatement : Statement{
        Token token; // the first token of the expression
        std::shared_ptr<Expression> expression;

        ExpressionStatement(const Token& token) : token(token){}

        void statementNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            if(expression != nullptr){
                return expression->String();
            }
            return "";
        }
    };

    // 块语句
    struct BlockStatement : Statement{
        Token token; // the '{' token
        std::vector<std::shared_ptr<Statement>> statements;

        BlockStatement(const Token& token) : token(token){}

        void statementNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            for(auto& stmt : statements){
                out += stmt->String();
            }
            return out;
        }
    };

    /*** 复杂表达式 ***/
    // 前缀表达式
    struct PrefixExpression : Expression{
        Token token;
        std::string op;
        std::shared_ptr<Expression> right;

        PrefixExpression(const Token& token, const std::string& op) : token(token), op(op){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += "(";
            out += op;
            out += right->String();
            out += ")";
            return out;
        }
    };

    // 中缀表达式
    struct InfixExpression : Expression{
        Token token;
        std::shared_ptr<Expression> left;
        std::string op;
        std::shared_ptr<Expression> right;

        InfixExpression(const Token& token, const std::string& op, std::shared_ptr<Expression> left) : token(token), op(op), left(left){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += "(";
            out += left->String();
            out += " " + op + " ";
            out += right->String();
            out += ")";
            return out;
        }
    };

    // if表达式
    struct IfExpression : Expression{
        Token token; // the 'if' token
        std::shared_ptr<Expression> condition; // if 条件
        std::shared_ptr<BlockStatement> consequence; // if 条件为真时执行的语句
        std::shared_ptr<BlockStatement> alternative; // if 条件为假时执行的语句(可有可无)

        IfExpression(const Token& token) : token(token){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += "if";
            out += condition->String();
            out += " ";
            out += consequence->String();
            if(alternative != nullptr){
                out += "else ";
                out += alternative->String();
            }
            return out;
        }
    };

    // 函数定义字面量
    struct FunctionLiteral : Expression{
        Token token; // the 'fn' token
        std::vector<std::shared_ptr<Identifier>> parameters; // 参数列表
        std::shared_ptr<BlockStatement> body; // 函数体

        FunctionLiteral(const Token& token) : token(token){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += token.getLiteral();
            out += "(";
            for(int i = 0; i < parameters.size(); ++i){
                out += parameters[i]->String();
                if(i != parameters.size() - 1){
                    out += ", ";
                }
            }
            out += ")";
            out += body->String();
            return out;
        }
    };

    // 函数调用表达式
    struct CallExpression : Expression{
        Token token; // the '(' token
        std::shared_ptr<Expression> function; // 函数
        std::vector<std::shared_ptr<Expression>> arguments; // 参数列表

        CallExpression(const Token& token, std::shared_ptr<Expression> function) : token(token), function(function){}

        void expressionNode() override{}
        std::string TokenLiteral() override{
            return token.getLiteral();
        }
        std::string String() override{
            std::string out;
            out += function->String();
            out += "(";
            for(int i = 0; i < arguments.size(); ++i){
                out += arguments[i]->String();
                if(i != arguments.size() - 1){
                    out += ", ";
                }
            }
            out += ")";
            return out;
        }
    };

    // 宏定义字面量
    struct MacroLiteral : Expression {
        Token token; // the 'macro' token
        std::vector<std::shared_ptr<Identifier>> parameters; // 参数列表
        std::shared_ptr<BlockStatement> body; // 函数体

        MacroLiteral(const Token& token) : token(token) {}

        void expressionNode() override {}
        std::string TokenLiteral() override {
            return token.getLiteral();
        }
        std::string String() override {
            std::string out;
            out += token.getLiteral();
            out += "(";
            for (int i = 0; i < parameters.size(); ++i) {
                out += parameters[i]->String();
                if (i != parameters.size() - 1) {
                    out += ", ";
                }
            }
            out += ")";
            out += body->String();
            return out;
        }
    };

} // namespace monkey