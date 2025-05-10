#ifndef LAB2_STACKNODE_H
#define LAB2_STACKNODE_H

#include "../AST/ASTNode.h"
#include "Operations.h"
#include "CharNode.h"

namespace AbstractTree {
    enum class Priority {
        BRACKET = 0,
        LOW,
        MEDIUM,
        HIGHT,
    };

    class StackNode {
    public:
        StackNode() = default;

        virtual Priority getPriority() const = 0;
        virtual size_t getArgsCount() const = 0;
        virtual std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) = 0;

        virtual ~StackNode() = default;
    };

    class StackOrNode : public StackNode {
    public:
        Priority getPriority() const override {return Priority::LOW;}
        size_t getArgsCount() const override {return 2;}

        std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) override;
    };

    class StackConcatinationNode : public StackNode {
    public:
        Priority getPriority() const override {return Priority::MEDIUM;}
        size_t getArgsCount() const override {return 2;}

        std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) override;
    };

    class StackKliniClosureNode : public StackNode {
    public:
        Priority getPriority() const override {return Priority::HIGHT;}
        size_t getArgsCount() const override {return 1;}

        std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) override;
    };

    class StackPositiveClosureNode : public StackNode {
    public:
        Priority getPriority() const override {return Priority::HIGHT;}
        size_t getArgsCount() const override {return 1;}

        std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) override;
    };

    class StackOptionalNode : public StackNode {
    public:
        Priority getPriority() const override {return Priority::HIGHT;}
        size_t getArgsCount() const override {return 1;}

        std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) override;
    };

    class Bracket : public StackNode {
    public:
        Priority getPriority() const override {return Priority::BRACKET;}
        size_t getArgsCount() const override {return 0;}

        std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) override;
    };

    class StackCatchGroupNode : public Bracket {
    private:
        std::string name;
    public:
        StackCatchGroupNode(const std::string& name): Bracket(), name(name) {}

        size_t getArgsCount() const override {return 1;}

        std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) override;
    };

    class StackRangeRepeatNode : public StackNode {
    private:
        int min;
        int max;
    public:
        StackRangeRepeatNode(int min, int max) : StackNode(), min(min), max(max) {}

        Priority getPriority() const override { return Priority::HIGHT; }
        size_t getArgsCount() const override { return 1; }

        std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) override;
    };

    class StackAnySymbolNode : public StackNode {
    public:
        Priority getPriority() const override { return Priority::HIGHT; }
        size_t getArgsCount() const override { return 0; }

        std::shared_ptr<OperationNode> compile(std::vector<std::shared_ptr<ASTNode>> args) override {
            return std::make_shared<AnySymbol>();
        }
    };
}

#endif //LAB2_STACKNODE_H
