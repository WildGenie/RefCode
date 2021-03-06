#include "NIDAQmx.h"
extern "C" {
int32 DAQmxCreateAIResistanceChan_kkh(TaskHandle taskHandle, 
		                      const char* chanName, 
				      const char* assigned, 
				      float64 minVal, 
				      float64 maxVal,
				      int32 unit, 
				      int32 config,
				      int32 source,
				      float64 excit,
				      const char* custom)
{
	return DAQmxCreateAIResistanceChan(taskHandle, chanName, assigned, minVal, maxVal, unit, config, source, excit, custom);
}
};
