#include <stdio.h>
#include <stdbool.h>
#include "Definitions.h"
#include "InitializingTools.h"
#include "FileManagment.h"
#include "InterpreterTools.h"
#include "StringManagment.h"
#include "SimulatorTools.h"

void giveCommands(int ac, char** av, Context* context) { // Wydaje rozkazy umozliwiajace wykonanie calego programu

	getInput(getInputFilePath(ac, av, context), context);
	if (context->psa_code)
		interpreteInput(context);
	generateMachineCode(context);
	simulateProgram(context->outputText, context);

}

void startProgramCommander(int ac, char** av) { // Rozpoczyna prace program commander'a

	giveCommands(ac, av, initContext());

}


