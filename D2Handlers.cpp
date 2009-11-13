#include <vector>

#include "D2Handlers.h"
#include "Script.h"
#include "ScreenHook.h"
#include "Unit.h"
#include "Helpers.h"
#include "Core.h"
#include "Constants.h"
#include "Events.h"
#include "CollisionMap.h"
#include "ScriptEngine.h"
#include "Console.h"
#include "D2BS.h"

using namespace std;

Variables Vars;

DWORD WINAPI D2Thread(LPVOID lpParam)
{
	bool beginStarter = true;
	bool bInGame = false;
	InitSettings();
	if(InitHooks())
	{
		Log("D2BS Engine startup complete.");
		Print("�c2D2BS�c0 :: Engine startup complete!");
	}
	else
	{
		Log("D2BS Engine startup failed.");
		Print("�c2D2BS�c0 :: Engine startup failed!");
		return FALSE;
	}

	static int x = D2GetScreenSizeX();
	Vars.image->SetX(x/2);
	Vars.text->SetX(x/2);

	while(Vars.bActive)
	{
		switch(ClientState())
		{
			case ClientStateInGame:
			{
				if(x != D2GetScreenSizeX())
				{
					x = D2GetScreenSizeX();
					Vars.image->SetX(x/2);
					Vars.text->SetX(x/2);
				}

				if(bInGame)
				{
					if((Vars.dwMaxGameTime && Vars.dwGameTime && 
							(GetTickCount() - Vars.dwGameTime) > Vars.dwMaxGameTime) ||
							(!IsTownLevel(GetPlayerArea()) &&
							(Vars.nChickenHP && Vars.nChickenHP >= GetUnitHP(D2CLIENT_GetPlayerUnit())) ||
							(Vars.nChickenMP && Vars.nChickenMP >= GetUnitMP(D2CLIENT_GetPlayerUnit()))))
					D2CLIENT_ExitGame();
				}
				else
				{
					Sleep(1000);

					Vars.dwGameTime = GetTickCount();
					D2CLIENT_InitInventory();

					GameJoined();

					bInGame = true;
				}
				break;
			}
			case ClientStateMenu:
			{
				if(x != D2GetScreenSizeX())
				{
					x = D2GetScreenSizeX();
					Vars.image->SetX(x/2);
					Vars.text->SetX(x/2);
				}

				if(bInGame)
				{
					Vars.dwGameTime = NULL;
					bInGame = false;
					MenuEntered(beginStarter);
					beginStarter = false;
				}
				break;
			}
			case ClientStateBusy:
			case ClientStateNull:
				break;
		}
		Sleep(50);
	}

	ScriptEngine::Shutdown();

	return NULL;
}

DWORD __fastcall GameInput(wchar_t* wMsg)
{
	if(Vars.bDontCatchNextMsg)
		return NULL;

	char* szBuffer = UnicodeToAnsi(wMsg);
	bool result = false;

	if(szBuffer[0] == '.')
	{
		result = ProcessCommand(szBuffer+1, false);
	}

	delete[] szBuffer;

	return result == true ? 0 : -1;
}

