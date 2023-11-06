#pragma once

#include "../ast/ast.h"
#include "../ast/modify.h"
#include "../object/object.h"
#include "builtins.h"

namespace monkey{
    static const std::shared_ptr<Null> NULL_OBJ = std::make_shared<Null>();
    static const std::shared_ptr<Boolea> TRUE_OBJ = std::make_shared<Boolea>(true);
    static const std::shared_ptr<Boolea> FALSE_OBJ = std::make_shared<Boolea>(false);


    class Evaluator{
    public:
        std::shared_ptr<Object> eval(std::shared_ptr<Node> node, std::shared_ptr<Environment> env) {
            if (std::dynamic_pointer_cast<Program>(node)) {
                return evalProgram(std::dynamic_pointer_cast<Program>(node), env);
            } 
            else if (std::dynamic_pointer_cast<BlockStatement>(node)) {
                return evalBlockStatement(std::dynamic_pointer_cast<BlockStatement>(node), env);
            } 
            else if (std::dynamic_pointer_cast<ExpressionStatement>(node)) {
                return eval(std::dynamic_pointer_cast<ExpressionStatement>(node)->expression, env);
            } 
            else if (std::dynamic_pointer_cast<ReturnStatement>(node)) {
                std::shared_ptr<Object> val = eval(std::dynamic_pointer_cast<ReturnStatement>(node)->returnValue, env);
                if (isError(val)) {
                    return val;
                }
                return std::make_shared<ReturnValue>(val);
            } 
            else if (std::dynamic_pointer_cast<LetStatement>(node)) {
                std::shared_ptr<Object> val = eval(std::dynamic_pointer_cast<LetStatement>(node)->value, env);
                if (isError(val)) {
                    return val;
                }
                env->set(std::dynamic_pointer_cast<LetStatement>(node)->name->value, val);
            }
            else if (std::dynamic_pointer_cast<IntegerLiteral>(node)) {
                return std::make_shared<Integer>(std::dynamic_pointer_cast<IntegerLiteral>(node)->value);
            } 
            else if (std::dynamic_pointer_cast<Boolea>(node)) {
                return nativeBoolToBooleaObject(std::dynamic_pointer_cast<Boolea>(node)->value);
            } 
            else if (std::dynamic_pointer_cast<StringLiteral>(node)) {
                return std::make_shared<Strin>(std::dynamic_pointer_cast<StringLiteral>(node)->value);
            }
            else if (std::dynamic_pointer_cast<PrefixExpression>(node)) {
                auto right = eval(std::dynamic_pointer_cast<PrefixExpression>(node)->right, env);
                if (isError(right)) {
                    return right;
                }
                return evalPrefixExpression(std::dynamic_pointer_cast<PrefixExpression>(node)->op, right);
            } else if (std::dynamic_pointer_cast<InfixExpression>(node)) {
                auto left = eval(std::dynamic_pointer_cast<InfixExpression>(node)->left, env);
                if (isError(left)) {
                    return left;
                }
                auto right = eval(std::dynamic_pointer_cast<InfixExpression>(node)->right, env);
                if (isError(right)) {
                    return right;
                }
                return evalInfixExpression(std::dynamic_pointer_cast<InfixExpression>(node)->op, left, right);
            } 
            else if (std::dynamic_pointer_cast<IfExpression>(node)) {
                return evalIfExpression(std::dynamic_pointer_cast<IfExpression>(node), env);
            } 
            else if (std::dynamic_pointer_cast<Identifier>(node)) {
                return evalIdentifier(std::dynamic_pointer_cast<Identifier>(node), env);
            }
            else if (std::dynamic_pointer_cast<FunctionLiteral>(node)) {
                auto params = std::dynamic_pointer_cast<FunctionLiteral>(node)->parameters;
                auto body = std::dynamic_pointer_cast<FunctionLiteral>(node)->body;
                return std::make_shared<Function>(params, body, env);
            } 
            else if (std::dynamic_pointer_cast<CallExpression>(node)) {
                auto cnode = std::dynamic_pointer_cast<CallExpression>(node);
                if (cnode->function->TokenLiteral() == "quote") {
                    if (cnode->arguments.size() != 1) {
                        return std::make_shared<Error>("wrong number of arguments in quote. got=" + std::to_string(cnode->arguments.size()) + ", want=1");
                    }
                    return quote(cnode->arguments[0], env);
                }
                auto function = eval(cnode->function, env);
                if (isError(function)) {
                    return function;
                }
                auto args = evalExpressions(cnode->arguments, env);
                if (args.size() == 1 && isError(args[0])) {
                    return args[0];
                }
                return applyFunction(function, args);
            }
            else if (std::dynamic_pointer_cast<ArrayLiteral>(node)) {
                auto elements = evalExpressions(std::dynamic_pointer_cast<ArrayLiteral>(node)->elements, env);
                if (elements.size() == 1 && isError(elements[0])) {
                    return elements[0];
                }
                return std::make_shared<Array>(elements);
            }
            else if (std::dynamic_pointer_cast<IndexExpression>(node)) {
                auto index_node = std::dynamic_pointer_cast<IndexExpression>(node);
                auto left = eval(index_node->left, env);
                if (isError(left)) {
                    return left;
                }
                auto index = eval(index_node->index, env);
                if (isError(index)) {
                    return index;
                }
                return evalIndexExpression(left, index);
            }
            else if (std::dynamic_pointer_cast<HashLiteral>(node)) {
                return evalHashLiteral(std::dynamic_pointer_cast<HashLiteral>(node), env);
            }
            return nullptr;
        }

