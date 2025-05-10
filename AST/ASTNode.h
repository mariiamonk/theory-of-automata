#ifndef LAB2_ASTNODE_H
#define LAB2_ASTNODE_H

#include <string>
#include <memory>
#include <set>
#include <vector>
#include <iostream>

namespace AbstractTree {

    class ASTNode {
    public:
        ASTNode() = default;

        virtual bool Nullable() const = 0;
        virtual std::set<size_t> Firstpos() const = 0;
        virtual std::set<size_t> Lastpos() const = 0;

        virtual std::shared_ptr<ASTNode> copy() const = 0;
        virtual void print(std::string prefix) const = 0;

        virtual void printTree(const std::string& prefix = "", bool isLast = true) const = 0;

        virtual ~ASTNode() = default;
    };

}

#endif //LAB2_ASTNODE_H
