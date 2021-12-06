#include <iostream>
#include <fstream>
#include <memory>

#include <fmt/core.h>

#include "parser/parser.hpp"
#include "ir_generator.hpp"

//#include "llvm/IR/Verifier.h"
//#include "llvm/ExecutionEngine/ExecutionEngine.h"
//#include "llvm/IR/BasicBlock.h"
//#include "llvm/IR/Constants.h"
//#include "llvm/IR/DerivedTypes.h"
//#include "llvm/IR/Function.h"
//#include "llvm/IR/LLVMContext.h"
//#include "llvm/IR/Module.h"
//#include "llvm/IR/Type.h"
//#include "llvm/Support/raw_ostream.h"
//#include "llvm/IR/IRBuilder.h"

//using namespace llvm;

std::string readFile(std::string const &path) {
    std::ifstream t(path.c_str());
    t.seekg(0, std::ios::end);
    auto size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);
    return buffer;
}

int main(int argc, char **argv) {
    std::string path = "tests/tst_prog1.c";
    if (argc > 1)
        path = argv[1];

    auto text = readFile(path);
    auto ast = std::shared_ptr<AST_TranslationUnit>(parse_program(text));

//    fmt::print("{}\n", ast->getTreeNode()->printHor());

    auto gen = IR_Generator();
    gen.parseAST(ast);
    gen.printBlocks();

//    LLVMContext *context = new LLVMContext();
//    Module *module = new Module("top", *context);
//    IRBuilder<> builder(*context);
//
//    FunctionType *funcType = FunctionType::get(builder.getInt32Ty(), false);
//    Function *mainFunc = Function::Create(funcType, Function::ExternalLinkage, "main", module);
//    mainFunc->setCallingConv(CallingConv::C);
//
//    BasicBlock *entry = BasicBlock::Create(*context, "entrypoint", mainFunc);
//    builder.SetInsertPoint(entry);
//
//    Value *const1 = ConstantInt::get(Type::getInt32Ty(*context), 353);
//    Value *const2 = ConstantInt::get(Type::getInt32Ty(*context), 48);
//    Value *retval = builder.CreateAdd(const1, const2, "ret");
//    builder.CreateRet(retval);
//
//    module->print(outs(), nullptr);
//    delete module;
//    delete context;

    return 0;
}
