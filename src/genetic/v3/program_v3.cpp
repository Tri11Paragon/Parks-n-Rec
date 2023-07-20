//
// Created by brett on 7/18/23.
//
#include <genetic/v3/program_v3.h>
#include "imgui.h"
#include <queue>
#include <utility>

namespace parks::genetic {
    
    static inline int pow(int b, int e){
        auto o = 1;
        for (int i = 0; i < e; i++)
            o *= b;
        return o;
    }
    
    void Program::run() {
        if (ImGui::Button("Run Program")){
            processImage();
            regenTreeDisplay();
        }
        if (ImGui::Button("Regen Program And Run")) {
            delete tree;
            tree = new GeneticTree(7);
            regenTreeDisplay();
            
            processImage();
        }
        if (ImGui::Button("Crossover")){
        
        }
        if (ImGui::Button("Mutate")){
            tree->mutate();
        }
        if (ImGui::Button("Save")){
        
        }
        if (ImGui::Button("Revert")){
        
        }
        if (ImGui::Button("Rescale Color")){
        
        }
        if (ImGui::CollapsingHeader("Progress")) {
            ImGui::Text("Render Progress: ");
            ImGui::ProgressBar(getRenderProgress());
        }
    }
    
    void Program::regenTreeDisplay() {
        treeNodes = {};
        for (int i = 0; i < tree->getSize(); i++){
            auto node = tree->node(i);
            if (node == nullptr)
                continue;
            auto height = GeneticTree::height(i);
            ImNode_t n;
            
            n.height = height;
            n.id = node->op;
            n.index = i;
            n.selected = false;
            n.pos = {(float)i * 200, 0};
            n.inputs[0] = {"In", 1};
            n.outputs[0] = {"Left", 1};
            n.outputs[1] = {"Right", 1};
            
            treeNodes.push_back(n);
        }
    }
    
