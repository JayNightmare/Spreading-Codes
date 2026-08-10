//Prevent multiple inclusions of this header file
#pragma once
//Include necessary header files
//This is like a list in python
#include <vector>
//namespace is for prevent naming conflicts in class and function names
namespace lunanet::gateway5 {
    //std::vector<float> = return type
    //BuildSyncReferenceSymbols = function name
    //The process code will be added in the cpp file
    std::vector<float> BuildSyncReferenceSymbols();

}  // namespace lunanet::gateway5