DWORD __fastcall GamePacketReceived(BYTE* pPacket, DWORD dwSize)
{
	if(pPacket[0] == 0x15)
	{
		if(*(LPDWORD)&pPacket[2] == D2CLIENT_GetPlayerUnit()->dwUnitId)
			pPacket[10] = NULL;	
	}
	if(pPacket[0] == 0x95 || pPacket[0] == 0x18)
	{
		WORD Life = *(WORD*)&pPacket[1];
		WORD Mana = *(WORD*)&pPacket[3];

		if((Life & 0x8000) == 0x8000)
		{
			Life ^= 0x8000;
		}
		if((Mana & 0x8000) == 0x8000)
		{
			Mana ^= 0x8000;
		}
		if((Mana & 0x4000) == 0x4000)
		{
			Mana ^= 0x4000;
		}
		Mana *= 2;

		static WORD SaveLife = 0;
		if(SaveLife != Life)
		{
			SaveLife = Life;
			LifeEvent(Life);
		}

		static WORD SaveMana = 0;
		if(SaveMana != Mana)
		{
			SaveMana = Mana;
			ManaEvent(Mana);
		}
	}
	else if(pPacket[0] == 0x26)
	{
		CHAR* pName = (CHAR*)pPacket+10;
		CHAR* pMessage = (CHAR*)pPacket + strlen(pName) + 11;
	
		ChatEvent(pName, pMessage);
	}
	else if(pPacket[0] == 0x5a && pPacket[1] == 0x07 && pPacket[2] == 0x08)
	{
		if(Vars.bQuitOnHostile)
		{
			D2CLIENT_ExitGame();
		}
	}
	else if(pPacket[0] == 0xA7)
	{
		if(pPacket[6] == AFFECT_JUST_PORTALED)
			return FALSE;
	}
	else if(pPacket[0] == 0x9c)	//itemDropEvent() by bobite, todo: sending all modes
	{
		if(pPacket[1] == 0x00 ||pPacket[1] == 0x02 ||pPacket[1] == 0x03 )
		{			
			char Code[5] = "";
			WORD itemX;
			WORD itemY;
			//(data+pos/8)<<(64-len-(pos&7))>>(64-len)); taken from magnet and mousepad
			//		date=packet, len= size of data being red, pos = where in the packet -1

			Code[0]=(*(unsigned __int64 *)(pPacket+141/8)<<(64-8-(141&7))>>(64-8));		
			Code[1]=(*(unsigned __int64 *)(pPacket+149/8)<<(64-8-(149&7))>>(64-8));	
			Code[2]=(*(unsigned __int64 *)(pPacket+157/8)<<(64-8-(157&7))>>(64-8));	
			Code[3]=(*(unsigned __int64 *)(pPacket+165/8)<<(64-8-(165&7))>>(64-8));	
			Code[(Code[3] == ' ' ? 3 : 4)] = '\0';
			itemX=(*(unsigned __int64 *)(pPacket+108/8)<<(64-16-(108&7))>>(64-16));	
			itemY=(*(unsigned __int64 *)(pPacket+125/8)<<(64-16-(125&7))>>(64-16));	
			itemX=itemX/2;
			//itemY=itemY/2; //only x gets /2
			WORD Mode = *(BYTE*)&pPacket[1];
			DWORD GID = *(DWORD*)&pPacket[4];

			if(strcmp(Code, "gld") == 0)
				GoldDropEvent(GID, itemX, itemY, Mode);
			else
				ItemDropEvent(GID, Code, itemX, itemY, Mode);

		}
	}
	else if(pPacket[0] == 0x5a){ // SOJ and Walks Msg by bobite
		if (pPacket[1] == 0x11){ //stones
			DWORD soj = *(DWORD*)&pPacket[3];
			char mess[256]; 
			sprintf_s(mess, sizeof(mess), "%u Stones of Jordan Sold to Merchants", soj);				
			GameMsgEvent(mess);
		}
		if (pPacket[1] == 0x12){ //diablo walks
			char mess[] ="Diablo Walks the Earth";
			GameMsgEvent(mess);
		}
	}

	return TRUE;
}

