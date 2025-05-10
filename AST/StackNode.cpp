#include "StackNode.h"
#include "CharNode.h"

namespace AbstractTree {
    std::shared_ptr<OperationNode> StackOrNode::compile(std::vector<std::shared_ptr<ASTNode>> args) {
        if (args.size() != getArgsCount())
            throw std::invalid_argument("wrong count of operands");
        return std::make_shared<Or>(args[0], args[1]);
    }

    std::shared_ptr<OperationNode> StackConcatinationNode::compile(std::vector<std::shared_ptr<ASTNode>> args) {
        if (args.size() != getArgsCount())
            throw std::invalid_argument("wrong count of operands");
        return std::make_shared<Concatenation>(args[0], args[1]);
    }

    std::shared_ptr<OperationNode> StackKliniClosureNode::compile(std::vector<std::shared_ptr<ASTNode>> args) {
        if (args.size() != getArgsCount())
            throw std::invalid_argument("wrong count of operands");
        return std::make_shared<KliniClosure>(args[0]);
    }

    std::shared_ptr<OperationNode> StackPositiveClosureNode::compile(std::vector<std::shared_ptr<ASTNode>> args) {
        if (args.size() != getArgsCount())
            throw std::invalid_argument("wrong count of operands");
        return std::make_shared<Concatenation>(args[0], std::make_shared<KliniClosure>(args[0]));
    }

    std::shared_ptr<OperationNode> Bracket::compile(std::vector<std::shared_ptr<ASTNode>> args) {
        return nullptr;
    }

    std::shared_ptr<OperationNode> StackCatchGroupNode::compile(std::vector<std::shared_ptr<ASTNode>> args) {
        if (args.size() != getArgsCount())
            throw std::invalid_argument("wrong count of operands");
        return std::make_shared<CatchGroup>(name, args[0]);
    }

    std::shared_ptr<OperationNode> StackOptionalNode::compile(std::vector<std::shared_ptr<ASTNode>> args) {
        if (args.size() != getArgsCount())
            throw std::invalid_argument("wrong count of operands");
        return std::make_shared<Optional>(args[0]);
    }

    std::shared_ptr<OperationNode> StackRangeRepeatNode::compile(std::vector<std::shared_ptr<ASTNode>> args) {
        if (args.size() != getArgsCount())
            throw std::invalid_argument("wrong count of operands");
        auto baseNode = args[0];

        if (min == 0 && max == -1) return std::make_shared<KliniClosure>(baseNode);
        if (min == 1 && max == -1) return std::make_shared<PositiveClosure>(baseNode);
        if (min == 0 && max == 1) return std::make_shared<Or>(std::make_shared<CharNode>('\0'), baseNode);

        std::shared_ptr<ASTNode> result = baseNode;
        for (int i = 1; i < min; ++i) {
            result = std::make_shared<Concatenation>(result, baseNode->copy());
        }

        if (max > min || max == -1) {
            std::shared_ptr<ASTNode> optional;
            if (max == -1) {
                optional = std::make_shared<KliniClosure>(baseNode->copy());
            } else {
                optional = std::make_shared<CharNode>('\0'); // ε
                for (int i = min; i < max; ++i) {
                    auto temp = baseNode->copy();
                    for (int j = 1; j <= i; ++j) {
                        temp = std::make_shared<Concatenation>(temp, baseNode->copy());
                    }
                    optional = std::make_shared<Or>(optional, temp);
                }
            }
            result = std::make_shared<Concatenation>(result, optional);
        }

        return std::dynamic_pointer_cast<OperationNode>(result);
    }
}