#ifndef LAB2_OPERATIONS_H
#define LAB2_OPERATIONS_H

#include "../AST/ASTNode.h"

namespace AbstractTree {

    class OperationNode : public ASTNode {
    protected:
        bool nullable;
        std::set<size_t> firstpos;
        std::set<size_t> lastpos;

    public:
        OperationNode() = default;

        virtual void calculateNFL() = 0;

        virtual const std::vector<std::shared_ptr<ASTNode>> getArguments() const = 0;
        virtual std::string getOperation() const = 0;

        void print(std::string prefix) const override;
        void printTree(const std::string& prefix = "", bool isLast = true) const override;

        bool Nullable() const override {
            return nullable;
        }

        std::set<size_t> Firstpos() const override {
            return firstpos;
        }

        std::set<size_t> Lastpos() const override {
            return lastpos;
        }
    };

    class Or : public OperationNode {
    private:
        std::shared_ptr<ASTNode> left;
        std::shared_ptr<ASTNode> right;
    public:
        Or(std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right);

        void calculateNFL() override;

        std::string getOperation() const override;
        const std::vector<std::shared_ptr<ASTNode>> getArguments() const override;

        std::shared_ptr<ASTNode> copy() const override;
    };

    class Concatenation : public OperationNode {
    private:
        std::shared_ptr<ASTNode> left;
        std::shared_ptr<ASTNode> right;
    public:
        Concatenation(std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right);

        void calculateNFL() override;

        std::string getOperation() const override;
        const std::vector<std::shared_ptr<ASTNode>> getArguments() const override;

        std::shared_ptr<ASTNode> copy() const override;
    };

    class KliniClosure : public OperationNode {
    private:
        std::shared_ptr<ASTNode> left;
    public:
        KliniClosure(std::shared_ptr<ASTNode> left);

        void calculateNFL() override;

        std::string getOperation() const override;
        const std::vector<std::shared_ptr<ASTNode>> getArguments() const override;

        std::shared_ptr<ASTNode> copy() const override;
    };

    class PositiveClosure : public OperationNode {
    private:
        std::shared_ptr<ASTNode> left;
    public:
        PositiveClosure(std::shared_ptr<ASTNode> left);

        void calculateNFL() override;

        std::string getOperation() const override;
        const std::vector<std::shared_ptr<ASTNode>> getArguments() const override;

        std::shared_ptr<ASTNode> copy() const override;
    };

    class Optional : public OperationNode {
    private:
        std::shared_ptr<ASTNode> left;
    public:
        Optional(std::shared_ptr<ASTNode> left);

        void calculateNFL() override;

        std::string getOperation() const override;
        const std::vector<std::shared_ptr<ASTNode>> getArguments() const override;

        std::shared_ptr<ASTNode> copy() const override;
    };

    class CatchGroup : public OperationNode {
    private:
        std::shared_ptr<ASTNode> left;
        std::string name;
    public:
        CatchGroup(const std::string& name, std::shared_ptr<ASTNode> left);

        void calculateNFL() override;

        std::string getOperation() const override;
        const std::vector<std::shared_ptr<ASTNode>> getArguments() const override;

        std::string getName() const;

        std::shared_ptr<ASTNode> copy() const override;
    };

    class AnySymbol : public OperationNode {
    private:
        size_t number = 0;
    public:
        AnySymbol() : OperationNode() {}
        std::shared_ptr<ASTNode> copy() const override {
            return std::make_shared<AnySymbol>(*this);
        }

        void calculateNFL() override {
            // Логика для AnySymbol
            firstpos = {number};
            lastpos = {number};
            nullable = false;
        }

        const std::vector<std::shared_ptr<ASTNode>> getArguments() const override {
            return {}; // AnySymbol не имеет аргументов
        }

        std::string getOperation() const override {
            return "."; // Возвращает метасимвол
        }
        bool Nullable() const override {
            return false;
        }

        std::set<size_t> Firstpos() const override {
            if (number == 0) throw std::logic_error("Node not enumerated");
            return {number};
        }

        std::set<size_t> Lastpos() const override {
            if (number == 0) throw std::logic_error("Node not enumerated");
            return {number};
        }

        void printTree(const std::string& prefix, bool isLast) const override {
            std::cout << prefix << (isLast ? "└─ " : "├─ ") << "Operation: ." << std::endl;
        }
    };
}

#endif //LAB2_OPERATIONS_H
