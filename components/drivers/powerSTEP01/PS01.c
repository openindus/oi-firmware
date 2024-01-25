#include "PS01.h"

void PS01_Init(PS01_Hal_Config_t* config, PS01_Param_t* param)
{ 
	PS01_Hal_SetConfig(config);

	for (int i=0; i<POWERSTEP01_NUMBER_OF_DEVICES; i++) {
		PS01_Hal_Init(i);
		// PS01_Param_SetAll(i, param);
		PS01_Cmd_HardHiZ(i);
		PS01_Cmd_GetStatus(i);
	}
}