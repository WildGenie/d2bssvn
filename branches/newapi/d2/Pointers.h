// this file is for variables and ASM entry points needed for modifying functions in-situ

#pragma once

#ifndef __POINTERS_H__
#define __POINTERS_H__

#include "Macros.h"
#include "Structs.h"

// variables
VARIABLE(D2Common, DWORD, sqptDataTable, 0x9B500)

VARIABLE(D2Launch, BnetData *, BnData, 0x25ACC)

VARIABLE(D2Multi, char*, ChatBoxMsg, 0x38F18)

VARIABLE(D2Win, Control*, FirstControl, 0x5C718)
VARIABLE(D2Win, Control*, FocusedControl, 0x5C718+0x10)

VARIABLE(D2Client, int, MouseOffsetY, 0x1121CC)
VARIABLE(D2Client, int, MouseOffsetX, 0x1121D0)
VARIABLE(D2Client, int, Divisor, 0xF3B98)
VARIABLE(D2Client, int, yShake, 0xFD114)
VARIABLE(D2Client, int, xShake, 0x11B9B4)
VARIABLE(D2Client, GameStructInfo*, GameInfo, 0x11B908)
VARIABLE(D2Client, DWORD, Ping, 0x1032BC)
VARIABLE(D2Client, DWORD, FPS, 0x11C388)
VARIABLE(D2Client, DWORD, WaypointTab, 0x11B956)
VARIABLE(D2Client, UnitAny*, SelectedInvItem, 0x11B9FC)
// TODO: Double check this value...
VARIABLE(D2Client, POINT*, Offset, 0x11C2E8)
VARIABLE(D2Client, DWORD, AutomapOn, 0x102B80)
VARIABLE(D2Client, AutomapLayer*, FirstAutomapLayer, 0x11C2B0)
VARIABLE(D2Client, AutomapLayer*, AutomapLayer, 0x11C2B4)
VARIABLE(D2Client, int, AutomapYPosition, 0x11C30C)
VARIABLE(D2Client, UnitAny*, PlayerUnit, 0x11C3D0)
VARIABLE(D2Client, RosterUnit*, PlayerUnitList, 0x11C080)
VARIABLE(D2Client, DWORD, QuestTab, 0x11D50D)
VARIABLE(D2Client, DWORD, MouseX, 0x101638)
VARIABLE(D2Client, DWORD, MouseY, 0x101634)
VARIABLE(D2Client, DWORD, MapId, 0x11C020)
VARIABLE(D2Client, DWORD, WeaponSwitch, 0x11BA58)
VARIABLE(D2Client, DWORD, WaypointTable, 0x11B951)
VARIABLE(D2Client, DWORD, TradeAccepted, 0x11BC2C)
VARIABLE(D2Client, DWORD, TradeBlock, 0x11BC3C)
VARIABLE(D2Client, DWORD, TradeFlag, 0x11C1F4)
VARIABLE(D2Client, DWORD, ScreenSizeX, 0xDC6E0)
VARIABLE(D2Client, DWORD, ScreenSizeY, 0xDC6E4)
VARIABLE(D2Client, wchar_t*, ChatMsg, 0x11FC40)
VARIABLE(D2Client, DWORD, AlwaysRun, 0x11C050)
VARIABLE(D2Client, wchar_t*, RecentTradeName, 0x12334C)
VARIABLE(D2Client, DWORD, RecentTradeId, 0x11C1F4)
VARIABLE(D2Client, UnitAny*, SelectedUnit, 0x11C1D8)
VARIABLE(D2Client, DWORD, ItemPriceList, 0x11B643)
VARIABLE(D2Client, DWORD, RepairCosts, 0x11B67B)
VARIABLE(D2Client, DWORD, RecentInteractId, 0x11B625)
VARIABLE(D2Client, NPCMenu*, NPCMenu, 0xF8300)
VARIABLE(D2Client, DWORD, NPCMenuAmount, 0xF8A50)
VARIABLE(D2Client, DWORD, ScreenCovered, 0x11C1D0)
VARIABLE(D2Client, InventoryLayout*, InventoryLayout, 0x100CD0)
VARIABLE(D2Client, InventoryLayout*, StashLayout, 0x100BC0)
VARIABLE(D2Client, InventoryLayout*, StoreLayout, 0x100CA0)
VARIABLE(D2Client, InventoryLayout*, CubeLayout, 0x100CB8)
VARIABLE(D2Client, InventoryLayout*, TradeLayout, 0x100B78)
VARIABLE(D2Client, InventoryLayout*, MercLayout, 0x11BB5C)
VARIABLE(D2Client, DWORD, StoreTable, 0x101638)
VARIABLE(D2Client, wchar_t*, ChatTextBuffer, 0x11FC40)
VARIABLE(D2Client, DWORD, RegularCursorType, 0x101674)
VARIABLE(D2Client, DWORD, ShopCursorType, 0x11B9F8)
VARIABLE(D2Client, DWORD, OverheadTrigger, 0x1054F6)
VARIABLE(D2Client, DWORD, ExpCharFlag, 0x10330C)
VARIABLE(D2Client, DWORD, ChatBoxFlag, 0x11BF4C)
VARIABLE(D2Client, WORD, MercStrIndex, 0xF6B4C)
VARIABLE(D2Client, DWORD, MercReviveCost, 0x11C3D4)
VARIABLE(D2Client, DWORD, ShopPerformFlag_I, 0x11B56B)
VARIABLE(D2Client, DWORD, ShopPerformFlag_II, 0x11B667)
VARIABLE(D2Client, DWORD, GoldDialogAmount, 0x11BF38)
VARIABLE(D2Client, DWORD, GoldDialogAction, 0x102B38)

