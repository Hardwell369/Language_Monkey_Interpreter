#pragma once

#include <iostream>
#include <string>
#include <map>

#include "../object/object.h"

namespace monkey{
    // len
    std::shared_ptr<Object> len(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 1){
            return std::make_shared<Error>("wrong number of arguments in builtin function(len). got=" + std::to_string(args.size()) + ", want=1");
        } else if(args[0]->type() == "STRING"){
            return std::make_shared<Integer>(static_cast<int64_t>(std::dynamic_pointer_cast<Strin>(args[0])->value.size()));
        } else if(args[0]->type() == "ARRAY"){
            return std::make_shared<Integer>(static_cast<int64_t>(std::dynamic_pointer_cast<Array>(args[0])->elements.size()));
        } else {
            return std::make_shared<Error>("argument to `len` not supported, got " + args[0]->type());
        }
    }

    // first
    std::shared_ptr<Object> first(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 1){
            return std::make_shared<Error>("wrong number of arguments in builtin function(first). got=" + std::to_string(args.size()) + ", want=1");
        } else if(args[0]->type() != "ARRAY"){
            return std::make_shared<Error>("argument to `first` must be ARRAY, got " + args[0]->type());
        } else {
            auto arr = std::dynamic_pointer_cast<Array>(args[0]);
            if(arr->elements.size() > 0){
                return arr->elements[0];
            } else {
                return nullptr;
            }
        }
    }

    // last
    std::shared_ptr<Object> last(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 1){
            return std::make_shared<Error>("wrong number of arguments in builtin function(last). got=" + std::to_string(args.size()) + ", want=1");
        } else if(args[0]->type() != "ARRAY"){
            return std::make_shared<Error>("argument to `last` must be ARRAY, got " + args[0]->type());
        } else {
            auto arr = std::dynamic_pointer_cast<Array>(args[0]);
            if(arr->elements.size() > 0){
                return arr->elements[arr->elements.size() - 1];
            } else {
                return nullptr;
            }
        }
    }

    // rest 接受一个数组，返回一个新数组，新数组包含原数组除第一个元素外的所有元素
    std::shared_ptr<Object> rest(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 1){
            return std::make_shared<Error>("wrong number of arguments in builtin function(rest). got=" + std::to_string(args.size()) + ", want=1");
        } else if(args[0]->type() != "ARRAY"){
            return std::make_shared<Error>("argument to `rest` must be ARRAY, got " + args[0]->type());
        } else {
            auto arr = std::dynamic_pointer_cast<Array>(args[0]);
            if(arr->elements.size() > 0){
                std::vector<std::shared_ptr<Object>> newElements;
                for(int i = 1; i < arr->elements.size(); ++i){
                    newElements.push_back(arr->elements[i]);
                }
                return std::make_shared<Array>(newElements);
            } else {
                return nullptr;
            }
        }
    }

    // push 接受一个数组和一个元素，返回一个新数组，新数组包含原数组的所有元素和新元素
    std::shared_ptr<Object> push(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 2){
            return std::make_shared<Error>("wrong number of arguments in builtin function(push). got=" + std::to_string(args.size()) + ", want=2");
        } else if(args[0]->type() != "ARRAY"){
            return std::make_shared<Error>("argument to `push` must be ARRAY, got " + args[0]->type());
        } else {
            auto arr = std::dynamic_pointer_cast<Array>(args[0]);
            std::vector<std::shared_ptr<Object>> newElements;
            for(int i = 0; i < arr->elements.size(); ++i){
                newElements.push_back(arr->elements[i]);
            }
            newElements.push_back(args[1]);
            return std::make_shared<Array>(newElements);
        }
    }

    // puts 
    std::shared_ptr<Object> puts(std::vector<std::shared_ptr<Object>> args){
        for(auto& arg : args){
            std::cout << arg->inspect() << std::endl;
        }
        return nullptr;
    }

    static std::map<std::string, std::shared_ptr<Builtin>> builtins = {
        {"len", std::make_shared<Builtin>(len)},
        {"first", std::make_shared<Builtin>(first)},
        {"last", std::make_shared<Builtin>(last)},
        {"rest", std::make_shared<Builtin>(rest)},
        {"push", std::make_shared<Builtin>(push)},
        {"puts", std::make_shared<Builtin>(puts)}
    };

    std::shared_ptr<Builtin> getBuiltin(const std::string& name) {
        return builtins[name];
    }
};
