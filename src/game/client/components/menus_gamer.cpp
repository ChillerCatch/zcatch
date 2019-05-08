#include <base/color.h>

#include <engine/shared/config.h>
#include <engine/textrender.h>
#include <engine/graphics.h>

#include <game/version.h>
#include <game/client/ui.h>
#include <game/client/render.h>
#include <game/client/gameclient.h>
// #include <game/client/animstate.h>

#include <fstream> // Loading stats
#include <time.h> // Random

#include "binds.h"
#include "menus.h"
#include "skins.h"
#include "items.h"
#include "sounds.h"

 void CMenus::NewLine(CUIRect *pButton, CUIRect *pView)
{
	pNewLineButton = pButton;
	pNewLineView = pView;
	
	if(pNewLineButton == NULL || pNewLineView == NULL)
		return;
	NewLine();
}

void CMenus::NewLine()
{
	if(pNewLineButton == NULL || pNewLineView == NULL)
		return;
	pNewLineView->HSplitTop(20.0f, pNewLineButton, pNewLineView);
} 

void CMenus::RenderSettingsGamer(CUIRect MainView)
{
	CUIRect Button, Tabbar, BottomView;
	{
		MainView.HSplitBottom(80.0f, &MainView, &BottomView);
		MainView.HSplitTop(24.0f, &Tabbar, &MainView);
		RenderTools()->DrawUIRect(&MainView, vec4(0.0f, 0.0f, 0.0f, 0.5f), CUI::CORNER_ALL, 10.0f);
		MainView.Margin(10.0f, &MainView);
		RenderSettingsGamerGeneral(MainView);
		return;
	}
	// char buf[64];
	
	static int s_SettingsPage = 0;
	
	// Tabs (Teecomp pattern)
	MainView.HSplitBottom(80.0f, &MainView, &BottomView);
	MainView.HSplitTop(24.0f, &Tabbar, &MainView);

	const char* Tabs[] = {"General", "Stats", "Credits"};
	int NumTabs = (int)(sizeof(Tabs)/sizeof(*Tabs));
	
	RenderTools()->DrawUIRect(&MainView, vec4(0.0f, 0.0f, 0.0f, 0.5f), CUI::CORNER_ALL, 10.0f);

	for(int i=0; i<NumTabs; i++)
	{
		Tabbar.VSplitLeft(10.0f, &Button, &Tabbar);
		Tabbar.VSplitLeft(80.0f, &Button, &Tabbar);
				
		static int s_Buttons[3] = {0};
		if (DoButton_MenuTab(&s_Buttons[i], Tabs[i], s_SettingsPage == i, &Button, CUI::CORNER_TL | CUI::CORNER_TR))
			s_SettingsPage = i;
	}
	
	// ui_hsplit_t(&main_view, 10.0f, &button, &main_view);
	MainView.Margin(10.0f, &MainView);
	
	if(s_SettingsPage == 0)
		RenderSettingsGamerGeneral(MainView);
	else if(s_SettingsPage == 1)
	{
		// RenderSettingsGamerStats(MainView);
	}
	else if(s_SettingsPage == 2)
	{
		// RenderSettingsGamerCredits(MainView);
	}
}

