#include <vector>

#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/GlobalValue.h"
#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/Instruction.h"
#include "llvm/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/InstIterator.h"

//#TODO: Merge with Jiesheng's instruction assignment
//#include "utils.h"

using namespace llvm;

namespace {
	struct instTrace : public FunctionPass {
		static char ID;
		Function::iterator lastBlock;
		BasicBlock::iterator lastInst;

		instTrace() : FunctionPass(ID) {}

		virtual bool doInitialization(Module &M) {
			return true;
		}

		long fetchLLFIInstructionID(Instruction *targetInst) {
			//#TODO: Merge with Jiesheng's instruction assignment
			//return getLLFIIndexofInst(Instruction *targetInst);
			return 1337; //DUMMY VALUE
		}

		virtual bool runOnFunction(Function &F) {

			//Create handles to the functions parent module and context
			LLVMContext& context = F.getContext();
			Module *M = F.getParent();			

			//iterate through each basicblock of the function
			for (Function::iterator blockIterator = F.begin(), lastBlock = F.end();
				blockIterator != lastBlock; ++blockIterator) {

				BasicBlock* block = blockIterator;

				//Iterate through each instruction of the basicblock
				for (BasicBlock::iterator instIterator = blockIterator->begin(), 
					 lastInst = blockIterator->end(); 
					 instIterator != lastInst;
					 ++instIterator) {

					//Print some Debug Info as the pass is being run
					Instruction *inst = instIterator;

					errs() << "instTrace: Found Instruction\n"
						   << "   Opcode Name: " << inst->getOpcodeName() << "\n"
						   << "   Opcode: " << inst->getOpcode() << "\n"
						   << "   Parent Function Name: " << inst->getParent()->getParent()->getNameStr() << "\n";

					if (inst->getType() != Type::getVoidTy(context)) {
						//Create the decleration of the printInstProfile Function with proper arg/return types
						//ID, Opcode, Num Ops, Instruction Value = 4 arguments
						std::vector<const Type*> parameterVector(4);
						parameterVector[0] = Type::getInt32Ty(context);
						parameterVector[1] = Type::getInt32Ty(context);
						parameterVector[2] = Type::getInt32Ty(context);
						parameterVector[3] = inst->getType();
						FunctionType* ppFuncType = FunctionType::get(Type::getVoidTy(context), parameterVector, 0 );
						Constant *ppFunc = M->getOrInsertFunction("printInstProfile", ppFuncType); 


						//Insert the tracing function, passing it the proper arguments
						std::vector<Value*> ppArgs;
						//Fetch the LLFI Instruction ID:
						ConstantInt* IDConstInt = ConstantInt::get(IntegerType::get(context,32), fetchLLFIInstructionID(inst));
						//Fetch the OPcode:
						ConstantInt* OPConstInt = ConstantInt::get(IntegerType::get(context,32), inst->getOpcode());
						//Fetch the number of operands passed to the instruction:
						unsigned numInstOperands = inst->getNumOperands();
						ConstantInt* NumOpsConstInt = ConstantInt::get(IntegerType::get(context,32), numInstOperands);
						//Load All Arguments
						ppArgs.push_back(IDConstInt);
						ppArgs.push_back(OPConstInt);
						ppArgs.push_back(NumOpsConstInt);
						ppArgs.push_back(inst);

						//Create the Function
						if (inst != block->getTerminator()) { //Make sure that the instruction is not a terminator
							//ie, that there is a next instruction to insert before
							instIterator++;
							Instruction *insertPoint = instIterator;
							CallInst::Create(ppFunc, ppArgs.begin(),ppArgs.end(), "", insertPoint);
						}
					}
				}//Instruction Iteration
			}//BasicBlock Iteration

			return true; //Tell LLVM that the Function was modified
		}
	};
}

//Register the pass with the llvm
char instTrace::ID = 0;
static RegisterPass<instTrace> X("instTrace", "Traces instruction execution through program", false, false);
