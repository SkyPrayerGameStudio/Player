/*
 * This file is part of EasyRPG Player.
 *
 * EasyRPG Player is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
 */

// Headers
#include <vector>
#include <sstream>
#include <iomanip>
#include "baseui.h"
#include "cache.h"
#include "input.h"
#include "game_variables.h"
#include "game_switches.h"
#include "game_map.h"
#include "game_system.h"
#include "scene_debug.h"
#include "scene_load.h"
#include "scene_save.h"
#include "player.h"
#include "window_command.h"
#include "window_varlist.h"
#include "window_numberinput.h"
#include "bitmap.h"
#include "game_temp.h"

Scene_Debug::Scene_Debug() {
	Scene::type = Scene::Debug;
}

void Scene_Debug::Start() {
	CreateRangeWindow();
	CreateVarListWindow();
	CreateNumberInputWindow();

	range_window->SetActive(true);
	var_window->SetActive(false);
	var_window->Refresh();
}

void Scene_Debug::Update() {
	range_window->Update();
	if (range_index != range_window->GetIndex()){
		range_index = range_window->GetIndex();
		var_window->UpdateList(range_page * 100 + range_index * 10 + 1);
		var_window->Refresh();
	}
	var_window->Update();

	if (numberinput_window->GetActive())
		numberinput_window->Update();

	if (Input::IsTriggered(Input::CANCEL)) {
		Game_System::SePlay(Game_System::GetSystemSE(Game_System::SFX_Cancel));
		if (range_window->GetActive())
			if (mode == eMain) {
				Scene::Pop();
			} else {
				if (mode == eSwitch) {
					prev_switch_range_index = range_index;
					prev_switch_range_page = range_page;
					range_index = 2;
				} else if (mode == eVariable) {
					prev_variable_range_index = range_index;
					prev_variable_range_page = range_page;
					range_index = 3;
				} else {
					range_index = 0;
				}
				range_page = 0;
				mode = eMain;
				range_window->SetIndex(range_index);
				var_window->SetMode(Window_VarList::eNone);
				UpdateRangeListWindow();
			}
		else if (var_window->GetActive()) {
			var_window->SetActive(false);
			range_window->SetActive(true);
			var_window->Refresh();
		} else if (numberinput_window->GetActive()) {
			numberinput_window->SetVisible(false);
			numberinput_window->SetActive(false);
			var_window->SetActive(true);
		}
	} else if (Input::IsTriggered(Input::DECISION)) {
		var_window->Refresh();
		if (range_window->GetActive()) {
			if (mode == eMain) {
				switch (range_window->GetIndex()) {
					case 0:
						if (Game_Temp::battle_running) {
							Game_System::SePlay(Game_System::GetSystemSE(Game_System::SFX_Buzzer));
						} else {
							Scene::PopUntil(Scene::Map);
							Scene::Push(std::make_shared<Scene_Save>());
						}
						break;
					case 1:
						Scene::Push(std::make_shared<Scene_Load>());
						break;
					case 2:
						Game_System::SePlay(Game_System::GetSystemSE(Game_System::SFX_Decision));
						range_index = prev_switch_range_index;
						range_page = prev_switch_range_page;
						mode = eSwitch;
						var_window->SetMode(Window_VarList::eSwitch);
						var_window->UpdateList(range_page * 100 + range_index * 10 + 1);
						range_window->SetIndex(range_index);
						UpdateRangeListWindow();
						var_window->Refresh();
						break;
					case 3:
						Game_System::SePlay(Game_System::GetSystemSE(Game_System::SFX_Decision));
						range_index = prev_variable_range_index;
						range_page = prev_variable_range_page;
						mode = eVariable;
						var_window->SetMode(Window_VarList::eVariable);
						var_window->UpdateList(range_page * 100 + range_index * 10 + 1);
						range_window->SetIndex(range_index);
						UpdateRangeListWindow();
						var_window->Refresh();
						break;
					default:
						break;
				}
			} else {
				range_window->SetActive(false);
				var_window->SetActive(true);
			}
		} else if (var_window->GetActive()) {
			switch (mode) {
				case eSwitch:
					if (Game_Switches.IsValid(GetIndex())) {
						Game_Switches[GetIndex()] = !Game_Switches[GetIndex()];
					}
					break;
				case eVariable:
					if (Game_Variables.IsValid(GetIndex())) {
						var_window->SetActive(false);
						numberinput_window->SetNumber(Game_Variables[GetIndex()]);
						numberinput_window->SetVisible(true);
						numberinput_window->SetActive(true);
					}
					break;
				default:
					break;
			}
			var_window->Refresh();
		} else if (numberinput_window->GetActive()) {
			Game_Variables[GetIndex()] = numberinput_window->GetNumber();
			numberinput_window->SetActive(false);
			numberinput_window->SetVisible(false);
			var_window->SetActive(true);
			var_window->Refresh();
		}
		Game_Map::SetNeedRefresh(Game_Map::Refresh_All);
	} else if (range_window->GetActive() && Input::IsTriggered(Input::RIGHT)) {
		if (range_page < GetLastPage()) {
			++range_page;
		} else {
			range_page = 0;
		}
		var_window->UpdateList(range_page * 100 + range_index * 10 + 1);
		UpdateRangeListWindow();
		var_window->Refresh();
	} else if (range_window->GetActive() && Input::IsTriggered(Input::LEFT)) {
		if (range_page > 0) {
			--range_page;
		} else {
			range_page = GetLastPage();
		}
		var_window->UpdateList(range_page * 100 + range_index * 10 + 1);
		UpdateRangeListWindow();
		var_window->Refresh();
	}
}