        std::shared_ptr<Object> evalProgram(std::shared_ptr<Program> program, std::shared_ptr<Environment> env) {
            std::shared_ptr<Object> result;
            for (auto& statement : program->statements) {
                result = eval(statement, env);
                if (std::dynamic_pointer_cast<ReturnValue>(result)) {
                    return std::dynamic_pointer_cast<ReturnValue>(result)->value;
                } else if (std::dynamic_pointer_cast<Error>(result)) {
                    return result;
                }
            }
            return result;
        }

        std::shared_ptr<Object> evalBlockStatement(std::shared_ptr<BlockStatement> block, std::shared_ptr<Environment> env) {
            std::shared_ptr<Object> result;
            for (auto& statement : block->statements) {
                result = eval(statement, env);
                if (result != nullptr) {
                    auto type = result->type();
                    if (type == "RETURN_VALUE" || type == "ERROR") {
                        return result;
                    }
                }
            }
            return result;
        }

        std::shared_ptr<Object> nativeBoolToBooleaObject(bool input) {
            if (input) {
                return TRUE_OBJ;
            }
            return FALSE_OBJ;
        }

        std::shared_ptr<Object> evalPrefixExpression(const std::string& op, std::shared_ptr<Object> right) {
            if (op == "!") {
                return evalBangOperatorExpression(right);
            } else if (op == "-") {
                return evalMinusPrefixOperatorExpression(right);
            } else {
                std::string msg = "unknown operator: " + op + right->type();
                return std::make_shared<Error>(msg);
            }
        }

        std::shared_ptr<Object> evalInfixExpression (const std::string& op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
            if (left->type() == "INTEGER" && right->type() == "INTEGER") {
                return evalIntegerInfixExpression(op, left, right);
            } else if (left->type() == "STRING" && right->type() == "STRING") {
                return evalStringInfixExpression(op, left, right);
            } else if (op == "==") {
                return nativeBoolToBooleaObject(left == right);
            } else if (op == "!=") {
                return nativeBoolToBooleaObject(left != right);
            } else if (left->type() != right->type()) {
                std::string msg = "type mismatch: " + left->type() + " " + op + " " + right->type();
                return std::make_shared<Error>(msg);
            } else {
                std::string msg = "unknown operator: " + left->type() + " " + op + " " + right->type();
                return std::make_shared<Error>(msg);
            }
        }

        std::shared_ptr<Object> evalBangOperatorExpression(std::shared_ptr<Object> right) {
            if (right == TRUE_OBJ) {
                return FALSE_OBJ;
            } else if (right == FALSE_OBJ) {
                return TRUE_OBJ;
            } else if (right == NULL_OBJ) {
                return TRUE_OBJ;
            } else {
                return FALSE_OBJ;
            }
        }

        std::shared_ptr<Object> evalMinusPrefixOperatorExpression(std::shared_ptr<Object> right) {
            if (right->type() != "INTEGER") {
                std::string msg = "unknown operator: -" + right->type();
                return std::make_shared<Error>(msg);
            }
            auto value = std::dynamic_pointer_cast<Integer>(right)->value;
            return std::make_shared<Integer>(-value);
        }