// ASM entry points
ADDRESS(D2Lang, Say_II, 0xA6E0)

ADDRESS(D2Common, DisplayOverheadMsg_I, -10375)
ADDRESS(D2Common, CheckCollision_I, 0x58370)

ADDRESS(D2Client, Say_I, 0x87206)
ADDRESS(D2Client, PlaySound_I, 0x31FA0)
ADDRESS(D2Client, PerfomNpcAction_I, 0x191C0)
ADDRESS(D2Client, TakeWP_I, 0x797E0)
ADDRESS(D2Client, ClickMap_I, 0x102B7C)
ADDRESS(D2Client, ClickMap_II, 0x11C1D8)
ADDRESS(D2Client, ClickMap_III, 0x5D5A0)
ADDRESS(D2Client, ClickMap_IV, 0x5CEF9)
ADDRESS(D2Client, GenerateSpell_I, 0x4F800)
ADDRESS(D2Client, TestPvpFlag_I, 0x30DD0)
ADDRESS(D2Client, LoadUIImage_I, 0xBEED0)
ADDRESS(D2Client, LoadAct_I, 0x409E0)
ADDRESS(D2Client, LoadAct_II, 0x406A0)
ADDRESS(D2Client, GetUnitName_I, 0x1F3D0)
ADDRESS(D2Client, InputCall_I, 0x79670)
ADDRESS(D2Client, Interact_I, 0x5CDD0)
ADDRESS(D2Client, ClickParty_I, 0x78D10)
ADDRESS(D2Client, ClickLocPtrs, 0xDCAE4)
ADDRESS(D2Client, ClickShopItem_I, 0x191C0)
ADDRESS(D2Client, ShopAction_I, 0x19E00)
ADDRESS(D2Client, ClickBelt_I, 0x81FB0)
ADDRESS(D2Client, ClickBeltRight_I, 0x83DF0)
ADDRESS(D2Client, ClickItemRight_I, 0xB60F0)
ADDRESS(D2Client, GetItemDesc_I, 0x8A140)
ADDRESS(D2Client, MercItemAction_I, 0x798E0)
ADDRESS(D2Client, Attack_I, 0xDC6E4)
ADDRESS(D2Client, AssignPlayer_I, 0x20D60)

// redefinitions for simplicity
#define p_D2COMMON_sqptDataTable GetD2CommonsqptDataTable()

#define p_D2LAUNCH_BnetData GetD2LaunchBnetData()

