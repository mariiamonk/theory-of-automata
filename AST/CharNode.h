#ifndef LAB2_CHARNODE_H
#define LAB2_CHARNODE_H

#include "../AST/ASTNode.h"

namespace AbstractTree {

    class CharNode : public ASTNode {
    private:
        char smb = '\0';
        size_t number = 0;
        std::string groupRef;
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

        void setGroupReference(const std::string& name) {
            groupRef = name;
            number = 0;
        }
        const std::string& getGroupReference() const { return groupRef; }

        bool isGroupReference() const { return !groupRef.empty(); }

        [[nodiscard]] std::shared_ptr<ASTNode> deepCopy() const {
            auto copy = std::make_shared<CharNode>(smb);
            if (isGroupReference()) {
                copy->setGroupReference(groupRef);
            }
            copy->number = number;
            return copy;
        }
    };
}
#endif //LAB2_CHARNODE_H
