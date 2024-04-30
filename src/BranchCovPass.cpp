#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"

#include <map>
#include <vector>

using namespace llvm;

namespace {

struct BranchCovPass : public PassInfoMixin<BranchCovPass> {

    // pointers to some primitive types
    Type* IntTy;
    Type* IntPtrTy;
    Type* VoidTy;

    // hold constant address of extern function
    // points to the function instance called _init_
    FunctionCallee PInit;
    // points to the function instance called _probe_
    FunctionCallee PProbe;

    // utility containers
    // hold the mapping from linenumber to branch index
    std::map<int, int> LineNoToIndex;

    // hold lines
    std::vector<int> Lines;

    // hold indices
    std::vector<int> Indices;

    // hold whether it is a switch or not
    std::vector<bool> IsSwitch;

    // do initialization, 
    // returns whether any transformatin occurs during initialization
    bool _initialize(Module &M) {
        // ensure the functions to be instrumented is not defined before the
        // instrumentation
        if (M.getFunction("_init_") != nullptr) {
            errs() << "Function _init_ is already defined\n";
            exit(1);
        }

        if (M.getFunction("_probe_") != nullptr) {
            errs() << "Function _probe_ is already defined\n";
            exit(1);
        }

        // store the ptr to types
        IntTy = Type::getInt32Ty(M.getContext());
        IntPtrTy = Type::getInt32PtrTy(M.getContext());
        VoidTy = Type::getVoidTy(M.getContext());

        // add _init_ declaration
        Type* InitArgs[4] = {IntTy, IntPtrTy, IntPtrTy, IntPtrTy};
        FunctionType* InitFTy = FunctionType::get(VoidTy, InitArgs, false);
        PInit = M.getOrInsertFunction("_init_", InitFTy);

        // add _probe_ declaration
        Type* ProbeArgs[3] = {IntTy, IntTy, IntTy};
        FunctionType* ProbeFTy = FunctionType::get(VoidTy, ProbeArgs, false);
        PProbe = M.getOrInsertFunction("_probe_", ProbeFTy);

        return false;
    }

    bool _finalize(Module &M) {
        // no need to instrument _init_ in a module without main function
        Function* MainFunc = M.getFunction("main");
        if (MainFunc == nullptr) {
            errs() << "No main function found\n";
            exit(1);
        }

        auto InsertPt = MainFunc->getEntryBlock().getFirstInsertionPt();
        IRBuilder<> Builder(&*InsertPt);

        std::vector<Value*> InitArgs {};

        InitArgs.push_back(ConstantInt::get(IntTy, Lines.size()));

        std::vector<Constant*> LinesVec {};

        for (auto Line: Lines) {
            LinesVec.push_back(ConstantInt::get(IntTy, Line));
        }

        Constant* LinesArr = ConstantArray::get(ArrayType::get(IntTy, Lines.size()), LinesVec);

        GlobalVariable* LinesGV = new GlobalVariable(
            M,
            LinesArr->getType(),
            false,
            GlobalValue::PrivateLinkage,
            LinesArr
        );
        InitArgs.push_back(Builder.CreatePointerCast(LinesGV, IntPtrTy));

        std::vector<Constant*> IndicesVec {};

        for (auto Index: Indices) {
            IndicesVec.push_back(ConstantInt::get(IntTy, Index));
        }

        Constant* IndicesArr = ConstantArray::get(ArrayType::get(IntTy, Indices.size()), IndicesVec);

        GlobalVariable* IndicesGV = new GlobalVariable(
            M,
            IndicesArr->getType(),
            false,
            GlobalValue::PrivateLinkage,
            IndicesArr
        );

        InitArgs.push_back(Builder.CreatePointerCast(IndicesGV, IntPtrTy));

        std::vector<Constant*> IsSwitchVec {};

        for (auto Switch: IsSwitch) {
            IsSwitchVec.push_back(ConstantInt::get(IntTy, Switch));
        }

        Constant* IsSwitchArr = ConstantArray::get(ArrayType::get(IntTy, IsSwitch.size()), IsSwitchVec);

        GlobalVariable* IsSwitchGV = new GlobalVariable(
            M,
            IsSwitchArr->getType(),
            false,
            GlobalValue::PrivateLinkage,
            IsSwitchArr
        );

        InitArgs.push_back(Builder.CreatePointerCast(IsSwitchGV, IntPtrTy));

        Builder.CreateCall(PInit, InitArgs);

        return true;
    }

    Value* _getDist(Value* Cond, IRBuilder<> &Builder) {
        auto* Inst = dyn_cast<ICmpInst>(Cond);
        if (Inst == nullptr) {
            return ConstantInt::get(IntTy, 8192);
        }
        auto* LHS = Inst->getOperand(0);
        auto* RHS = Inst->getOperand(1);
        auto* Sub = Builder.CreateSub(LHS, RHS);
        return Sub;
    }

