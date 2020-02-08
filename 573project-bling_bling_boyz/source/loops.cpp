
#include "asts.h"
#include "liveness.h"
#include "loops.h"

#include <string>
#include <iostream>

// Just need 10% improvement over base cases, don't need to implement all of this (one at a time till we meet benchmark)

std::vector<std::vector<int>> getEdges(std::vector<std::vector<instruction>> blocks)
{
    std::vector<std::vector<int>> edges;
    for (int i = 0; i < blocks.size(); i++)
    {
        if (blocks[i].size() != 0) 
        {
            instruction jumpStmt = blocks[i][blocks[i].size()-1];
            std::vector<int> edge;
            switch(jumpStmt.type)
            {
                case JMP:
                case JEQ:
                case JNE:
                case JLT:
                case JGT:
                case JLE:
                case JGE:
                    edge.push_back(i);
                    for (int j = 0; j < blocks.size(); j++)
                    {
                        if (!blocks[j].empty())
                            if (blocks[j][0].type == LABEL && (blocks[j][0].op1 == jumpStmt.result || blocks[j][0].op1 == jumpStmt.op1))
                                edge.push_back(j);
                    }
                    break;
            }
            edges.push_back(edge);
        }
    }
    return edges;
}

/* 
TODO: 1.) Add reaching-definition analysis 
a.) Find some way to implement powerset of variables
Evidently powerset is set of sets

input: control flow graph CFG = (Nodes, Edges, Entry, Exit)

out[Entry] = NULL

for i in Nodes
    out[i] = NULL
ChangedNodes = N

while(ChangedNodes != NULL)
{
    remove i from ChangedNodes
    in[i] = in[i] union out[p], for all predecessors p of i
    oldout = out[i]
    out[i] = gen[i] union (in[i] - kill[i])
    if oldout != out[i]
    {
        for all successors s of i
            add s to ChangedNodes
    }
}
*/

void reachingDefinition(CFG * lattice)
{
    std::vector<std::set<int>> in, out;
    std::set<int> changedNodes;

    for (int i = 0; i < lattice->nodes.size(); i++)
    {
        std::set<int> temp;
        out.push_back(temp);
        changedNodes.insert(i);
    }
 
    while(!changedNodes.empty())
    {
        // Remove first node from set
        std::vector<instruction> node;
        int nVal = *changedNodes.begin();
        //instruction cur = lattice->nodes.at(nVal);

        //node.push_back(lattice->nodes[nVal]);
        changedNodes.erase(changedNodes.begin());
    }
}

std::vector<instruction> loopOptimize(std::vector<instruction>& code)
{
    loadLiveness(code);
    // Divide code into nodes/blocks
    std::vector<std::vector<instruction>> blocks = getBlocks(code);
    // Determine edges between nodes
    std::vector<std::vector<int>> edges = getEdges(blocks);

    CFG * lattice = new CFG(blocks, edges, 0, blocks.size());

    reachingDefinition(lattice);
    
    return code;
}


/* 
TODO: 2.) Determine invariance/movability of statements
Requisites for movability + invariance for var Z
a.) Statement is in all loop exits where Z is live 
b.) Only one definition of Z in loop
c.) Z is not live before loop
*/



/* 
TODO: 3.) Identify basic induction, then mutual induction variables
*/

/* 
TODO: 4.) Perform strength reduction
*/