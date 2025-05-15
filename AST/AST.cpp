#include <queue>
#include "AST.h"
#define ESCSMB '&'

namespace AbstractTree {
    static std::set<char> specialSymbols = {'-', '|', '+', '*','(', ')', '|', '?', '{', '}', '.', '<'};

    bool isSMB(char smb) {
        return ' ' <= smb && smb <= '~';
    }
    bool isSpecialSMB(char smb) {
        return specialSymbols.contains(smb);
    }

    void pushToResult(std::stack<std::shared_ptr<ASTNode>>& resultStack, StackNode* operation) {
        size_t argumentsCount = operation->getArgsCount();
        if (argumentsCount == 0) {
            return;
        }

        if (resultStack.size() < argumentsCount) {
            throw std::runtime_error("Not enough arguments to operation");
        }

        std::vector<std::shared_ptr<ASTNode>> args(argumentsCount);
        for (int i = argumentsCount-1; i >= 0 ; i--) {
            args[i] = resultStack.top();
            resultStack.pop();
        }
        auto new_operation = operation->compile(args);
        resultStack.push(new_operation);
    }

    void addOperationToStack(std::stack<StackNode*>&operationsStack, std::stack<std::shared_ptr<ASTNode>>& resultStack, StackNode* operation) {
        if (dynamic_cast<Bracket*>(operation) == nullptr){
            auto priority = operation->getPriority();

            while (!(   operationsStack.empty()
                        || dynamic_cast<Bracket*>(operationsStack.top()) != nullptr
                        || operationsStack.top()->getPriority() < priority)) {
                pushToResult(resultStack, operationsStack.top());
                delete operationsStack.top();
                operationsStack.pop();
            }
        }
        operationsStack.push(operation);
    }

    void AST::readGroupName(const std::string& expr, size_t& i,
                            std::stack<StackNode*>& operationsStack,
                            std::stack<std::shared_ptr<ASTNode>>& resultStack) {
        if (i + 1 >= expr.size() || expr[i+1] != '<') {
            addOperationToStack(operationsStack, resultStack, new Bracket());
            return;
        }

        size_t name_start = i + 2;
        size_t name_end = name_start;
        while (name_end < expr.size() && isalpha(expr[name_end])) name_end++;

        if (name_end >= expr.size() || expr[name_end] != '>') {
            addOperationToStack(operationsStack, resultStack, new Bracket());
            return;
        }

        std::string name = expr.substr(name_start, name_end - name_start);
        if (groups.count(name))
            throw std::runtime_error("Multiple declaration of capture groups");

        groups.insert(name);
        size_t stackSizeBefore = resultStack.size();
        addOperationToStack(operationsStack, resultStack, new StackCatchGroupNode(name));
        i = name_end;

        if (resultStack.size() > stackSizeBefore) {
            groupDefinitions[name] = resultStack.top();
        }
    }