void CMenus::RenderSettingsGamerGeneral(CUIRect MainView)
{
	CUIRect Button, LeftView, RightView;
	char aBuf[512];
	
	static int s_FirstTime = 1;
	if(s_FirstTime)
	{
		srand(time(NULL));
		s_FirstTime = 0;
	}
	
	MainView.VSplitLeft(MainView.w/2, &LeftView, &RightView);
		
	// LeftView.HSplitTop(20.0f, &Button, &LeftView);
	NewLine(&Button, &LeftView);
	
	UI()->DoLabel(&Button, Localize("Health and Ammo"), 14.0f, CUI::ALIGN_LEFT);
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClGhud, "Gamer HUD", &Button);
		
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClNoHud, "Disable HUD", &Button);
		
	NewLine();
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_GfxHealthBar, "Display healthbar", &Button);
	
	if(g_Config.m_GfxHealthBar)
	{
		NewLine();
		DoButton_BinaryCheckBox(&g_Config.m_GfxHealthBarDamagedOnly, "Only display damaged attributes", &Button);

		NewLine();
		DoButton_BinaryCheckBox(&g_Config.m_GfxArmorUnderHealth, "Render armor under health for the healthbar", &Button);
		
		NewLine();
		DoButton_BinaryCheckBox(&g_Config.m_GfxHealthBarNumbers, "Render numbers next to the healthbar", &Button);
	}

	// NewLine();
	// DoButton_BinaryCheckBox(&g_Config.m_ClGcolor, "Use Gamer colors", &Button);
	
	/*
	NewLine();
	NewLine();
	UI()->DoLabel(&Button, Localize("Announcers"), 14.0f, CUI::ALIGN_LEFT);
	
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClAnnouncers, "Visual announcers", &Button);
	
	if(g_Config.m_ClAnnouncers)
	{
		// NewLine();
		// DoButton_BinaryCheckBox(&g_Config.m_ClAnnouncersShadows, "Render shadows around the announcers", &Button);
		
		NewLine();
		DoButton_BinaryCheckBox(&g_Config.m_ClAnnouncersLegend, "Render a legend under the announcers", &Button);
	}

	NewLine();
	NewLine();
	UI()->DoLabel(&Button, Localize("Extras"), 14.0f, CUI::ALIGN_LEFT);
	NewLine();
	{
		static float Offset = 0.0f;
		static int s_PlusButton = 0;
		static int s_MinusButton = 0;
		bool Minus, Plus;
		
		str_format(aBuf, sizeof(aBuf), "%d", g_Config.m_ClBroadcastSize);
		UI()->DoLabel(&Button, Localize("Broadcast size (%)"), 12.0f, -1);
		Button.VSplitLeft(120.0f, 0, &Button);
		Button.VSplitLeft(40.0f, &Button, 0);
		DoEditBox(&g_Config.m_ClBroadcastSize, &Button, aBuf, sizeof(aBuf), 14.0f, &Offset);
		
		g_Config.m_ClBroadcastSize =  max(1, str_toint(aBuf));
		
		Button.VSplitLeft(45.0f, 0, &Button);
		Button.VSplitLeft(15.0f, &Button, 0);
		Plus = DoButton_Menu(&s_PlusButton, "+", 0, &Button);
		
		Button.VSplitLeft(16.0f, 0, &Button);
		Button.VSplitLeft(15.0f, &Button, 0);
		Minus = DoButton_Menu(&s_MinusButton, "-", 0, &Button);
		
		g_Config.m_ClBroadcastSize += (int)Plus*10 - (int)Minus*10;
	}*/

	NewLine();
	NewLine();
	UI()->DoLabel(&Button, Localize("Instagib"), 14.0f, CUI::ALIGN_LEFT);
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClShieldDisplay, "Display shield graphics instead of pickups", &Button);
	
	
	NewLine();
	NewLine();
	UI()->DoLabel(&Button, Localize("Automapper"), 14.0f, CUI::ALIGN_LEFT);
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClAutomapperMenus, "Show automapper options in the ingame menus", &Button);

	NewLine();
	static int s_ShowGameTiles = 0;
	if(DoButton_CheckBox(&s_ShowGameTiles, "Show the game tiles", g_Config.m_GfxGameTiles > 0, &Button))
	{
		if(g_Config.m_GfxGameTiles)
			g_Config.m_GfxGameTiles = 0;
		else
			g_Config.m_GfxGameTiles = 1;
	}

	NewLine();
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClClientRecognition, "Enable gamer client recognition", &Button);
	

	/*
	NewLine();
	if(g_Config.m_GfxGameTiles)
	{
		// LeftView.HSplitTop(Spacing, 0, &LeftView);
		Button.VSplitLeft(20.f, 0, &Button);
		static int s_ShowMapOnTop = 0;
		if(DoButton_CheckBox(&s_ShowMapOnTop, Localize("Show the map on top"), g_Config.m_GfxGameTiles == 1, &Button))
		{
			if(g_Config.m_GfxGameTiles == 2)
				g_Config.m_GfxGameTiles = 1;
			else
				g_Config.m_GfxGameTiles = 2;
		}
	}*/

	// NewLine();
	// NewLine();
	// UI()->DoLabel(&Button, Localize("Browser"), 14.0f, CUI::ALIGN_LEFT);
	// NewLine();
	// DoButton_BinaryCheckBox(&g_Config.m_ClColorfulBrowser, "Use colored icons", &Button);


	// char aBuf[32];
	// str_format(aBuf, sizeof(aBuf), Localize("Show the game tiles:"));
	// Text.y += 2.0f;
	// UI()->DoLabel(&Text, aBuf, Text.h*ms_FontmodHeight*0.8f, CUI::ALIGN_LEFT);

	// Button.VSplitLeft(160.0f, &Button, 0);
	// if(g_Config.m_GfxGameTiles == 0)
	// 	str_format(aBuf, sizeof(aBuf), Localize("no"));
	// else if(g_Config.m_GfxGameTiles == 1)
	// 	str_format(aBuf, sizeof(aBuf), Localize("yes, on top of the world"));
	// else if(g_Config.m_GfxGameTiles == 2)
	// 	str_format(aBuf, sizeof(aBuf), Localize("yes, only them"));
	// static CButtonContainer s_ButtonGfxGameTiles;
	// if(DoButton_Menu(&s_ButtonGfxGameTiles, aBuf, 0, &Button))
	// 	g_Config.m_GfxGameTiles = (g_Config.m_GfxGameTiles + 1) % 3;

	/*
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClShowapm, "Show ingame APM counter", &Button);
	
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClShowLanTest, "Show the LAN test button", &Button);
	
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_UiHighlighGametype, "Color gametypes in the server browser", &Button);
	
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_UiGrayServers, "Gray empty servers in the server browser", &Button);
	*/
	
	// Right View	
	// RightView.HSplitTop(20.0f, &Button, &RightView);
	NewLine(&Button, &RightView);
	
	/*UI()->DoLabel(&Button, Localize("Fake"), 14.0f, -1);
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_UiFakeOption, "Display one-click fake button in server browser", &Button);
	
	NewLine();
	if(DoButton_CheckBox(&g_Config.m_ClFake, Localize("Use fake"), g_Config.m_ClFake, &Button))
	{
		char name[32];

		g_Config.m_ClFake ^= 1;
		str_copy(name, g_Config.m_PlayerName, sizeof(name));
		str_copy(g_Config.m_PlayerName, g_Config.m_PlayerSecondName, sizeof(g_Config.m_PlayerName));
		str_copy(g_Config.m_PlayerSecondName, name, sizeof(g_Config.m_PlayerSecondName));
		
		str_copy(name, g_Config.m_PlayerSkin, sizeof(name));
		str_copy(g_Config.m_PlayerSkin, g_Config.m_PlayerSecondSkin, sizeof(g_Config.m_PlayerSkin));
		str_copy(g_Config.m_PlayerSecondSkin, name, sizeof(g_Config.m_PlayerSecondSkin));
		
		str_copy(name, g_Config.m_PlayerClan, sizeof(name));
		str_copy(g_Config.m_PlayerClan, g_Config.m_PlayerSecondClan, sizeof(g_Config.m_PlayerClan));
		str_copy(g_Config.m_PlayerSecondClan, name, sizeof(g_Config.m_PlayerSecondClan));
	}
	char str[50];
	str_format(str, sizeof(str), "(Used: %s|%s)", g_Config.m_PlayerClan, g_Config.m_PlayerName);
	Button.x += 120.0f;
	UI()->DoLabel(&Button, str, 12.0f, -1);
	*/
	// NewLine();
	// NewLine();
	UI()->DoLabel(&Button, Localize("Chat"), 14.0f, CUI::ALIGN_LEFT);
	
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClChatSound, "Chat sounds", &Button);

	if(g_Config.m_ClChatSound)
	{
		NewLine();
		DoButton_BinaryCheckBox(&g_Config.m_ClSwapChatSounds, "Swap normal/highlight chat sounds", &Button);
	}
		
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClDiscreetWhispers, "Discreet whispering", &Button);

	/*NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClArrows, "Use arrows in place of chat", &Button);
		
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClNoCustomForArrows, "Do not use custom team colors for arrows", &Button);
	*/

	NewLine();
	{		
		static float Offset = 0.0f;
		static CButtonContainer s_PlusButton;
		static CButtonContainer s_MinusButton;
		bool Minus, Plus;
		
		str_format(aBuf, sizeof(aBuf), "%d", g_Config.m_ClTextSize);
		UI()->DoLabel(&Button, Localize("Chat size (%)"), 12.0f, CUI::ALIGN_LEFT);
		Button.VSplitLeft(90.0f, 0, &Button);
		Button.VSplitLeft(40.0f, &Button, 0);
		DoEditBox(&g_Config.m_ClTextSize, &Button, aBuf, sizeof(aBuf), 14.0f, &Offset);
		
		Button.VSplitLeft(45.0f, 0, &Button);
		Button.VSplitLeft(15.0f, &Button, 0);
		Plus = DoButton_Menu(&s_PlusButton, "+", 0, &Button);
		
		Button.VSplitLeft(16.0f, 0, &Button);
		Button.VSplitLeft(15.0f, &Button, 0);
		Minus = DoButton_Menu(&s_MinusButton, "-", 0, &Button);
			
		Button.VSplitLeft(45.0f, 0, &Button);
		UI()->DoLabel(&Button, Localize("Dune says : Test !"), 12.0f*clamp(g_Config.m_ClTextSize, 50, 200)/100.0f, CUI::ALIGN_LEFT);
		g_Config.m_ClTextSize =  max(1, str_toint(aBuf));
		
		if(Plus)
			g_Config.m_ClTextSize += 10;
		else if(Minus)
			g_Config.m_ClTextSize -= 10;
	}
	
	/*
	NewLine();
	NewLine();

	UI()->DoLabel(&Button, Localize("Scoreboard"), 14.0f, CUI::ALIGN_LEFT);
	
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClShowId, "Show IDs", &Button);
	
	NewLine();
	DoButton_BinaryCheckBox(&g_Config.m_ClShowSkins, "Show skin names", &Button);
	*/

	NewLine();
	NewLine();
	UI()->DoLabel(&Button, Localize("Sounds"), 14.0f, CUI::ALIGN_LEFT);
	NewLine();
	if(DoButton_CheckBox(&g_Config.m_ClGSound, Localize("Gamer sounds"), g_Config.m_ClGSound, &Button))
		g_Config.m_ClGSound ^= 1;
		
	Button.VSplitLeft(145.0f, 0, &Button);
	Button.VSplitLeft(140.0f, &Button, 0);
	static CButtonContainer s_TestButton;
	if(DoButton_Menu(&s_TestButton, "Test one!", 0, &Button))
	{
		int sounds[11] = {SOUND_SPREE_HUMILIATION, SOUND_SPREE_KILLING, SOUND_SPREE_RAMPAGE, SOUND_SPREE_DOMINATING, SOUND_SPREE_UNSTOPPABLE, SOUND_SPREE_GODLIKE,
		SOUND_SPREE_WICKEDSICK, SOUND_SPREE_PREPARETOFIGHT, SOUND_SPREE_PREPARETOKILL, SOUND_SPREE_HOLYSHIT, SOUND_SPREE_FIRSTBLOOD};
		m_pClient->m_pSounds->Play(CSounds::CHN_GUI, sounds[rand()%11], 0);
	}	

	NewLine();
	NewLine();
	UI()->DoLabel(&Button, Localize("Maps: default background"), 14.0f, CUI::ALIGN_LEFT);
	NewLine();
	RenderTools()->DrawUIRect(&Button, vec4(0.0f, 0.0f, 0.0f, 0.25f), CUI::CORNER_ALL, 5.0f);
	// Button.VSplitLeft(0.0f, 0, &Button);
	// Button.VSplitLeft(350.0f, &Button, 0);
	static CButtonContainer s_ResetClearButton;
	if(DoButton_Menu(&s_ResetClearButton, Localize("Reset background color"), 0, &Button))
	{
		const ivec4 DefaultClearColor = ivec4(149, 255, 53, 255);
		g_Config.m_GfxClearColor = (int(DefaultClearColor.x) << 16) + (int(DefaultClearColor.y) << 8) + int(DefaultClearColor.z);
	}
	bool Modified;
	ivec4 Hsl = RenderHSLPicker(RightView, g_Config.m_GfxClearColor, false, Modified);
	if(Modified)
		g_Config.m_GfxClearColor = (Hsl.x << 16) + (Hsl.y << 8) + Hsl.z;

	NewLine(NULL, NULL);
}

