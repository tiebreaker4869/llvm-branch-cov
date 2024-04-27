# LLVM Branch Coverage Tool
This is an LLVM transformation pass that instruments LLVM IR to collect branch coverage data.

## How to Use
Follow these steps to use this tool:

1. Build the BranchCovPass shared library.
2. Compile the program you want to instrument with the Pass. Make sure to use the -g option - it's essential for this process.
3. Compile the instrumented function.
4. Link the object files to create the executable.

## View Execution Result
The branch coverage data will be saved in a file named coverage. The data format is as follows:

For conditional branches: lineNo.predicateIndex -> true_count, true_dist,  false_count, false_dist

For switch statements: lineNo.predicateIndex -> true_count, true_dist

Please note that the predicateIndex is not globally unique, but it is unique within the same line number.
