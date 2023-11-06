#pragma once

#include <string>
#include <functional>
#include "ast.h"

namespace monkey {
    using modifierFunc = std::function<std::shared_ptr<Node>(std::shared_ptr<Node>)>;

    std::shared_ptr<Node> modify(std::shared_ptr<Node> node, modifierFunc modifier) {
        if (std::dynamic_pointer_cast<Program>(node)) {
            auto program = std::dynamic_pointer_cast<Program>(node);
            for (auto& stmt : program->statements) {
                stmt = std::dynamic_pointer_cast<Statement>(modify(stmt, modifier));
            }
        } 
        else if (std::dynamic_pointer_cast<ExpressionStatement>(node)) {
            auto stmt = std::dynamic_pointer_cast<ExpressionStatement>(node);
            stmt->expression = std::dynamic_pointer_cast<Expression>(modify(stmt->expression, modifier));
        } 
        else if (std::dynamic_pointer_cast<InfixExpression>(node)) {
            auto expr = std::dynamic_pointer_cast<InfixExpression>(node);
            expr->left = std::dynamic_pointer_cast<Expression>(modify(expr->left, modifier));
            expr->right = std::dynamic_pointer_cast<Expression>(modify(expr->right, modifier));
        } 
        else if (std::dynamic_pointer_cast<PrefixExpression>(node)) {
            auto expr = std::dynamic_pointer_cast<PrefixExpression>(node);
            expr->right = std::dynamic_pointer_cast<Expression>(modify(expr->right, modifier));
        } 
        else if (std::dynamic_pointer_cast<IndexExpression>(node)) {
            auto expr = std::dynamic_pointer_cast<IndexExpression>(node);
            expr->left = std::dynamic_pointer_cast<Expression>(modify(expr->left, modifier));
            expr->index = std::dynamic_pointer_cast<Expression>(modify(expr->index, modifier));
        } 
        else if (std::dynamic_pointer_cast<IfExpression>(node)) {
            auto expr = std::dynamic_pointer_cast<IfExpression>(node);
            expr->condition = std::dynamic_pointer_cast<Expression>(modify(expr->condition, modifier));
            expr->consequence = std::dynamic_pointer_cast<BlockStatement>(modify(expr->consequence, modifier));
            if (expr->alternative) {
                expr->alternative = std::dynamic_pointer_cast<BlockStatement>(modify(expr->alternative, modifier));
            }
        } 
        else if (std::dynamic_pointer_cast<BlockStatement>(node)) {
            auto block = std::dynamic_pointer_cast<BlockStatement>(node);
            for (auto& stmt : block->statements) {
                stmt = std::dynamic_pointer_cast<Statement>(modify(stmt, modifier));
            }
        } 
        else if (std::dynamic_pointer_cast<ReturnStatement>(node)) {
            auto stmt = std::dynamic_pointer_cast<ReturnStatement>(node);
            stmt->returnValue = std::dynamic_pointer_cast<Expression>(modify(stmt->returnValue, modifier));
        } 
        else if (std::dynamic_pointer_cast<LetStatement>(node)) {
            auto stmt = std::dynamic_pointer_cast<LetStatement>(node);
            stmt->value = std::dynamic_pointer_cast<Expression>(modify(stmt->value, modifier));
        } 
        else if (std::dynamic_pointer_cast<FunctionLiteral>(node)) {
            auto lit = std::dynamic_pointer_cast<FunctionLiteral>(node);
            for (auto& param : lit->parameters) {
                param = std::dynamic_pointer_cast<Identifier>(modify(param, modifier));
            }
            lit->body = std::dynamic_pointer_cast<BlockStatement>(modify(lit->body, modifier));
        } else if (std::dynamic_pointer_cast<ArrayLiteral>(node)) {
            auto lit = std::dynamic_pointer_cast<ArrayLiteral>(node);
            for (auto& elem : lit->elements) {
                elem = std::dynamic_pointer_cast<Expression>(modify(elem, modifier));
            }
        } else if (std::dynamic_pointer_cast<HashLiteral>(node)) {
            std::map<std::shared_ptr<Expression>, std::shared_ptr<Expression>> newPairs;
            auto lit = std::dynamic_pointer_cast<HashLiteral>(node);
            for (auto& pair : lit->pairs) {
                auto newKey = std::dynamic_pointer_cast<Expression>(modify(pair.first, modifier));
                auto newValue = std::dynamic_pointer_cast<Expression>(modify(pair.second, modifier));
                newPairs.insert(std::make_pair(newKey, newValue));
            }
            lit->pairs = newPairs; 
        }  
        return modifier(node); 
        }
} // namespace monkey