        std::shared_ptr<Object> evalIntegerInfixExpression(const std::string& op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
            auto leftVal = std::dynamic_pointer_cast<Integer>(left)->value;
            auto rightVal = std::dynamic_pointer_cast<Integer>(right)->value;
            if (op == "+") {
                return std::make_shared<Integer>(leftVal + rightVal);
            } else if (op == "-") {
                return std::make_shared<Integer>(leftVal - rightVal);
            } else if (op == "*") {
                return std::make_shared<Integer>(leftVal * rightVal);
            } else if (op == "/") {
                return std::make_shared<Integer>(leftVal / rightVal);
            } else if (op == "<") {
                return nativeBoolToBooleaObject(leftVal < rightVal);
            } else if (op == ">") {
                return nativeBoolToBooleaObject(leftVal > rightVal);
            } else if (op == "==") {
                return nativeBoolToBooleaObject(leftVal == rightVal);
            } else if (op == "!=") {
                return nativeBoolToBooleaObject(leftVal != rightVal);
            } else {
                std::string msg = "unknown operator: " + left->type() + " " + op + " " + right->type();
                return std::make_shared<Error>(msg);
            }
        }

        std::shared_ptr<Object> evalStringInfixExpression(const std::string& op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
            auto leftVal = std::dynamic_pointer_cast<Strin>(left)->value;
            auto rightVal = std::dynamic_pointer_cast<Strin>(right)->value;
            if (op == "+") {
                return std::make_shared<Strin>(leftVal + rightVal);
            } else if (op == "==") {
                return nativeBoolToBooleaObject(leftVal == rightVal);
            } else if (op == "!=") {
                return nativeBoolToBooleaObject(leftVal != rightVal);
            } else {
                std::string msg = "unknown operator: " + left->type() + " " + op + " " + right->type();
                return std::make_shared<Error>(msg);
            }
        }


        std::shared_ptr<Object> evalIfExpression(std::shared_ptr<IfExpression> ie, std::shared_ptr<Environment> env) {
            auto condition = eval(ie->condition, env);
            if (isError(condition)) {
                return condition;
            }
            if (isTruthy(condition)) {
                return eval(ie->consequence, env);
            } else if (ie->alternative != nullptr) {
                return eval(ie->alternative, env);
            } else {
                return NULL_OBJ;
            }
        }

        std::shared_ptr<Object> evalIdentifier(std::shared_ptr<Identifier> node, std::shared_ptr<Environment> env) {
            if (getBuiltin(node->value) != nullptr) {
                return getBuiltin(node->value);
            } else if (env->get(node->value) != nullptr) {
                auto val = env->get(node->value);
                return val;
            }
            std::string msg = "identifier not found: " + node->value;
            return std::make_shared<Error>(msg);
        }

        bool isTruthy(std::shared_ptr<Object> obj) {
            if (obj == NULL_OBJ) {
                return false;
            } else if (obj == TRUE_OBJ) {
                return true;
            } else if (obj == FALSE_OBJ) {
                return false;
            } else {
                return true;
            }
        }

        bool isError(std::shared_ptr<Object> obj) {
            if (obj != nullptr) {
                return obj->type() == "ERROR";
            }
            return false;
        }

        std::vector<std::shared_ptr<Object>> evalExpressions(std::vector<std::shared_ptr<Expression>> exps, std::shared_ptr<Environment> env) {
            std::vector<std::shared_ptr<Object>> result;
            for (auto& e : exps) {
                auto evaluated = eval(e, env);
                if (isError(evaluated)) {
                    return {evaluated};
                }
                result.push_back(evaluated);
            }
            return result;
        }

        std::shared_ptr<Object> evalIndexExpression(std::shared_ptr<Object> left, std::shared_ptr<Object> index) {
            if (left->type() == "ARRAY" && index->type() == "INTEGER") {
                return evalArrayIndexExpression(left, index);
            } else if (left->type() == "HASH_TABLE") {
                return evalHashIndexExpression(left, index);
            } else {
                std::string msg = "index operator not supported: " + left->type();
                return std::make_shared<Error>(msg);
            }
        }
    
        std::shared_ptr<Object> evalArrayIndexExpression(std::shared_ptr<Object> left, std::shared_ptr<Object> index) {
            auto array = std::dynamic_pointer_cast<Array>(left);
            auto idx = std::dynamic_pointer_cast<Integer>(index)->value;
            auto max = static_cast<int64_t>(array->elements.size() - 1);
            if (idx < 0 || idx > max) {
                return NULL_OBJ;
            }
            return array->elements[idx];
        }