void Scene_Debug::CreateRangeWindow() {

	std::vector<std::string> ranges;
	for (int i = 0; i < 10; i++)
		ranges.push_back("");
	range_window.reset(new Window_Command(ranges, 96));

	range_window->SetHeight(176);
	range_window->SetY(32);
	UpdateRangeListWindow();
}

void Scene_Debug::UpdateRangeListWindow() {
	switch (mode) {
		case eMain:
			{
				auto addItem = [&](int idx, const char* name, bool battle_ok) {
					range_window->SetItemText(idx, name);
					if (!battle_ok && Game_Temp::battle_running) {
						range_window->DisableItem(0);
					}
				};

				int i = 0;
				addItem(i++, "Save", false);
				addItem(i++, "Load", true);
				addItem(i++, "Switch", true);
				addItem(i++, "Variable", true);
				while (i < 10) {
					addItem(i++, "", true);
				}
				return;
			}
			break;
		case eSwitch:
		case eVariable:
			{
				const char* prefix = "???";
				switch (mode) {
					case eSwitch:
						prefix = "Sw[";
						break;
					case eVariable:
						prefix = "Vr[";
						break;
					default:
						break;
				}
				std::stringstream ss;
				for (int i = 0; i < 10; i++){
					ss.str("");
					ss  << prefix
						<< std::setfill('0')
						<< std::setw(4)
						<< (range_page * 100 + i * 10 + 1)
						<< "-"
						<< std::setw(4)
						<< (range_page * 100 + i * 10 + 10) <<
						"]";
					range_window->SetItemText(i, ss.str());
				}
			}
			break;
		default:
			break;
	}
}

void Scene_Debug::CreateVarListWindow() {
	std::vector<std::string> vars;
	for (int i = 0; i < 10; i++)
		vars.push_back("");
	var_window.reset(new Window_VarList(vars));
	var_window->SetX(range_window->GetWidth());
	var_window->SetY(range_window->GetY());
	var_window->SetVisible(false);

	var_window->UpdateList(range_page * 100 + range_index * 10 + 1);
}

void Scene_Debug::CreateNumberInputWindow() {
	numberinput_window.reset(new Window_NumberInput(105, 104,
		Player::IsRPG2k() ? 12 * 7 + 16 : 12 * 8 + 16, 32));
	numberinput_window->SetVisible(false);
	numberinput_window->SetOpacity(255);
	numberinput_window->SetShowOperator(true);
}

int Scene_Debug::GetIndex() {
	return (range_page * 100 + range_index * 10 + var_window->GetIndex() + 1);
}


int Scene_Debug::GetLastPage() {
	switch (mode) {
		case eSwitch:
			return Game_Switches.GetSize() / 100;
		case eVariable:
			return Game_Variables.GetSize() / 100;
		default:
			return 0;
	}
}