#if 0
#define PRINT_STATS_BEGIN(str) \
	MainView.HSplitTop(20.0f, &Button, &MainView);\
	UI()->DoLabel(&Button, Localize(str), 14.0f, CUI::ALIGN_LEFT);\
	Button.VSplitLeft(115.0f, 0, &Button);\
	
#define PRINT_STATS(str, var) \
	PRINT_STATS_BEGIN(str) \
	str_format(buf, sizeof(buf), "%d", var);\
	UI()->DoLabel(&Button, buf, 12.0f, CUI::ALIGN_LEFT);\

#define PRINT_STATS_MED(str, var, games) \
	PRINT_STATS_BEGIN(str) \
	str_format(buf, sizeof(buf), "%.2f", (float)var/(float)games);\
	UI()->DoLabel(&Button, buf, 12.0f, CUI::ALIGN_LEFT);\

#define PRINT_STATS_STR(str, str2, str3) \
	PRINT_STATS_BEGIN(str) \
	UI()->DoLabel(&Button, str2, 12.0f, CUI::ALIGN_LEFT);\
	Button.VSplitLeft(120.0f, 0, &Button);\
	UI()->DoLabel(&Button, str3, 12.0f, CUI::ALIGN_LEFT);\


void CMenus::RenderSettingsGamerStats(CUIRect MainView)
{
	CUIRect Button, ResetButton, RefreshButton, Footer;
	static PersonalStats stats;
	static int IsLoaded = NOT_LOADED;
	
	if(!IsLoaded)
	{
		char Path[512];
		FILE* File;
		
#if defined(CONF_FAMILY_WINDOWS)
		str_format(Path, sizeof(Path), "%s\\stats.cfg", ClientUserDirectory());
#else
		str_format(Path, sizeof(Path), "%s/stats.cfg", ClientUserDirectory());
#endif
		
		File = fopen(Path, "r");
		if(File)
		{
			int* StatsVar[] = {&stats.games, &stats.frags_with[0], &stats.frags_with[1], &stats.frags_with[2], &stats.frags_with[3], &stats.frags_with[4], &stats.frags_with[5],
			&stats.deaths_from[0], &stats.deaths_from[1], &stats.deaths_from[2], &stats.deaths_from[3], &stats.deaths_from[4], &stats.deaths_from[5], &stats.frags, &stats.deaths,
			&stats.suicides, &stats.flag_grabs, &stats.flag_captures, &stats.carriers_killed, &stats.kills_carrying, &stats.deaths_carrying, &stats.won,};
		
			int NumStatsVar = 22, i;
			
			for(i = 0; i < NumStatsVar; i++)
			{
				if(!fscanf(File, "%d", StatsVar[i]))
				{
					dbg_msg("client/stats", "file is corrupted, cannnot continue reading");
					break;
				}
			}
			
			fclose(File);
			
			IsLoaded = (i == NumStatsVar) ? LOADED : FAILED_LOADING;
		}
		else
			IsLoaded = FAILED_LOADING;
	}
	
	MainView.HSplitTop(3.0f, &MainView, &Button);
	
	UI()->DoLabel(&Button, Localize("Personal statistics"), 20.0f, -1);
	
	MainView.HSplitTop(10.0f, &Button, &MainView);
	// MainView.HSplitTop(20.0f, &Button, &MainView);
	NewLine(&Button, &MainView);
	
	NewLine();
	if(DoButton_CheckBox(&g_Config.m_ClRegisterStats, Localize("Register stats"), g_Config.m_ClRegisterStats, &Button))
		g_Config.m_ClRegisterStats ^= 1;
		
	NewLine();
	if(DoButton_CheckBox(&g_Config.m_ClRegisterStatsPure, Localize("Only register stats from pure games"), g_Config.m_ClRegisterStatsPure, &Button))
		g_Config.m_ClRegisterStatsPure ^= 1;
	
	NewLine();
	
	if(IsLoaded == FAILED_LOADING)
	{
		UI()->DoLabel(&Button, Localize("Cannot load stats: you have never finished a game."), 14.0f, -1);
	}
	else
	{		
		char buf[256];
		char buf2[256];
		PRINT_STATS("Games:", stats.games)
		
		if(stats.games)
			str_format(buf, sizeof(buf), "%.1f%%", 100*(float)stats.won/(float)stats.games);
		else str_format(buf, sizeof(buf), "N/A");
		PRINT_STATS_STR("Won:", buf, "")
		// MainView.HSplitTop(10.0f, &Button, &MainView);

		if(stats.frags > (stats.deaths))
			TextRender()->TextColor(0,1,0,1);
		else if(stats.frags < (stats.deaths))
			TextRender()->TextColor(1,0,0,1);
		else
			TextRender()->TextColor(1,1,0,1);
		if(stats.deaths)
			str_format(buf, sizeof(buf), "%.2f (%d/%d)", (float)stats.frags/(float)(stats.deaths), stats.frags, stats.deaths);
		else str_format(buf, sizeof(buf), "N/A (%d/%d)", stats.frags, stats.deaths);
		PRINT_STATS_STR("K/D Ratio:", buf, "")
		TextRender()->TextColor(1,1,1,1);
		
		MainView.HSplitTop(8.0f, &Button, &MainView);
		NewLine();
		UI()->DoLabel(&Button, Localize("Per game:"), 16.0f, -1);
		if(stats.games)
		{
			PRINT_STATS_MED("Suicides:", stats.suicides, stats.games)
			PRINT_STATS_MED("Flag grabs:", stats.flag_grabs, stats.games)
			PRINT_STATS_MED("Carriers killed:", stats.carriers_killed, stats.games)
			PRINT_STATS_MED("Kills carrying:", stats.kills_carrying, stats.games)
			PRINT_STATS_MED("Deaths carrying:", stats.deaths_carrying, stats.games)
		}
		else
		{
			PRINT_STATS("Suicides:", stats.suicides)
			PRINT_STATS("Flag grabs:", stats.flag_grabs)
			PRINT_STATS("Carriers killed:", stats.carriers_killed)
			PRINT_STATS("Kills carrying:", stats.kills_carrying)
			PRINT_STATS("Deaths carrying:", stats.deaths_carrying)
		}
		
		NewLine();
		MainView.HSplitTop(16.0f, &Button, &MainView);
		
		const char* WeaponsStr[6] = {"      Hammer", "      Pistol", "      Shotgun", "      Rocket", "      Rifle", "      Ninja"};
		Button.VSplitLeft(110.0f, 0, &Button); UI()->DoLabel(&Button, Localize("Frags with:"), 14.0f, -1);
		Button.VSplitLeft(120.0f, 0, &Button); UI()->DoLabel(&Button, Localize("Deaths from:"), 14.0f, -1);
		
		for(int i = 0; i < 6; i++)
		{
			if(!stats.frags_with[i] && !stats.deaths_from[i]) // Unused weapon
				continue;
				
			if(stats.frags)
			{
				if(stats.frags_with[i])
					str_format(buf, sizeof(buf), "%s%.1f%%", 10*stats.frags_with[i] < stats.frags ? "0" : "",
						(float)(100*stats.frags_with[i])/(float)stats.frags);
				else
					str_copy(buf, "    -", sizeof(buf));
			}
			else str_copy(buf, "N/A", sizeof(buf));
			if(stats.deaths)
			{
				if(stats.deaths_from[i])
					str_format(buf2, sizeof(buf2), "%s%.1f%%", 10*stats.deaths_from[i] < stats.deaths ? "0" : "",
						(float)(100*stats.deaths_from[i])/(float)stats.deaths);
				else
					str_copy(buf2, "    -", sizeof(buf2));
			}
			else str_copy(buf2, "N/A", sizeof(buf2));
			
			PRINT_STATS_STR(WeaponsStr[i], buf, buf2)
			
			Graphics()->TextureSet(g_pData->m_aImages[IMAGE_GAME].m_Id);
			Graphics()->QuadsBegin();
			RenderTools()->SelectSprite(g_pData->m_Weapons.m_aId[i].m_pSpriteBody, 0);
			if(i == 5)
				RenderTools()->DrawSprite(30.0f, Button.y+10.0f, 27.0f);
			else
				RenderTools()->DrawSprite(35.0f, Button.y+10.0f, 27.0f);
			Graphics()->QuadsEnd();
		}
	}

	// draw footers	
	MainView.HSplitTop(15.0f, &Button, &MainView);
	NewLine();
	
	Button.HSplitTop(20.0f, &RefreshButton, &MainView);
	Button.HSplitTop(20.0f, &ResetButton, &MainView);
	
	RefreshButton.VSplitLeft(20.0f, 0, &RefreshButton);
	RefreshButton.VSplitLeft(150.0f, &RefreshButton, 0);
	static int s_Refresh = 0;
	if(DoButton_Menu(&s_Refresh, Localize("Refresh"), 0, &RefreshButton))
	{
		IsLoaded = NOT_LOADED;
	}

	ResetButton.VSplitLeft(200.0f, 0, &ResetButton);
	ResetButton.VSplitLeft(150.0f, &ResetButton, 0);
	static int s_Reset = 0;
	if(DoButton_Menu(&s_Reset, Localize("Reset stats"), 0, &ResetButton))
	{
		char Path[512];
		FILE* File;
		
#if defined(CONF_FAMILY_WINDOWS)
		str_format(Path, sizeof(Path), "%s\\stats.cfg", ClientUserDirectory());
#else
		str_format(Path, sizeof(Path), "%s/stats.cfg", ClientUserDirectory());
#endif

		File = fopen(Path, "w+");
		if(!File)
			return;
		for(int j = 0; j < 22; j++)
			fputs("0\n", File);
		fclose(File);
		IsLoaded = NOT_LOADED; // We will refresh the stats the next time
	}
}