        std::shared_ptr<Object> evalHashLiteral(std::shared_ptr<HashLiteral> node, std::shared_ptr<Environment> env) {
            std::map<std::string, std::shared_ptr<HashPair>> pairs;
            for (auto& pair : node->pairs) {
                auto key = eval(pair.first, env);
                if (isError(key)) {
                    return key;
                }
                if (!std::dynamic_pointer_cast<Hashable>(key)) {
                    return std::make_shared<Error>("unusable as hash key: " + key->type());
                }
                auto value = eval(pair.second, env);
                if (isError(value)) {
                    return value;
                }
                auto hashed = std::dynamic_pointer_cast<Hashable>(key)->hashKey();
                pairs[hashed->inspect()] = std::make_shared<HashPair>(key, value);
            }
            return std::make_shared<HashTable>(pairs);
        }

        std::shared_ptr<Object> evalHashIndexExpression(std::shared_ptr<Object> left, std::shared_ptr<Object> index) {
            auto hash = std::dynamic_pointer_cast<HashTable>(left);
            if (!std::dynamic_pointer_cast<Hashable>(index)) {
                return std::make_shared<Error>("unusable as hash key: " + index->type());
            }
            auto key = std::dynamic_pointer_cast<Hashable>(index)->hashKey();
            if (hash->pairs.find(key->inspect()) == hash->pairs.end()) {
                return NULL_OBJ;
                // return std::make_shared<Error>("key not found: " + key->inspect() + "\n" + hash->inspect());
            }
            return hash->pairs[key->inspect()]->value;
        }

        std::shared_ptr<Object> applyFunction(std::shared_ptr<Object> fn, std::vector<std::shared_ptr<Object>>& args) {
            if (std::dynamic_pointer_cast<Function>(fn)) {
                auto f = std::dynamic_pointer_cast<Function>(fn);
                auto extendedEnv = extendFunctionEnv(f, args);
                auto evaluated = eval(f->body, extendedEnv);
                return unwrapReturnValue(evaluated);
            } else if (std::dynamic_pointer_cast<Builtin>(fn)) {
                auto f = std::dynamic_pointer_cast<Builtin>(fn);
                return f->fn(args);
            } else {
                std::string msg = "not a function: " + fn->type();
                return std::make_shared<Error>(msg);
            }
        }

        std::shared_ptr<Environment> extendFunctionEnv(std::shared_ptr<Function> fn, std::vector<std::shared_ptr<Object>>& args) {
            auto env = Environment(fn->env);
            for (int i = 0; i < fn->parameters.size(); ++i) {
                env.set(fn->parameters[i]->value, args[i]);
            }
            return std::make_shared<Environment>(env);
        }

        std::shared_ptr<Object> unwrapReturnValue(std::shared_ptr<Object> obj) {
            if (std::dynamic_pointer_cast<ReturnValue>(obj)) {
                return std::dynamic_pointer_cast<ReturnValue>(obj)->value;
            }
            return obj;
        }

        /*** quote_unquote ***/
        std::shared_ptr<Object> quote(std::shared_ptr<Node> node, std::shared_ptr<Environment> env) {
            return std::make_shared<Quote>(evalUnquoteCalls(node, env));
        }

        bool isUnquoteCall(std::shared_ptr<Node> node) {
            if (!std::dynamic_pointer_cast<CallExpression>(node)) {
                return false;
            }
            auto call = std::dynamic_pointer_cast<CallExpression>(node);
            return call->function->TokenLiteral() == "unquote";
        }

        std::shared_ptr<Node> evalUnquoteCalls(std::shared_ptr<Node> node, std::shared_ptr<Environment> env) {
            return modify(node, [&](std::shared_ptr<Node> node) {
                if (!isUnquoteCall(node)) {
                    return node;
                }
                if (!std::dynamic_pointer_cast<CallExpression>(node)) {
                    return node;
                };
                auto call = std::dynamic_pointer_cast<CallExpression>(node);
                if (call->arguments.size() != 1) {
                    return node;
                }
                auto unquoted = eval(call->arguments[0], env);
                return convertObjectToNode(unquoted);
            });
        }

