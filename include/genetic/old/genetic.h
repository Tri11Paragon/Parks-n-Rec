//
// Created by brett on 7/11/23.
//

#ifndef PARKSNREC_GENETIC_H
#define PARKSNREC_GENETIC_H

#include <genetic/old/operators.h>
#include <imgui.h>
#include <ImNodes.h>
#include "ImNodesEz.h"
#include <queue>
#include <blt/profiling/profiler.h>
#include <unordered_set>

namespace parks::genetic {
    
    class InvalidTreeException : public std::runtime_error {
        public:
            explicit InvalidTreeException(const std::string& message): std::runtime_error(message) {}
    };
    
    class OperatorSet {
        private:
            std::unordered_set<Operators> hasOperators;
            std::vector<Operator*> operators;
            template <typename E>
            constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
                return static_cast<typename std::underlying_type<E>::type>(e);
            }
            void set(Operators code, Operator* op){
                operators[to_underlying(code)] = (op);
                hasOperators.insert(code);
            }
        public:
            OperatorSet() {
                operators.reserve(sizeof(operatorInfo) / sizeof(OperatorProperties));
                set(Operators::X, new XOperator);
                set(Operators::Y, new YOperator);
                set(Operators::RandScalar, new RandomScalarOperator);
                set(Operators::RandColor, new RandomColorOperator);
                
                set(Operators::Multiplication, new MultiplicationOperator);
                set(Operators::Addition, new AdditionOperator);
                set(Operators::Subtraction, new SubtractionOperator);
                set(Operators::Modulo, new ModOperator);
                set(Operators::Min, new MinOperator);
                set(Operators::Max, new MaxOperator);
                set(Operators::Log, new LogOperator);
                set(Operators::PerlinBW, new PerlinBWOperator);
                set(Operators::PerlinColor, new PerlinColorOperator);
                set(Operators::PerlinRidge, new PerlinRidgeOperator);
                set(Operators::PerlinFBM, new PerlinFBMOperator);
                set(Operators::PerlinTurbulence, new PerlinTurbulenceOperator);
                set(Operators::ColorNoise, new ColorNoiseOperator);
            };
            
            OperatorSet(OperatorSet& set1, OperatorSet& set2){
                operators.reserve(sizeof(operatorInfo) / sizeof(OperatorProperties));
                for (size_t i = 0; i < operators.size(); i++)
                    set((Operators)i, operators[i]->breed(set2[i]));
                
            }
            
            OperatorSet& mutate(){
                for (auto& o : operators)
                    if (chance(50))
                        o->mutate();
            }
            
            Operator* operator[](size_t index) const {
                return operators[index];
            }
            
            [[nodiscard]] int randomOperator(){
                auto begin = hasOperators.begin();
                std::advance(begin, randomInt(0, (int)hasOperators.size()));
                return to_underlying(*begin);
            }
            
            [[nodiscard]] int randomBaseOperator() {
                const Operators base[] = {
                    Operators::X,
                    Operators::Y,
                    //Operators::RandColor,
                    //Operators::RandScalar
                };
                return to_underlying(base[randomInt(0, sizeof(base) / sizeof(Operators))]);
            }
            
