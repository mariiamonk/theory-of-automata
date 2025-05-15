#ifndef LAB2_AST_H
#define LAB2_AST_H

#include "StackNode.h"
#include "Operations.h"
#include "CharNode.h"
#include "ASTNode.h"
#include <iostream>
#include <map>
#include <stack>

namespace AbstractTree {
    bool isSMB(char smb);
    bool isSpecialSMB(char smb);

    struct GroupEnumData {
        std::set<size_t> beginWith;
        std::set<size_t> insideIn;
        std::set<size_t> endWith;
    };

    struct AstEnumData {
        std::map<char, std::set<size_t>> character_index;
        std::vector<std::set<size_t>> folowPos;
        std::set<size_t> rootFirstpos;
        std::map<std::string, GroupEnumData> groupsData;
    };

    class AST {
    private:
        std::shared_ptr<ASTNode> root;

        AstEnumData enumData;

        std::set<std::string> groups;
        std::map<std::string, GroupEnumData> grpEnumData;
        std::map<std::string, std::shared_ptr<ASTNode>> groupDefinitions;
        std::shared_ptr<ASTNode> copySubtree(const std::shared_ptr<ASTNode>& node) const;

        void readGroupName( const std::string& expr, size_t& i,
                            std::stack<StackNode*>&operationsStack,
                            std::stack<std::shared_ptr<ASTNode>>& resultStack);

        size_t calculateNode(ASTNode* node, size_t num);

    public:
        AST(const std::string& expr);

        void calculateFollowpos();

        void print() const;

        AstEnumData getEnumData() const;

        std::shared_ptr<ASTNode> getRoot() const { return root; }

        AST();
    };

} // AbstractTree

#endif //LAB2_AST_H
