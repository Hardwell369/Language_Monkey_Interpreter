#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

#include "../ast/ast.h"

namespace monkey{
    /*** 定义对象系统 ***/
    // 前置声明
    class Environment;
    class HashKey;
    // 抽象对象类型基类
    class Object{
    public:
        virtual std::string type() = 0;
        virtual std::string inspect() = 0;

        virtual ~Object() = default;
    };

    // 可哈希对象
    class Hashable : public Object{
    public:
        virtual std::shared_ptr<HashKey> hashKey() = 0;
    };

    // 整数对象
    class Integer : public Hashable{
    public:
        int64_t value;

        Integer(int value) : value(value){}

        std::string type() override{
            return "INTEGER";
        }

        std::string inspect() override{
            return std::to_string(value);
        }

        std::shared_ptr<HashKey> hashKey() override{
            return std::make_shared<HashKey>(type(), value);
        }
    };

    // 布尔对象
    class Boolea : public Hashable{
    public:
        bool value;

        Boolea(bool value) : value(value){}

        std::string type() override{
            return "BOOLEAN";
        }

        std::string inspect() override{
            return value ? "true" : "false";
        }

        std::shared_ptr<HashKey> hashKey() override{
            return std::make_shared<HashKey>(type(), value ? 1 : 0);
        }
    };

    // 字符串对象
    class Strin : public Hashable{
    public:
        std::string value;

        Strin(const std::string& value) : value(value){}

        std::string type() override{
            return "STRING";
        }

        std::string inspect() override{
            return value;
        }

        std::shared_ptr<HashKey> hashKey() override{
            u_int64_t hash = 0;
            for (auto& c : value) {
                hash += c;
            }
            return std::make_shared<HashKey>(type(), hash);
        }
    };

    // 空对象
    class Null : public Object{
    public:
        std::string type() override{
            return "NULL";
        }

        std::string inspect() override{
            return "null";
        }
    };

    // 返回值对象
    class ReturnValue : public Object{
    public:
        std::shared_ptr<Object> value;

        ReturnValue(std::shared_ptr<Object> value) : value(value){}

        std::string type() override{
            return "RETURN_VALUE";
        }

        std::string inspect() override{
            return value->inspect();
        }
    };

    // 错误对象
    class Error : public Object{
    public:
        std::string message;

        Error(const std::string& message) : message(message){}

        std::string type() override{
            return "ERROR";
        }

        std::string inspect() override{
            return "ERROR: " + message;
        }
    };

    // 函数对象
    class Function : public Object{
    public:
        std::vector<std::shared_ptr<Identifier>> parameters;
        std::shared_ptr<BlockStatement> body;
        std::shared_ptr<Environment> env;

        Function(std::vector<std::shared_ptr<Identifier>> parameters, std::shared_ptr<BlockStatement> body, std::shared_ptr<Environment> env) : parameters(parameters), body(body), env(env){}

        std::string type() override{
            return "FUNCTION";
        }

        std::string inspect() override{
            std::string out = "";
            out += "fn(";
            for (auto& param : parameters) {
                out += param->String();
                out += ", ";
            }
            out += ") {\n";
            out += body->String();
            out += "\n}";
            return out;
        }
    }; 

    // 内置函数对象
    class Builtin : public Object{
    public:
        using builtin_function = std::function<std::shared_ptr<Object>(std::vector<std::shared_ptr<Object>>)>;
        builtin_function fn;

        Builtin(builtin_function fn) : fn(fn){}

        std::string type() override{
            return "BUILTIN";
        }

        std::string inspect() override{
            return "builtin function";
        }
    };

    // 数组对象
    class Array : public Object{
    public:
        std::vector<std::shared_ptr<Object>> elements;

        Array(std::vector<std::shared_ptr<Object>> elements) : elements(elements){}

        std::string type() override{
            return "ARRAY";
        }

        std::string inspect() override{
            std::string out = "";
            out += "[";
            for (auto& elem : elements) {
                out += elem->inspect();
                if (elem != elements.back()) {
                    out += ", ";
                }
            }
            out += "]";
            return out;
        }
    };

    // hash 键
    class HashKey : public Object{
    public: 
        std::string objectType;
        u_int64_t value;

        HashKey(std::string objectType, u_int64_t value) : objectType(objectType), value(value){}

        std::string type() override{
            return "HASH_KEY";
        }
        std::string inspect() override{
            return objectType + "_" + std::to_string(value);
        }

        bool operator==(const HashKey& other) const {
            return objectType == other.objectType && value == other.value;
        }
    };

    // 键值对对象
    class HashPair : public Object{
    public:
        std::shared_ptr<Object> key;
        std::shared_ptr<Object> value;

        HashPair(std::shared_ptr<Object> key, std::shared_ptr<Object> value) : key(key), value(value){}

        std::string type() override{
            return "HASH_PAIR";
        }

        std::string inspect() override{
            return key->inspect() + " : " + value->inspect();
        }
    };

    // 哈希对象
    class HashTable : public Object{
    public:
        std::map<std::string, std::shared_ptr<HashPair>> pairs;

        HashTable(std::map<std::string, std::shared_ptr<HashPair>> pairs) : pairs(pairs){}

        std::string type() override{
            return "HASH_TABLE";
        }

        std::string inspect() override{
            std::string out = "";
            out += "{";
            int i = 0;
            for (auto& pair : pairs) {
                out += pair.second->inspect();
                if (i != pairs.size() - 1) {
                    out += ", ";
                }
                ++i;
            }
            out += "}";
            return out;
        }
    };

    class Quote : public Object{
    public:
        std::shared_ptr<Node> node;

        Quote(std::shared_ptr<Node> node) : node(node){}

        std::string type() override{
            return "QUOTE";
        }

        std::string inspect() override{
            return "QUOTE(" + node->String() + ")";
        }
    };

    class Macro : public Object{
    public:
        std::vector<std::shared_ptr<Identifier>> parameters;
        std::shared_ptr<BlockStatement> body;
        std::shared_ptr<Environment> env;

        Macro(std::vector<std::shared_ptr<Identifier>> parameters, std::shared_ptr<BlockStatement> body, std::shared_ptr<Environment> env) : parameters(parameters), body(body), env(env){}

        std::string type() override{
            return "MACRO";
        }

        std::string inspect() override{
            std::string out = "";
            out += "macro(";
            for (auto& param : parameters) {
                out += param->String();
                out += ", ";
            }
            out += ") {\n";
            out += body->String();
            out += "\n}";
            return out;
        }
    };

    class Environment{
    public:
        Environment() = default;
        Environment(std::shared_ptr<Environment> outer) : outer(outer){}

        std::shared_ptr<Object> get(const std::string& name){
            auto it = store.find(name);
            if(it != store.end()) {
                return it->second;
            } else if(outer != nullptr) {
                return outer->get(name);
            }
            return nullptr;
        }

        std::shared_ptr<Object> set(const std::string& name, std::shared_ptr<Object> value){
            store[name] = value;
            return value;
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<Object>> store;
        std::shared_ptr<Environment> outer;   // 外部作用域
    };
} // namespace monkey