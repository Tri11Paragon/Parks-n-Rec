//
// Created by brett on 7/18/23.
//
#include <genetic/v3/program_v3.h>
#include "imgui.h"
#include <queue>

namespace parks::genetic {
    
    void Program::run() {
        if (ImGui::Button("Regen Program And Run")) {
            ParameterSet set = functions[FunctionID::COLOR_NOISE].generateRandomParameters();
            
            for (unsigned int i = 0; i < WIDTH; i++) {
                for (unsigned int j = 0; j < HEIGHT; j++){
                    auto pos = getPixelPosition(i, j);
                    
                    auto out = functions[FunctionID::COLOR_NOISE].call({ARGS_BOTH, Color((double)i / WIDTH), Color((double)j / HEIGHT)}, set);
                    
                    pixels[pos] = (unsigned char)(out.r * 255);
                    pixels[pos + 1] = (unsigned char) (out.g * 255);
                    pixels[pos + 2] = (unsigned char) (out.b * 255);
                }
            }
        }
        if (ImGui::Button("Crossover")){
        
        }
        if (ImGui::Button("Mutate")){
        
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
    
    GeneticNode::GeneticNode(FunctionID op, unsigned int pos, const ParameterSet& set):
            op(op), pos(pos), set(set) {}
    
    void GeneticTree::generateRandomTree() {
        std::queue<int> nodesToProcess;
        nodesToProcess.push(0);
        while (!nodesToProcess.empty()){
            int node = nodesToProcess.front();
            nodesToProcess.pop();
            
            if (node >= size)
                continue;
            
            // end early by constructing a leaf
            if (chance(5)){
                continue;
            }
            
            auto op = functions.select();
            auto& func = functions[op];
            ParameterSet set = func.generateRandomParameters();
            
            if (func.allowsArgument() && func.allowedFuncs()) {
                if (func.singleArgument())
                    nodesToProcess.push(left(node));
                else if (func.bothArgument()){
                    nodesToProcess.push(left(node));
                    nodesToProcess.push(right(node));
                } else if (func.dontCareArgument()) {
                    if (chance(80))
                        nodesToProcess.push(left(node));
                    if (chance(80))
                        nodesToProcess.push(right(node));
                }
            }
            
            nodes[node] = new GeneticNode(op, node, set);
        }
    }
    
    int GeneticTree::height(int node) {
        int height = 1;
        while ((node = parent(node)) != 0)
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
        
        // functions should always take precedence
        if (func.allowedFuncs()) {
            int l = left(node);
            int r = right(node);
            
            auto lNode = leftNode(l);
            auto rNode = rightNode(r);
            
            if (lNode != nullptr)
                leftC = execute_internal(x, y, l);
            if (rNode != nullptr)
                rightC = execute_internal(x, y, r);
        } else {
            
            if (func.allowedColors())
            
        }
        
        return Color(0, 0, 0);
    }
    
}