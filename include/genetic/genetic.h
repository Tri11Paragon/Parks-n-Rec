//
// Created by brett on 7/11/23.
//

#ifndef PARKSNREC_GENETIC_H
#define PARKSNREC_GENETIC_H

#include <genetic/operators.h>
#include <imgui.h>
#include <ImNodes.h>
#include "ImNodesEz.h"
#include <queue>

namespace parks::genetic {
    
    class InvalidTreeException : public std::runtime_error {
        public:
            explicit InvalidTreeException(const std::string& message): std::runtime_error(message) {}
    };
    
    class OperatorSet {
        private:
            std::vector<Operator*> operators;
        public:
            OperatorSet() = default;
            
            void add(Operator* op){
                operators.push_back(op);
            }
            
            Operator* operator[](int index) const {
                return operators[index];
            }
            
            [[nodiscard]] size_t size() const {
                return operators.size();
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
                if (left != nullptr)
                    left_v = left->apply(x, y, time);
                if (right != nullptr)
                    right_v = right->apply(x, y, time);
                return set[op]->apply(x, y, time, left_v, right_v);
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
                return new Node(set, randomInt(0, 2), nullptr, nullptr);
            }
            
            Node* constructTree(int depth) {
                // TODO: remove magic numbers!
                if (depth == 0)
                    return constructLeaf();
                if (randomInt(0, 100) == 0)
                    return constructLeaf();
                return new Node(set, randomInt(0, (int)set.size()), constructTree(depth - 1),
                                constructTree(depth - 1));
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
        public:
            Program(){
                set.add(new XOperator);
                set.add(new YOperator);
                set.add(new ZeroOperator);
                set.add(new OneOperator);
                
                set.add(new MultiplicationOperator);
                set.add(new AdditionOperator);
                set.add(new SubtractionOperator);
//                set.add(new ModOperator);
//                set.add(new MinOperator);
//                set.add(new MaxOperator);
//                set.add(new LogOperator);
                set.add(new PerlinBWOperator);
                set.add(new PerlinColorOperator);
                set.add(new PerlinRidgeOperator);
                set.add(new PerlinFBMOperator);
                set.add(new PerlinTurbulenceOperator);
                set.add(new ColorNoiseOperator);
                
                //auto numberOfNodes = randomInt(10, 10);
                
                root = constructTree(7);
                
                constructNodeList();
//                root = new Node(set, 12, new Node(set, 5, new Node(set, 16, nullptr, nullptr), new Node(set, 0,
//                                                                                                        nullptr,
//                                                                                                        nullptr)),
//                                new Node(set, 5, new Node(set, 16, nullptr, nullptr), new Node(set, 1,
//                                                                                               nullptr,
//                                                                                               nullptr)));
            
            
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
                        if (ImNodes::Ez::BeginNode(&node, ("D: " + std::to_string(node.depth) + " | " + std::to_string(node.index) + " : " + set[node.opCode]->opString).c_str(), &node.pos, &node.selected))
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
