#include "CharNode.h"

namespace AbstractTree {
    CharNode::CharNode(char smb) : ASTNode(), smb(smb) {}

    std::shared_ptr<ASTNode> CharNode::copy() const {
        return std::make_shared<CharNode>(smb);
    }

    void CharNode::print(std::string prefix) const {
        if (smb == '\0') {
            std::cout << prefix << "Empty" << std::endl;
        } else {
            std::cout << prefix << smb << std::endl;
        }
    }

    void CharNode::printTree(const std::string& prefix, bool isLast) const {
        std::cout << prefix << (isLast ? "└── " : "├── ") << "Char: " << smb << std::endl;
    }

    bool CharNode::Nullable() const {
        return smb == '\0';
    }

    std::set<size_t> CharNode::Firstpos() const {
        if (Nullable()) return {};
        return {number};
    }

    std::set<size_t> CharNode::Lastpos() const {
        if (Nullable()) return {};
        return {number};
    }

    void CharNode::enumerate(size_t num) {
        if (num == 0) {
            throw std::invalid_argument("Invalid node number");
        }
        number = num;
    }

    char CharNode::getValue() const {
        return smb;
    }
}