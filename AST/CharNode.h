#ifndef LAB2_CHARNODE_H
#define LAB2_CHARNODE_H

#include "../AST/ASTNode.h"

namespace AbstractTree {

    class CharNode : public ASTNode {
    private:
        char smb = '\0';
        size_t number = 0;
    public:
        CharNode() = default;
        CharNode(char smb);

        std::shared_ptr<ASTNode> copy() const override;

        void print(std::string prefix) const override;

        void printTree(const std::string& prefix = "", bool isLast = true) const override;

        bool Nullable() const override;
        std::set<size_t> Firstpos() const override;
        std::set<size_t> Lastpos() const override;

        void enumerate(size_t num);

        char getValue() const;
        size_t getNumber() const { return number; }
    };
}
#endif //LAB2_CHARNODE_H
