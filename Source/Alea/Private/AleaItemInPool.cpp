#include "AleaItemInPool.h"
#include "AleaAleo.h"

void AAleaItemInPool::SetOwningPlayer(AAleaAleo* Aleo)
{
	Owner = Aleo;

	if (!Owner)
	{
		Release();
	}
}
