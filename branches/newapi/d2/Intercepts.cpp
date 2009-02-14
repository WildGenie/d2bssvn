#include "Structs.h"
#include "Functions.h"
#include "Intercepts.h"
#include "Handlers.h"

#include "debug_new.h"

// disable 'inline asm modifies ebp' warning -- we know it's there, we did it.
#pragma warning ( disable: 731 )

void GameMinimize_Intercept()
{
	__asm
	{
		xor eax, eax
		retn
	}
}

void GameInput_Intercept()
{
	static DWORD InputCall_I = NULL;
	if(!InputCall_I)
	{
		InputCall_I = GetD2ClientInputCall_I_ASM();
	}
	__asm
	{
		pushad
		mov ecx, ebx
		call Input_Handler
		cmp eax, -1
		popad
		je BlockIt
		call InputCall_I
		ret

BlockIt:
		xor eax,eax
		ret
	}
}

void GameInternalDraw_Intercept()
{
	__asm
	{
		call InternalDraw_Handler;
		pop esi
		pop ebx
		pop ecx
		retn 4
	}
}

void GamePacketReceived_Intercept()
{
	__asm
	{
		pop ebp;
		pushad;

		call PacketReceived_Handler;
		test eax, eax;

		popad;
		jnz OldCode;

		mov edx, 0;

OldCode:
		call ReceivePacket_I;

		push ebp;
		ret;
	}
}

UnitAny* GameGetSelectedUnit_Intercept(void)
{
/*	if(Vars.bClickAction)
	{
		if(Vars.dwSelectedUnitId)
		{
			return D2CLIENT_FindUnit(Vars.dwSelectedUnitId, Vars.dwSelectedUnitType);
		}

		return NULL;
	}*/

	return GetSelectedUnit();
}

void GameExternalDraw_Intercept(void)
{
	ExternalDraw_Handler();
}

void GameWhisper_Intercept()
{
	__asm
	{
		mov ebp, dword ptr ss:[esp+0x200]
		pushad
		mov ecx, edx
		mov edx, ebx
		call Whisper_Handler
		popad
		//jmp D2MULTI_WhisperIntercept_Jump
		retn
	}
}

void GameAttack_Intercept()
{
	static DWORD Attack_I = NULL;
	if(!Attack_I)
	{
		Attack_I = GetD2ClientAttack_I_ASM();
	}
	__asm 
	{
		push ecx
		mov ecx, [esp+0xC]
		call Attack_Handler
		pop ecx

		cmp eax, -1
		je OldCode

		call GetSelectedUnit
		
		cmp eax, 0
		je OldCode

		mov [esp+0x0C], 1

OldCode:
		mov eax, [Attack_I]
		retn
	}
}

void GameCrashFix_Intercept()
{
	__asm
	{
		cmp ecx, 0
		je Skip
		mov dword ptr ds:[ecx+0x10], edx

Skip:
		mov dword ptr ds:[eax+0xC], 0
		retn
	}
}

void GamePlayerAssign_Intercept()
{
	static DWORD AssignPlayer_I = NULL;
	if(!AssignPlayer_I)
	{
		AssignPlayer_I = GetD2ClientAssignPlayer_I_ASM();
	}
	__asm
	{
		fnop
		call AssignPlayer_I
		mov ecx, eax
		call PlayerAssignment_Handler
		retn
	}
}
