#include <windows.h>
#include <fwpmu.h>
#include <stdio.h>

#pragma comment (lib, "fwpuclnt.lib")

#define EXIT_ON_ERROR(fnName) \
    if (result != ERROR_SUCCESS) \
    { \
        printf(#fnName " = 0x%08X\n", result); \
        goto CLEANUP; \
    }

DWORD GetAllLayers(
	__in HANDLE engine,
	__deref_out_ecount(*layerCount) FWPM_LAYER *** layers,
	__out UINT32 * layerCount)
{
	DWORD result = ERROR_SUCCESS;

	HANDLE enumHandle;
	result = FwpmLayerCreateEnumHandle(
		engine,
		NULL, // Don't filter out any layers (no enumeration template).
		&enumHandle);
	EXIT_ON_ERROR(FwpmLayerCreateEnumHandle);

	FwpmLayerEnum(
		engine,
		enumHandle,
		INFINITE, // Don't limit how many layers we get back.
		layers,
		layerCount);
	EXIT_ON_ERROR(FwpmLayerCreateEnumHandle);
	
CLEANUP:
	FwpmLayerDestroyEnumHandle(engine, enumHandle);
	return result;
}

DWORD GetAllFiltersInLayer(
	__in HANDLE engine,
	__in const GUID * layerKey,
	__deref_out_ecount(*filterCount) FWPM_FILTER *** filters,
	__out UINT32 * filterCount)
{
	DWORD result = ERROR_SUCCESS;
	FWPM_FILTER_ENUM_TEMPLATE enumTemplate;
	HANDLE enumHandle = NULL;

	memset(&enumTemplate, 0, sizeof(enumTemplate));
	enumTemplate.layerKey = *layerKey;    // Only select filters in the layer with `layerKey`.
	enumTemplate.numFilterConditions = 0;
	enumTemplate.actionMask = 0xFFFFFFFF; // We want to see all filters regardless of action.

	result = FwpmFilterCreateEnumHandle(
		engine,
		&enumTemplate,
		&enumHandle);
	EXIT_ON_ERROR(FwpmFilterCreateEnumHandle);

	result = FwpmFilterEnum(
		engine,
		enumHandle,
		INFINITE,
		filters,
		filterCount);
	EXIT_ON_ERROR(FwpmFilterEnum);

CLEANUP:
	FwpmFilterDestroyEnumHandle(engine, enumHandle);
	return result;
}

DWORD wmain(void)
{
	DWORD result = ERROR_SUCCESS;

	// Use dynamic sessions for efficiency and safety:
	//  - All objects associated with the dynamic session are deleted with one call.
	//  - Filtering policy objects are deleted even when the application crashes.
	HANDLE engineHandle = 0;
	FWPM_SESSION session;
	memset(&session, 0, sizeof(session));
	session.flags = FWPM_SESSION_FLAG_DYNAMIC;

	result = FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, &session, &engineHandle);
	EXIT_ON_ERROR(FwpmEngineOpen);

	UINT32 layerCount = 0;
	FWPM_LAYER ** layers = 0;
	result = GetAllLayers(engineHandle, &layers, &layerCount);
	EXIT_ON_ERROR(GetAllLayers);

	for (size_t i = 0; i < layerCount; i++)
	{
		UINT32 filterCount = 0;
		FWPM_FILTER ** filters = 0;

		result = GetAllFiltersInLayer(
			engineHandle,
			&(layers[i]->layerKey),
			&filters,
			&filterCount);

		if (result == ERROR_SUCCESS)
		{
			printf("\n\n%ws: %d filters", layers[i]->displayData.name, filterCount);

			for (size_t j = 0; j < filterCount; j++)
			{
				printf("\n\t%d.\t%ws (%ws)", j + 1, filters[j]->displayData.name, filters[j]->displayData.description);
			}
		}
		else
		{
			printf("\n\tError getting filters (error code 0x%08X)", result);
		}

		//FwpmFreeMemory((void**)filters);
	}

CLEANUP:
	//FwpmFreeMemory((void**)layers);
	//FwpmEngineClose(engineHandle); // This is automatically done when the program closes, but I want to be explicit.
	return result;
}