#define CREDITS_HEADER(s) 	MainView.HSplitTop(10.0f, &Button, &MainView);\
	MainView.HSplitTop(20.0f, &Button, &MainView);\
	UI()->DoLabel(&Button, Localize(s), 14.0f, 0);
	
#define CREDITS_LINE(s)MainView.HSplitTop(20.0f, &Button, &MainView);\
	UI()->DoLabel(&Button, Localize(s), 12.0f, 0);

/*
void CMenus::RenderSettingsGamerCredits(CUIRect MainView)
{
	CUIRect Button;
	
	MainView.HSplitTop(52.0f, &Button, &MainView);
	UI()->DoLabel(&Button, "Gamer", 38.0f, 0);
	
	CREDITS_HEADER("Programmation")
	CREDITS_LINE("Dunedune")

	CREDITS_HEADER("Patched separated projects")
	CREDITS_LINE("Spl0k (Teecomp)")
	CREDITS_LINE("Sushi Tee (Race)")
	CREDITS_LINE("EdgeCruSher (Teecomp++)")
	
	CREDITS_HEADER("Graphics")
	CREDITS_LINE("Landil (arrows and entities)")
	CREDITS_LINE("Apmox (logo and icon)")
	CREDITS_LINE("Bee (icons)")
	CREDITS_LINE("Sheep (entities)")
	CREDITS_LINE("Puch (clock)")
	
	CREDITS_HEADER("Sounds")
	CREDITS_LINE("Unreal Tournament")
	
	
		CREDITS_HEADER("Programmation support")
		CREDITS_LINE("LordSkelethom")
		CREDITS_LINE("heinrich5991")
		CREDITS_LINE("Vijfhoek")
	
	
	CREDITS_HEADER("Thanks to")
	CREDITS_LINE("The Teeworlds developers for 0.6.0")
	CREDITS_LINE("The whole community for their feedback and encouragements")	
}*/
#endif