    bool _instrumentOnBr(BranchInst* InstBr) {
        //errs() << "Instrumenting Branch\n";
        // 编译一定要记得开 -g 选项，不然这里就会 crash
        int LineNo = InstBr->getDebugLoc().getLine();
        //errs() << "LineNo: " << LineNo << "\n";
        int Index {-1};
        auto It = LineNoToIndex.find(LineNo);
        
        if (It != LineNoToIndex.end()) {
            Index = It->second + 1;
        } else {
            Index = 0;
        }

        // Store Metadata about the branch
        Lines.push_back(LineNo);
        Indices.push_back(Index);
        IsSwitch.push_back(false);

        LineNoToIndex[LineNo] = Index;

        // Create Probe Call before branch
        IRBuilder<> Builder(InstBr);
        // create a cast inst to cast the condition to int
        //errs() << "Creating cast inst\n";
        Value* Cond = InstBr->getCondition();
        Value* CondInt = Builder.CreateZExt(Cond, IntTy);
        Value* Dist = _getDist(Cond, Builder);
        // create a call to probe
        Value* ProbeArgs[3] = {ConstantInt::get(IntTy, Lines.size() - 1), CondInt, Dist};
        
        //errs() << "ProbeArgs prepared\n";
        
        Builder.CreateCall(PProbe, ProbeArgs);

        //errs() << "Instrumented Branch, line: " << LineNo << ", index: " << Index << "\n";

        return true;
    }

    bool _instrumentOnSwitch(SwitchInst* InstSwitch) {

        //errs() << "Instrumenting Switch\n";

        int LineNo = InstSwitch->getDebugLoc().getLine();
        int Index {-1};

        auto It = LineNoToIndex.find(LineNo);
        if (It != LineNoToIndex.end()) {
            Index = It->second + 1;
        } else {
            Index = 0;
        }

        // Store Metadata about the branch
        for (int i = 1; i <= InstSwitch->getNumSuccessors(); i++) {
            Lines.push_back(LineNo);
            Indices.push_back(Index);
            IsSwitch.push_back(true);

            BasicBlock* BB;

            if (i == InstSwitch->getNumSuccessors()) {
                BB = InstSwitch->getDefaultDest();
            } else {
                BB = InstSwitch->getSuccessor(i - 1);
            }

            // create probe call before branch
            IRBuilder<> Builder(BB->getFirstNonPHI());
            Value* ProbeArgs[3] = {ConstantInt::get(IntTy, Lines.size() - 1), ConstantInt::get(IntTy, 1), ConstantInt::get(IntTy, 1)};
            Builder.CreateCall(PProbe, ProbeArgs);

            Index ++;
        }

        LineNoToIndex[LineNo] = Index - 1;

        return false;
    }

    bool _instrumentOnBB(BasicBlock &BB) {
        
        bool Changed = false;
        for (auto &I: BB) {
            if (I.getOpcode() == Instruction::Br) {
                auto InstBr = cast<BranchInst>(&I);
                if (InstBr->isConditional()) {
                    Changed |= _instrumentOnBr(InstBr);
                }
            } else if (I.getOpcode() == Instruction::Switch) {
                auto InstSwitch = cast<SwitchInst>(&I);
                Changed |= _instrumentOnSwitch(InstSwitch);
            }
        }

        return Changed;
    }

    bool _instrument(Module &M, ModuleAnalysisManager &AM) {
        bool Changed = false;
        for (auto& F: M) {
            auto Subprogram = F.getSubprogram();
            if (Subprogram == nullptr || Subprogram->getName().empty()) {
                //errs() << "skip function " << F.getName() << "\n";
                continue;
            }
            if (F.getName().equals("main") || 
            F.getName().equals("_init_") || 
            F.getName().equals("_probe_") || 
            F.getName().contains("TestCase") || 
            F.getName().contains("Suite") || 
            F.getName().contains("testing") ||
            F.getName().contains("RUN_ALL_TESTS")
            || F.getName().startswith("__cxx_global")) {
                //errs() << "skip function " << F.getName() << "\n"; 
                //errs() << "corresponding to " << Subprogram->getName() << "\n";
                continue;
            }
            //errs() << "instrument function " << F.getName() << "\n";
            //errs() << "corresponding to " << Subprogram->getName() << "\n";
            for (auto& BB: F) {
                Changed |= _instrumentOnBB(BB);
            }
        }

        return Changed;
    }

    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        bool Changed = _initialize(M);

        Changed |= _instrument(M, AM);

        Changed |= _finalize(M);

        return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
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
