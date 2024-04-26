#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace {

struct BranchCovPass : public PassInfoMixin<BranchCovPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        for (auto &F : M) {
            // declare function
            LLVMContext& ctx = F.getContext();
            FunctionCallee logFunc = F.getParent()->getOrInsertFunction(
                "logop", Type::getVoidTy(ctx), Type::getInt32Ty(ctx)
            );
            for (auto& B: F) {
                for (auto& I: B) {
                    if (auto* instr = dyn_cast<BinaryOperator>(&I)) {
                        // Insert at the point where the instr appears(before)
                        IRBuilder<> builder(instr);

                        builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

                        // Insert Function Call
                        Value* args[] = {instr};
                        builder.CreateCall(logFunc, args);

                        // we modified the code
                        return PreservedAnalyses::none();
                    }
                }
            }
        }
        return PreservedAnalyses::all();
    };
};

}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "BranchCov pass",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                    MPM.addPass(BranchCovPass());
                });
        }
    };
}