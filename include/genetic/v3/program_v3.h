//
// Created by brett on 7/18/23.
//

#ifndef PARKSNREC_PROGRAM_V3_H
#define PARKSNREC_PROGRAM_V3_H

#include <genetic/v3/functions_v3.h>

namespace parks::genetic {
    
    struct GeneticNode {
        FunctionID op;
        unsigned int pos{};
        ParameterSet set;
        
        GeneticNode(FunctionID op, unsigned int pos, const ParameterSet& set);
    };
    
    class GeneticTree {
        private:
            GeneticNode** nodes;
            int size = 1;
            
            void generateRandomTree();
            
            Color execute_internal(double x, double y, int node);
        public:
            explicit GeneticTree(int max_height){
                for (int i = 0; i < max_height; i++)
                    size *= 2;
                nodes = new GeneticNode*[size];
                
                for (int i = 0; i < size; i++)
                    nodes[i] = nullptr;
                
                generateRandomTree();
            }
            
            Color execute(double x, double y);
            
            static inline int left(int pos){
                return 2 * pos;
            }
            static inline int right(int pos){
                return 2 * pos + 1;
            }
            static inline int parent(int pos){
                if (pos <= 0)
                    return 0;
                return (pos) / 2;
            }
            inline GeneticNode* leftNode(int pos){
                if (pos < 0 || pos >= size)
                    return nullptr;
                return nodes[left(pos)];
            }
            inline GeneticNode* rightNode(int pos){
                if (pos < 0 || pos >= size)
                    return nullptr;
                return nodes[right(pos)];
            }
            
            static int height(int node);
            
            void deleteTree(){
                for (int i = 0; i < size; i++) {
                    delete nodes[i];
                    nodes[i] = nullptr;
                }
            }
            
            ~GeneticTree(){
                deleteTree();
                delete[] nodes;
            }
    };
    
    class Program {
        private:
            unsigned char pixels[WIDTH * HEIGHT * CHANNELS];
            
            static size_t getPixelPosition(unsigned int x, unsigned int y){
                return x * CHANNELS + y * WIDTH * CHANNELS;
            }
        public:
            void run();
        
            float getRenderProgress(){
                return 0;
            }
            
            inline unsigned char* getPixels(){
                return pixels;
            }
    };
    
}

#endif //PARKSNREC_PROGRAM_V3_H