LONG WINAPI GameEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* pCopy;
	switch(uMsg)
	{
		case WM_COPYDATA:
			pCopy = (COPYDATASTRUCT*)lParam;

			if(pCopy)
			{
				if(pCopy->dwData == 0x1337) // 0x1337 = Execute Script
				{
					Script* script = ScriptEngine::CompileCommand((char*)pCopy->lpData);
					if(script)
						CreateThread(0, 0, ScriptThread, script, 0, 0);
				}
				CopyDataEvent(pCopy->dwData, (char*)pCopy->lpData);
			}
			return TRUE;
		case WM_ACTIVATEAPP:
			if(!wParam && Vars.bBlockMinimize)
				return NULL;
			break;
		//case WM_NCDESTROY:
		//	{
		//		//Vars.bActive = FALSE;
		//		return TRUE;
		//	}
	}

	return (LONG)CallWindowProcA(Vars.oldWNDPROC, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam)
{
	if(code >= HC_ACTION)
	{
		WORD repeatCount = LOWORD(lParam);
		bool altState = !!(HIWORD(lParam) & KF_ALTDOWN);
		bool previousState = !!(HIWORD(lParam) & KF_REPEAT);
		bool transitionState = !!(HIWORD(lParam) & KF_UP);
		bool isRepeat = !transitionState && repeatCount != 1;
		bool isDown = !(previousState && transitionState);
		bool isUp = previousState && transitionState;

		bool gameState = !!GameReady();
		bool chatBoxOpen = gameState ? !!D2CLIENT_GetUIState(5) : false;
		bool escMenuOpen = gameState ? !!D2CLIENT_GetUIState(9) : false;

		if (altState && wParam == VK_F4)
			return CallNextHookEx(NULL, code, wParam, lParam);

		if(Vars.bBlockKeys)
			return 1;
		
		if(wParam == VK_HOME && !(chatBoxOpen || escMenuOpen))
		{
			if(isDown && !isRepeat && code == HC_ACTION)
			{
				if(!altState)
					Console::ToggleBuffer();
				else
					Console::TogglePrompt();

				return CallNextHookEx(NULL, code, wParam, lParam);
			}
		}			
		else if(wParam == VK_ESCAPE && Console::IsVisible())
		{
			if(isDown && !isRepeat && code == HC_ACTION )
			{
				Console::Hide();
				return 1;
			}
			return CallNextHookEx(NULL, code, wParam, lParam);
		}
		else if(Console::IsEnabled())
		{
			BYTE layout[256] = {0};
			WORD out[2] = {0};
			switch(wParam)
			{
				case VK_TAB:
					if(isUp)
						for(int i = 0; i < 5; i++)
							Console::AddKey(' ');
					break;
				case VK_RETURN:
					if(isUp && !isRepeat && !escMenuOpen)
						Console::ExecuteCommand();
					break;
				case VK_BACK:
					if(isDown)
						Console::RemoveLastKey();
					break;
				case VK_UP:
					if(isUp && !isRepeat)
						Console::PrevCommand();
					break;
				case VK_DOWN:
					if(isUp && !isRepeat)
						Console::NextCommand();
					break;
				default:
					if(isDown)
					{
						GetKeyboardState(layout);
						if(ToAscii(wParam, (lParam & 0xFF0000), layout, out, 0) != 0)
						{
							for(int i = 0; i < repeatCount; i++)
								Console::AddKey(out[0]);
						}
					}
					break;
			}			
			return 1;
		}
		else if(code == HC_ACTION && !isRepeat && !(chatBoxOpen || escMenuOpen))
			KeyDownUpEvent(wParam, isUp);
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT* mouse = (MOUSEHOOKSTRUCT*)lParam;
	POINT pt = mouse->pt;
	ScreenToClient(mouse->hwnd, &pt);

	// filter out clicks on the window border
	if(code == HC_ACTION && (pt.x < 0 || pt.y < 0))
		return CallNextHookEx(NULL, code, wParam, lParam);

	if(Vars.bBlockMouse)
		return 1;

	if(code == HC_ACTION)
	{
		bool clicked = false;

		switch(wParam)
		{
			case WM_LBUTTONDOWN:
				MouseClickEvent(0, pt, false);
				for(HookList::iterator it = Genhook::GetFirstHook(); it != Genhook::GetLastHook(); it++)
					if((*it)->Click(0, &pt))
						clicked = true;
				break;
			case WM_LBUTTONUP:
				MouseClickEvent(0, pt, true);
				break;
			case WM_RBUTTONDOWN:
				MouseClickEvent(1, pt, false);
				for(HookList::iterator it = Genhook::GetFirstHook(); it != Genhook::GetLastHook(); it++)
					if((*it)->Click(1, &pt))
						clicked = true;
				break;
			case WM_RBUTTONUP:
				MouseClickEvent(1, pt, true);
				break;
			case WM_MBUTTONDOWN:
				MouseClickEvent(2, pt, false);
				for(HookList::iterator it = Genhook::GetFirstHook(); it != Genhook::GetLastHook(); it++)
					if((*it)->Click(2, &pt))
						clicked = true;
				break;
			case WM_MBUTTONUP:
				MouseClickEvent(2, pt, true);
				break;
			case WM_MOUSEMOVE:
	//			MouseMoveEvent(pt);
	//			for(HookList::iterator it = Genhook::GetFirstHook(); it != Genhook::GetLastHook(); it++)
	//				(*it)->Hover(&pt);
				break;
		}

		return clicked ? 1 : CallNextHookEx(NULL, code, wParam, lParam);
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}

VOID GameDraw(VOID)
{
	if(Vars.bActive && ClientState() == ClientStateInGame)
	{
		Console::Draw();
		Genhook::DrawAll(IG);
	}
}

VOID GameDrawOOG(VOID)
{
	D2WIN_DrawSprites();
	if(Vars.bActive && ClientState() == ClientStateMenu)
	{
		Console::Draw();
		Genhook::DrawAll(OOG);
	}
}

VOID __fastcall WhisperHandler(CHAR* szAcc, CHAR* szText)
{
	WhisperEvent(szAcc, szText);
}

DWORD __fastcall GameAttack(AttackStruct* pAttack)
{
	if(!pAttack || !pAttack->lpTargetUnit || pAttack->lpTargetUnit->dwType != UNIT_MONSTER)
		return (DWORD)-1;

	if(pAttack->dwAttackType == ATTACKTYPE_UNITLEFT)
		pAttack->dwAttackType = ATTACKTYPE_SHIFTLEFT;

	if(pAttack->dwAttackType == ATTACKTYPE_RIGHT)
		pAttack->dwAttackType = ATTACKTYPE_SHIFTRIGHT;

	return NULL;
}

VOID __fastcall GamePlayerAssignment(UnitAny* pPlayer)
{
	if(!pPlayer)
		return;

	PlayerAssignEvent(pPlayer->dwUnitId);
}