    AST::AST(const std::string &expr) {
        std::stack<StackNode*> operationsStack;
        std::stack<std::shared_ptr<ASTNode>> resultStack;

        bool escaping = false;
        bool begin_of_token = true;

        for (size_t i = 0; i < expr.size(); i++) {
            auto current = expr[i];
            if (!isSMB(current))
                throw std::runtime_error("Non printable symbol");

            if (current == ESCSMB) {
                escaping = !escaping;
                continue;
            }

            if (escaping) {
                if (!begin_of_token) {
                    addOperationToStack(operationsStack, resultStack, new StackConcatinationNode());
                }
                resultStack.push(std::make_shared<CharNode>(current));
                escaping = false;
            } else {
                switch (current) {
                    case '.':
                        if (!begin_of_token) {
                            addOperationToStack(operationsStack, resultStack, new StackConcatinationNode());
                        }
                        resultStack.push(std::make_shared<AnySymbol>());
                        break;
                    case '*':
                        addOperationToStack(operationsStack, resultStack, new StackKliniClosureNode());
                        break;
                    case '-':
                        addOperationToStack(operationsStack, resultStack, new StackConcatinationNode());
                        begin_of_token = true;
                        continue;
                    case '+':
                        addOperationToStack(operationsStack, resultStack, new StackPositiveClosureNode());
                        break;
                    case '?':
                        addOperationToStack(operationsStack, resultStack, new StackOptionalNode());
                        break;
                    case '|':
                        addOperationToStack(operationsStack, resultStack, new StackOrNode());
                        begin_of_token = true;
                        continue;
                    case '<': {
                        size_t name_start = i + 1;
                        size_t name_end = name_start;
                        while (name_end < expr.size() && isalpha(expr[name_end])) name_end++;

                        if (name_end < expr.size() && expr[name_end] == '>' && name_end > name_start) {
                            std::string name = expr.substr(name_start, name_end - name_start);

                            if (i > 0 && expr[i-1] == '(') {
                                if (groups.count(name))
                                    throw std::runtime_error("Multiple declaration of capture groups");

                                groups.insert(name);
                                addOperationToStack(operationsStack, resultStack, new StackCatchGroupNode(name));
                                i = name_end;
                                begin_of_token = true;
                                continue;
                            }
                            else {
                                if (!groupDefinitions.count(name))
                                    throw std::runtime_error("Group definition not found for: " + name);

                                if (!begin_of_token)
                                    addOperationToStack(operationsStack, resultStack, new StackConcatinationNode());

                                resultStack.push(copySubtree(groupDefinitions[name]));
                                i = name_end;
                                begin_of_token = false;
                                continue;
                            }
                        }
                        if (!begin_of_token)
                            addOperationToStack(operationsStack, resultStack, new StackConcatinationNode());
                        resultStack.push(std::make_shared<CharNode>(current));
                        break;
                    }
                    case '(':
                        if (!begin_of_token)
                            addOperationToStack(operationsStack, resultStack, new StackConcatinationNode());
                        readGroupName(expr, i, operationsStack, resultStack);
                        begin_of_token = true;
                        continue;
                    case ')':
                        if (begin_of_token)
                            throw std::runtime_error("Empty brackets");

                        while (!operationsStack.empty() &&
                               dynamic_cast<Bracket*>(operationsStack.top()) == nullptr) {
                            pushToResult(resultStack, operationsStack.top());
                            delete operationsStack.top();
                            operationsStack.pop();
                        }

                        if (operationsStack.empty())
                            throw std::runtime_error("Wrong brackets sequence (too many closing)");

                        pushToResult(resultStack, operationsStack.top());
                        delete operationsStack.top();
                        operationsStack.pop();

                        if (!resultStack.empty()) {
                            auto groupNode = std::dynamic_pointer_cast<CatchGroup>(resultStack.top());
                            if (groupNode) {
                                groupDefinitions[groupNode->getName()] = groupNode->getArguments()[0];
                            }
                        }

                        break;

                    case '{': {
                        size_t i_ = i + 1;
                        while (i_ < expr.size() && isdigit(expr[i_])) ++i_;

                        int min = 0, max = -1;
                        if (i_ < expr.size() && expr[i_] == ',') {
                            min = std::stoi(expr.substr(i + 1, i_ - i - 1));
                            size_t comma_pos = i_;
                            i_++;
                            while (i_ < expr.size() && isdigit(expr[i_])) ++i_;

                            if (i_ > comma_pos + 1) {
                                max = std::stoi(expr.substr(comma_pos + 1, i_ - comma_pos - 1));
                            }
                        } else if (i_ > i + 1) {
                            min = max = std::stoi(expr.substr(i + 1, i_ - i - 1));
                        }

                        if (!(i_ < expr.size()) || expr[i_] != '}')
                            throw std::runtime_error("Wrong range format");

                        addOperationToStack(operationsStack, resultStack, new StackRangeRepeatNode(min, max));
                        i = i_;
                        break;
                    }
                    default:
                        if (isSpecialSMB(current))
                            throw std::runtime_error("Wrong using of metasymbol");
                        if (!begin_of_token)
                            addOperationToStack(operationsStack, resultStack, new StackConcatinationNode());
                        resultStack.push(std::make_shared<CharNode>(current));
                        break;
                }
            }
            begin_of_token = false;
        }

        if (escaping)
            throw std::runtime_error("Escaping isn't closed");

        while (!operationsStack.empty()) {
            auto c = operationsStack.top();
            if (dynamic_cast<Bracket*>(c) != nullptr)
                throw std::runtime_error("Wrong brackets sequence (too many opening)");
            pushToResult(resultStack, c);
            delete c;
            operationsStack.pop();
        }
        root = resultStack.top();
    }

    size_t AST::calculateNode(ASTNode* node, size_t num) {
        if (auto* charNode = dynamic_cast<CharNode*>(node)) {
            if (charNode->isGroupReference()) {
                return num;
            }

            if (!charNode->Nullable()) {
                charNode->enumerate(num);
                enumData.folowPos.resize(num);
                enumData.character_index[charNode->getValue()].insert(num);
                return num + 1;
            }
            return num;
        }
        else if (auto* opNode = dynamic_cast<OperationNode*>(node)) {
            for (auto& arg : opNode->getArguments()) {
                num = calculateNode(arg.get(), num);
            }
            opNode->calculateNFL();

            if (auto* concat = dynamic_cast<Concatenation*>(opNode)) {
                auto args = opNode->getArguments();
                for (auto last : args[0]->Lastpos()) {
                    auto first = args[1]->Firstpos();
                    enumData.folowPos[last-1].insert(first.begin(), first.end());
                }
            }
            else if (auto* klini = dynamic_cast<KliniClosure*>(opNode)) {
                auto args = opNode->getArguments();
                for (auto last : args[0]->Lastpos()) {
                    auto first = args[0]->Firstpos();
                    enumData.folowPos[last-1].insert(first.begin(), first.end());
                }
            }
            else if (auto* group = dynamic_cast<CatchGroup*>(opNode)) {
                enumData.groupsData[group->getName()].beginWith = group->Firstpos();
                enumData.groupsData[group->getName()].endWith = group->Lastpos();

                std::set<size_t> visited;
                std::queue<size_t> queue;
                for (size_t pos : group->Firstpos()) queue.push(pos);

                while (!queue.empty()) {
                    size_t current = queue.front();
                    queue.pop();

                    if (visited.count(current)) continue;
                    visited.insert(current);

                    if (!group->Firstpos().count(current) && !group->Lastpos().count(current)) {
                        enumData.groupsData[group->getName()].insideIn.insert(current);
                    }

                    for (size_t next : enumData.folowPos[current - 1]) {
                        if (!visited.count(next) && !group->Lastpos().count(next)) {
                            queue.push(next);
                        }
                    }
                }
            }
            return num;
        }
        throw std::runtime_error("Error of calculate Node");
    }

    std::shared_ptr<ASTNode> AST::copySubtree(const std::shared_ptr<ASTNode>& node) const {
        if (!node) return nullptr;
        return node->deepCopy();
    }

    void AST::print() const {
        if (root == nullptr)
            std::cout <<"empty" <<std::endl;
        else
            root->printTree();
    }

    AstEnumData AST::getEnumData() const {
        return enumData;
    }

    AST::AST() {

    }
}