    void Program::draw() {
        static ImNodes::Ez::Context* context = ImNodes::Ez::CreateContext();
        IM_UNUSED(context);
        
        if (ImGui::Begin("ImNodes", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            ImNodes::Ez::BeginCanvas();
            
            for (ImNode_t& node : treeNodes)
            {
                if (ImNodes::Ez::BeginNode(&node, ("H: " + std::to_string(node.height) + " | I: " + std::to_string(node.index) + " : " + functions[node.id].name).c_str(), &node.pos, &node.selected))
                {
                    ImNodes::Ez::InputSlots(node.inputs, 1);
                    ImNodes::Ez::OutputSlots(node.outputs, 2);
                    ImNodes::Ez::EndNode();
                }
//                auto p = GeneticTree::parent(node.index);
//                auto parent = tree->node(p);
//                if (p >= 0 && parent != nullptr) {
//                    //BLT_TRACE("Parent i: %d, Parent %d for input node %d", p, parent, node.index);
//                    if (GeneticTree::left(p) == node.index)
//                        ImNodes::Connection(&node, "In", &parent, "Left");
//                    else
//                        ImNodes::Connection(&node, "In", &parent, "Right");
//                }
            }
            //ImNodes::Connection(&nodes[2], "In", &nodes[0], "Out");
            
            ImNodes::Ez::EndCanvas();
        }
        ImGui::End();
    }
    
    void Program::processImage() {
        for (unsigned int i = 0; i < WIDTH; i++) {
            for (unsigned int j = 0; j < HEIGHT; j++){
                auto pos = getPixelPosition(i, j);
                
                //auto out = functions[FunctionID::COLOR_NOISE].call({ARGS_BOTH, Color((double)i / WIDTH), Color((double)j / HEIGHT)}, set);
                
                auto out = tree->execute((double)i / WIDTH, (double)j / HEIGHT);
                
                pixels[pos] = (unsigned char)(out.r * 255);
                pixels[pos + 1] = (unsigned char) (out.g * 255);
                pixels[pos + 2] = (unsigned char) (out.b * 255);
            }
        }
    }
    
    GeneticNode::GeneticNode(FunctionID op, unsigned int pos, ParameterSet  set):
            op(op), pos(pos), set(std::move(set)) {}
    
    void GeneticTree::generateRandomTree(int n) {
        std::queue<int> nodesToProcess;
        std::queue<int> nonFuncNodesToProcess;
        nodesToProcess.push(n);
        while (!nodesToProcess.empty()){
            int node = nodesToProcess.front();
            nodesToProcess.pop();
            
            if (node >= size)
                continue;
            
            auto op = functions.select();
            auto& func = functions[op];
            ParameterSet set = func.generateRandomParameters();
            
            if (func.allowsArgument()) {
                auto& queueToAddTo = func.allowedFuncs() ? nodesToProcess : nonFuncNodesToProcess;
                if (func.singleArgument())
                    queueToAddTo.push(left(node));
                else if (func.bothArgument()){
                    queueToAddTo.push(left(node));
                    queueToAddTo.push(right(node));
                } else if (func.dontCareArgument()) {
                    if (chance(80))
                        queueToAddTo.push(left(node));
                    if (chance(80))
                        queueToAddTo.push(right(node));
                }
            }
            
            nodes[node] = new GeneticNode(op, node, set);
        }
        
        while (!nonFuncNodesToProcess.empty()){
            int node = nonFuncNodesToProcess.front();
            nonFuncNodesToProcess.pop();
            
            if (node >= size)
                continue;
            
            int parent = GeneticTree::parent(node);
            auto parentNode = this->node(parent);
            
            // TODO: throw error?!?
            if (parentNode == nullptr)
                continue;
            
            auto& parentFunc = functions[parentNode->op];
            
            std::vector<FunctionID> allowedFuncs;
            if (parentFunc.allowedColors())
                allowedFuncs.push_back(FunctionID::RAND_COLOR);
            if (parentFunc.allowedScalars())
                allowedFuncs.push_back(FunctionID::RAND_SCALAR);
            
            if (allowedFuncs.empty())
                continue;
            
            auto func = allowedFuncs[randomInt(0, (int)allowedFuncs.size()-1)];
            nodes[node] = new GeneticNode(func, node, functions[func].generateRandomParameters());
        }
    }
    
    int GeneticTree::height(int node) {
        int height = 0;
        while ((node = parent(node)) != -1)
            height++;
        return height;
    }
    
    Color GeneticTree::execute(double x, double y) {
        return execute_internal(x, y, 0);
    }
    
    Color GeneticTree::execute_internal(double x, double y, int node) {
        Color leftC {0};
        Color rightC {0};
        
        auto ourNode = nodes[node];
        auto& func = functions[ourNode->op];
        
        if (func.disallowsArgument())
            return func.call({ARGS_NONE, leftC, rightC}, ourNode->set);
        
        // functions should always take precedence
        if (func.allowedFuncs()) {
            int l = left(node);
            int r = right(node);
            
            auto lNode = leftNode(l);
            auto rNode = rightNode(r);
            
            if (lNode != nullptr)
                leftC = execute_internal(x, y, l);
            else
                if (func.allowedVariables())
                    leftC = Color(x);
            if (rNode != nullptr)
                rightC = execute_internal(x, y, r);
            else
                if (func.allowedVariables())
                    rightC = Color(y);
        } else {
            if (func.allowedVariables()) {
                leftC = Color(x);
                rightC = Color(y);
            } else {
                BLT_WARN("Function called (%s) from node (%d) without any args!", func.name.c_str(), node);
            }
        }
        return func.call({ARGS_BOTH, leftC, rightC}, ourNode->set);
    }
    
    void GeneticTree::mutate() {
        for (int i = 0; i < size; i++){
            if (nodes[i] == nullptr) {
                // TODO: ?
            } else {
                int nodeSize = subtreeSize(i);
                double factor = 1.0 / nodeSize;
                if (chance(nodeMutationChance * factor)) {
                    // delete old subtrees
                    deleteSubtree(i);
                    // create a new completely random tree
                    generateRandomTree(i);
                }
                if (chance(functionMutationChance * factor)){
                    auto newFuncID = functions.select();
                    auto oldFuncID = nodes[i]->op;
                    if (newFuncID != nodes[i]->op){
                        nodes[i]->op = newFuncID;
                        auto& newFunc = functions[newFuncID];
                        auto& oldFunc = functions[oldFuncID];
                        // TODO: use some of the old params!!
                        if (newFunc.getRequiredColors() != oldFunc.getRequiredColors() || newFunc.getRequiredScalars() != oldFunc.getRequiredScalars()){
                            nodes[i]->set = newFunc.generateRandomParameters();
                        }
                        if (oldFunc.dontCareArgument() && newFunc.dontCareArgument())
                            continue;
                        if (oldFunc.singleArgument() && newFunc.singleArgument())
                            continue;
                        if (oldFunc.bothArgument() && oldFunc.bothArgument())
                            continue;
                        if (oldFunc.singleArgument() && newFunc.bothArgument())
                            generateRandomTree(right(i));
                        if (oldFunc.bothArgument() && newFunc.singleArgument())
                            deleteSubtree(right(i));
                    }
                }
                if (nodes[i]->op == FunctionID::RAND_SCALAR && chance(scalarMutationChance * factor)){
                    ParameterSet newSet;
                    newSet.add(RandomScalar::get(nodes[i]->set[0]));
                    nodes[i]->set = std::move(newSet);
                }
                if (nodes[i]->op == FunctionID::RAND_COLOR && chance(colorMutationChance * factor)){
                    ParameterSet newSet;
                    newSet.add(RandomColor::get(nodes[i]->set[0]));
                    nodes[i]->set = std::move(newSet);
                }
            }
        }
    }
    
    void GeneticTree::deleteSubtree(int n) {
        std::queue<int> nodesToDelete;
        nodesToDelete.push(n);
        while (!nodesToDelete.empty()){
            auto node = nodesToDelete.front();
            nodesToDelete.pop();
            
            if (node >= size)
                continue;
            
            if (nodes[node] != nullptr) {
                delete nodes[node];
                nodes[node] = nullptr;
            }
            
            nodesToDelete.push(left(node));
            nodesToDelete.push(right(node));
        }
    }
    
    int GeneticTree::subtreeSize(int n) const {
        int l = left(n);
        int r = right(n);
        int leftSize = 1;
        int rightSize = 1;
        if (l < size && nodes[l] != nullptr)
            leftSize = subtreeSize(l);
        if (r < size && nodes[r] != nullptr)
            rightSize = subtreeSize(r);
        return std::max(leftSize, rightSize) + 1;
    }
    
}