#define p_D2MULTI_ChatBoxMsg GetD2MultiChatBoxMsg()

#define p_D2WIN_FirstControl GetD2WinFirstControl()
#define p_D2WIN_FocusedControl GetD2WinFocusedControl()

#define p_D2CLIENT_MouseOffsetY GetD2ClientMouseOffsetY()
#define p_D2CLIENT_MouseOffsetX GetD2ClientMouseOffsetX()
#define p_D2CLIENT_Divisor GetD2ClientDivisor()
#define p_D2CLIENT_yShake GetD2ClientyShake()
#define p_D2CLIENT_xShake GetD2ClientxShake()
#define p_D2CLIENT_GameInfo GetD2ClientGameInfo()
#define p_D2CLIENT_Ping GetD2ClientPing()
#define p_D2CLIENT_FPS GetD2ClientFPS()
#define p_D2CLIENT_WaypointTab GetD2ClientWaypointTab()
#define p_D2CLIENT_SelectedInvItem GetD2ClientSelectedInvItem()
#define p_D2CLIENT_Offset GetD2ClientOffset()
#define p_D2CLIENT_AutomapOn GetD2ClientAutomapOn()
#define p_D2CLIENT_FirstAutomapLayer GetD2ClientFirstAutomapLayer()
#define p_D2CLIENT_AutomapLayer GetD2ClientAutomapLayer()
#define p_D2CLIENT_AutomapYPosition GetD2ClientAutomapYPosition()
#define p_D2CLIENT_PlayerUnit GetD2ClientPlayerUnit()
#define p_D2CLIENT_PlayerUnitList GetD2ClientPlayerUnitList()
#define p_D2CLIENT_QuestTab GetD2ClientQuestTab()
#define p_D2CLIENT_MouseX GetD2ClientMouseX()
#define p_D2CLIENT_MouseY GetD2ClientMouseY()
#define p_D2CLIENT_MapId GetD2ClientMapId()
#define p_D2CLIENT_WeaponSwitch GetD2ClientWeaponSwitch()
#define p_D2CLIENT_WaypointTable GetD2ClientWaypointTable()
#define p_D2CLIENT_TradeAccepted GetD2ClientTradeAccepted()
#define p_D2CLIENT_TradeBlock GetD2ClientTradeBlock()
#define p_D2CLIENT_TradeFlag GetD2ClientTradeFlag()
#define p_D2CLIENT_ScreenSizeX GetD2ClientScreenSizeX()
#define p_D2CLIENT_ScreenSizeY GetD2ClientScreenSizeY()
#define p_D2CLIENT_ChatMsg GetD2ClientChatMsg()
#define p_D2CLIENT_AlwaysRun GetD2ClientAlwaysRun()
#define p_D2CLIENT_RecentTradeName GetD2ClientRecentTradeName()
#define p_D2CLIENT_RecentTradeId GetD2ClientRecentTradeId()
#define p_D2CLIENT_SelectedUnit GetD2ClientSelectedUnit()
#define p_D2CLIENT_ItemPriceList GetD2ClientItemPriceList()
#define p_D2CLIENT_RepairCosts GetD2ClientRepairCosts()
#define p_D2CLIENT_RecentInteractId GetD2ClientRecentInteractId()
#define p_D2CLIENT_NPCMenu GetD2ClientNPCMenu()
#define p_D2CLIENT_NPCMenuAmount GetD2ClientNPCMenuAmount()
#define p_D2CLIENT_ScreenCovered GetD2ClientScreenCovered()
#define p_D2CLIENT_InventoryLayout GetD2ClientInventoryLayout()
#define p_D2CLIENT_StashLayout GetD2ClientStashLayout()
#define p_D2CLIENT_StoreLayout GetD2ClientStoreLayout()
#define p_D2CLIENT_CubeLayout GetD2ClientCubeLayout()
#define p_D2CLIENT_TradeLayout GetD2ClientTradeLayout()
#define p_D2CLIENT_MercLayout GetD2ClientMercLayout()
#define p_D2CLIENT_StoreTable GetD2ClientStoreTable()
#define p_D2CLIENT_ChatTextBuffer GetD2ClientChatTextBuffer()
#define p_D2CLIENT_RegularCursorType GetD2ClientRegularCursorType()
#define p_D2CLIENT_ShopCursorType GetD2ClientShopCursorType()
#define p_D2CLIENT_OverheadTrigger GetD2ClientOverheadTrigger()
#define p_D2CLIENT_ExpCharFlag GetD2ClientExpCharFlag()
#define p_D2CLIENT_ChatBoxFlag GetD2ClientChatBoxFlag()
#define p_D2CLIENT_MercStrIndex GetD2ClientMercStrIndex()
#define p_D2CLIENT_MercReviveCost GetD2ClientMercReviveCost()
#define p_D2CLIENT_ShopPerformFlag_I GetD2ClientShopPerformFlag_I()
#define p_D2CLIENT_ShopPerformFlag_II GetD2ClientShopPerformFlag_II()
#define p_D2CLIENT_GoldDialogAmount GetD2ClientGoldDialogAmount()
#define p_D2CLIENT_GoldDialogAction GetD2ClientGoldDialogAction()