        std::shared_ptr<Node> convertObjectToNode(std::shared_ptr<Object> obj) {
            if (std::dynamic_pointer_cast<Integer>(obj)) {
                auto i = std::dynamic_pointer_cast<Integer>(obj);
                Token token(INT, std::to_string(i->value));
                return std::make_shared<IntegerLiteral>(token, i->value);
            } else if (std::dynamic_pointer_cast<Boolea>(obj)) {
                auto b = std::dynamic_pointer_cast<Boolea>(obj);
                Token token;
                if (b->value) {
                    token = Token(TRUE, "true");
                } else {
                    token = Token(FALSE, "false");
                }
                return std::make_shared<Boolean>(token, b->value);
            } else if (std::dynamic_pointer_cast<Strin>(obj)) {
                auto s = std::dynamic_pointer_cast<Strin>(obj);
                Token token = Token(STRING, s->value);
                return std::make_shared<StringLiteral>(token, s->value);
            } else if (std::dynamic_pointer_cast<Quote>(obj)) {
                return std::dynamic_pointer_cast<Quote>(obj)->node;
            } else  {
                return nullptr;
            }
        }

        void defineMacros(std::shared_ptr<Program> program, std::shared_ptr<Environment> env) {
            std::vector<int> definitions;
            for (int i = 0; i < program->statements.size(); ++i) {
                auto statement = program->statements[i];
                if (isMacroDefinition(statement)) {
                    addMacro(statement, env);
                    definitions.push_back(i);
                }
            }
            for (int i = definitions.size() - 1; i >= 0; --i) {
                int definitionIndex = definitions[i];
                program->statements.erase(program->statements.begin() + definitionIndex);
            }
        }

        bool isMacroDefinition(std::shared_ptr<Node> node) {
            if (!std::dynamic_pointer_cast<LetStatement>(node)) {
                return false;
            }
            auto letStatement = std::dynamic_pointer_cast<LetStatement>(node);
            if (!std::dynamic_pointer_cast<MacroLiteral>(letStatement->value)) {
                return false;
            }
            return true;
        }

        void addMacro(std::shared_ptr<Node> statement, std::shared_ptr<Environment> env) {
            auto letStatement = std::dynamic_pointer_cast<LetStatement>(statement);
            auto macroLiteral = std::dynamic_pointer_cast<MacroLiteral>(letStatement->value);
            auto macro = std::make_shared<Macro>(macroLiteral->parameters, macroLiteral->body, env);
            env->set(letStatement->name->value, macro);
        }

        std::shared_ptr<Node> expandMacros(std::shared_ptr<Node> node, std::shared_ptr<Environment> env) {
            return modify(node, [&](std::shared_ptr<Node> node) {
                if (!std::dynamic_pointer_cast<CallExpression>(node)) {
                    return node;
                }
                auto callExpression = std::dynamic_pointer_cast<CallExpression>(node);
                auto macro = MacroCall(callExpression, env);
                if (macro == nullptr) {
                    return node;
                }
                auto args = quoteArgs(callExpression);
                auto evalEnv = extendMacroEnv(macro, args);
                auto evaluated = eval(macro->body, evalEnv);
                if (!std::dynamic_pointer_cast<Quote>(evaluated)) {
                    return node;
                }
                auto quote = std::dynamic_pointer_cast<Quote>(evaluated);
                return quote->node;
            });
        }

        std::shared_ptr<Macro> MacroCall(std::shared_ptr<CallExpression> node, std::shared_ptr<Environment> env) {
            if (!std::dynamic_pointer_cast<Identifier>(node->function)) {
                return nullptr;
            }
            auto identifier = std::dynamic_pointer_cast<Identifier>(node->function);
            auto obj = env->get(identifier->value);
            return std::dynamic_pointer_cast<Macro>(obj) != nullptr ? std::dynamic_pointer_cast<Macro>(obj) : nullptr;
        }

        std::vector<std::shared_ptr<Quote>> quoteArgs(std::shared_ptr<CallExpression> exp) {
            std::vector<std::shared_ptr<Quote>> args;
            for (auto& a : exp->arguments) {
                args.push_back(std::dynamic_pointer_cast<Quote>(a));
            }
            return args;
        }

        std::shared_ptr<Environment> extendMacroEnv(std::shared_ptr<Macro> macro, std::vector<std::shared_ptr<Quote>>& args) {
            auto extended = Environment(macro->env);
            for (int i = 0; i < macro->parameters.size(); ++i) {
                extended.set(macro->parameters[i]->value, args[i]);
            }
            return std::make_shared<Environment>(extended);
        }
    }; // class Evaluator
} // namespace monkey
