#include "Spectral/SSSpectralSubsystem.h"

void USSSpectralSubsystem::SetSpectralVisionActive(bool bActive)
{
	if (bSpectralVisionActive == bActive)
	{
		return;
	}

	bSpectralVisionActive = bActive;
	OnSpectralVisionChanged.Broadcast(bSpectralVisionActive);
}