            ~OperatorSet(){
                for (Operator* op : operators)
                    delete op;
            }
    };
    
    class Program;
    
    class Node {
        private:
            friend Program;
            Node* left;
            Node* right;
            OperatorSet& set;
            int op = 0;
        public:
            /**
             * Gives ownership of the left / right node to this node!
             * @param left left subtree
             * @param right right subtree
             * @param op operator to apply
             */
            Node(OperatorSet& set, int op, Node* left = nullptr, Node* right = nullptr): left(left), right(right), set(set), op(op) {
                if (op == -1)
                    throw InvalidTreeException("Node operator must be assigned!");
            }
            
            /**
             * Applies the subtree operators by supplying the x / y / time params to the left and right subtrees then using the results in the operator
             * @param x current x in the image
             * @param y current y in the image
             * @param time current time
             * @return operator value
             */
            Color apply(double x, double y, unsigned int time){
                Color left_v(0);
                Color right_v(0);
                int argCount = 0;
                if (left != nullptr) {
                    left_v = left->apply(x, y, time);
                    argCount |= 0x2;
                }
                if (right != nullptr) {
                    argCount |= 0x1;
                    right_v = right->apply(x, y, time);
                }
                
                OperatorArguments args{x, y, time, argCount, left_v, right_v};

                return set[op]->apply(args);;
            }
            
            ~Node() {
                delete left;
                delete right;
            }
    };
    
    class Program {
        private:
            struct ImNode_t
            {
                int depth;
                size_t index;
                int opCode;
                ImVec2 pos{};
                bool selected{};
                ImNodes::Ez::SlotInfo inputs[1];
                ImNodes::Ez::SlotInfo outputs[2];
            };
            
            int height = 0;
            Node* root;
            OperatorSet set;
            std::vector<ImNode_t> nodes;
            
            Node* constructLeaf(){
                return new Node(set, set.randomBaseOperator(), nullptr, nullptr);
            }
            
            Node* constructTree(int depth) {
                // TODO: remove magic numbers!
                if (depth == 0)
                    return constructLeaf();
                if (chance(5))
                    return constructLeaf();
                auto opcode = set.randomOperator();
                auto acceptsInput = operatorInfo[opcode].acceptsInput;
                auto hasLeftSubtree = acceptsInput & 0x2;
                auto hasRightSubtree = acceptsInput & 0x1;
                return new Node(set, opcode, hasLeftSubtree ? constructTree(depth - 1) : nullptr,
                                hasRightSubtree ? constructTree(depth - 1) : nullptr);
            }
            
            Node* copyTree(Node* root) {
                Node* left = nullptr;
                Node* right = nullptr;
                if (root->left != nullptr)
                    left = copyTree(root->left);
                if (root->right != nullptr)
                    right = copyTree(root->right);
                return new Node(set, root->op, left, right);
            }
            
            int heightInternal(Node* parent){
                if (parent == nullptr)
                    return 1;
                int left = heightInternal(parent->left);
                int right = heightInternal(parent->right);
                return std::max(left, right) + 1;
            }
            
            void constructNodeList() {
                if (root == nullptr)
                    return;
                
                std::deque<Node*> nodesQueue;
                nodesQueue.push_back(root);
                
                int depth = 0;
                while (!nodesQueue.empty()) {
                    auto levelSize = nodesQueue.size();
                    auto currentLevelSize = nodesQueue.size();
                    while (currentLevelSize-- != 0){
                        auto node = nodesQueue.front();
                        nodesQueue.pop_front();
                        if (node == nullptr)
                            continue;
                        
                        nodesQueue.push_back(node->left);
                        nodesQueue.push_back(node->right);
                        
                        ImNode_t newNode;
                        
                        newNode.depth = depth;
                        newNode.index = currentLevelSize;
                        newNode.opCode = node->op;
                        newNode.pos = {static_cast<float>(250 * depth), static_cast<float>(100 * currentLevelSize)};
                        newNode.inputs[0] = {"In", 1};
                        newNode.outputs[0] = {"Left", 1};
                        newNode.outputs[1] = {"Right", 1};
                        newNode.selected = false;
                        
                        nodes.push_back(newNode);
                    }
                    depth++;
                }
            }
            Program(Program* program1, Program* program2){
                set = OperatorSet{program1->set, program2->set};
                
                root = copyTree(program1->root);
                
                // 1 point crossover for now
                
                Node* search = root;
                Node* parent = root;
                if (chance(50))
                    search = search->left;
                else
                    search = search->right;
                while (true){
                    if (search->left != nullptr) {
                        if (chance(50)) {
                            parent = search;
                            search = search->left;
                        }
                    }
                    if (search->right != nullptr) {
                        if (chance(50)) {
                            parent = search;
                            search = search->right;
                        }
                    }
                    if ((chance(25) && search != parent) || (search->left == nullptr && search->right == nullptr))
                        break;
                }
                
                Node* replace = program2->root;
                while (true){
                    if (replace->left != nullptr) {
                        if (chance(50))
                            replace = replace->left;
                    }
                    if (replace->right != nullptr) {
                        if (chance(50))
                            replace = replace->right;
                    }
                    if (chance(25) || (replace->left == nullptr && replace->right == nullptr))
                        break;
                }
                
                if (parent->left == search){
                    delete parent->left;
                    parent->left = copyTree(replace);
                } else {
                    delete parent->right;
                    parent->right = copyTree(replace);
                }
                
                constructNodeList();
            }
        public:
            Program(){
                root = constructTree(7);
                //root = new Node(set, (int)Operators::ColorNoise, nullptr, nullptr);
//                root = new Node(set, (int)Operators::Multiplication, new Node(set, (int)Operators::X,
//                                                                        nullptr, nullptr), new Node(set, (int)Operators::Y,
//                                                                                                    nullptr, nullptr));
//                root = new Node(set, (int)Operators::PerlinColor, new Node(set, (int)Operators::X,
//                                                                           nullptr, nullptr), new Node(set, (int)Operators::Y,
//                                                                                                       nullptr,
//                                                                                                       nullptr));
                constructNodeList();
//                root = new Node(set, 12, new Node(set, 5, new Node(set, 16, nullptr, nullptr), new Node(set, 0,
//                                                                                                        nullptr,
//                                                                                                        nullptr)),
//                                new Node(set, 5, new Node(set, 16, nullptr, nullptr), new Node(set, 1,
//                                                                                               nullptr,
//                                                                                               nullptr)));
            }
            
            void mutate(){
                set.mutate();
            }
            
            Program* crossover(Program* program){
                return new Program(this, program);
            }
            
            int getHeight(){
                if (height != 0)
                    return height;
                height = heightInternal(root);
            }
            
            void drawTree(){
                static ImNodes::Ez::Context* context = ImNodes::Ez::CreateContext();
                IM_UNUSED(context);
                
                if (ImGui::Begin("ImNodes", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
                {
                    ImNodes::Ez::BeginCanvas();
                    
                    for (ImNode_t& node : nodes)
                    {
                        if (ImNodes::Ez::BeginNode(&node, ("D: " + std::to_string(node.depth) + " | " + std::to_string(node.index) + " : " + operatorInfo[node.opCode].opCode).c_str(), &node.pos, &node.selected))
                        {
                            ImNodes::Ez::InputSlots(node.inputs, 1);
                            ImNodes::Ez::OutputSlots(node.outputs, 2);
                            ImNodes::Ez::EndNode();
                        }
                    }
                    
                    //ImNodes::Connection(&nodes[1], "In", &nodes[0], "Out");
                    //ImNodes::Connection(&nodes[2], "In", &nodes[0], "Out");
                    
                    ImNodes::Ez::EndCanvas();
                }
                ImGui::End();
            }
            
            Color apply(double x, double y, unsigned int time){
                return root->apply(x, y, time);
            }
            
            ~Program(){
                delete root;
            }
    };
    
    inline unsigned char filter(double input){
        return (unsigned char) ((unsigned long)std::abs(((input) * 255)) % 255);
    }
    
}

#endif //PARKSNREC_GENETIC_H