#define D2LANG_Say_II GetD2LangSay_II_ASM()

#define D2COMMON_DisplayOverheadMsg_I GetD2CommonDisplayOverheadMsg_I_ASM()
#define D2COMMON_CheckCollision_I GetD2CommonCheckCollision_I_ASM()

#define D2CLIENT_Say_I GetD2ClientSay_I_ASM()
#define D2CLIENT_PlaySound_I GetD2ClientPlaySound_I_ASM()
#define D2CLIENT_PerfomNpcAction_I GetD2ClientPerfomNpcAction_I_ASM()
#define D2CLIENT_TakeWP_I GetD2ClientTakeWP_I_ASM()
#define D2CLIENT_ClickMap_I GetD2ClientClickMap_I_ASM()
#define D2CLIENT_ClickMap_II GetD2ClientClickMap_II_ASM()
#define D2CLIENT_ClickMap_III GetD2ClientClickMap_III_ASM()
#define D2CLIENT_ClickMap_IV GetD2ClientClickMap_IV_ASM()
#define D2CLIENT_GenerateSpell_I GetD2ClientGenerateSpell_I_ASM()
#define D2CLIENT_TestPvpFlag_I GetD2ClientTestPvpFlag_I_ASM()
#define D2CLIENT_LoadUIImage_I GetD2ClientLoadUIImage_I_ASM()
#define D2CLIENT_LoadAct_I GetD2ClientLoadAct_I_ASM()
#define D2CLIENT_LoadAct_II GetD2ClientLoadAct_II_ASM()
#define D2CLIENT_GetUnitName_I GetD2ClientGetUnitName_I_ASM()
#define D2CLIENT_InputCall_I GetD2ClientInputCall_I_ASM()
#define D2CLIENT_Interact_I GetD2ClientInteract_I_ASM()
#define D2CLIENT_ClickParty_I GetD2ClientClickParty_I_ASM()
#define D2CLIENT_ClickLocPtrs GetD2ClientClickLocPtrs_ASM()
#define D2CLIENT_ClickShopItem_I GetD2ClientClickShopItem_I_ASM()
#define D2CLIENT_ShopAction_I GetD2ClientShopAction_I_ASM()
#define D2CLIENT_ClickBelt_I GetD2ClientClickBelt_I_ASM()
#define D2CLIENT_ClickBeltRight_I GetD2ClientClickBeltRight_I_ASM()
#define D2CLIENT_ClickItemRight_I GetD2ClientClickItemRight_I_ASM()
#define D2CLIENT_GetItemDesc_I GetD2ClientGetItemDesc_I_ASM()
#define D2CLIENT_MercItemAction_I GetD2ClientMercItemAction_I_ASM()
#define D2CLIENT_Attack_I GetD2ClientAttack_I_ASM()
#define D2CLIENT_AssignPlayer_I GetD2ClientAssignPlayer_I_ASM()

#endif
