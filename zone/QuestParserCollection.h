/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2006  EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _EQE_QUESTPARSERCOLLECTION_H
#define _EQE_QUESTPARSERCOLLECTION_H

#include "../common/types.h"
#include "../common/Item.h"

#include "masterentity.h"
#include "QuestInterface.h"

#include <string.h>
#include <string>
#include <list>
#include <map>

#define QuestFailedToLoad 0xFFFFFFFF
#define QuestUnloaded 0x00

class QuestParserCollection {
public:
	QuestParserCollection();
	~QuestParserCollection();

	void RegisterQuestInterface(QuestInterface *qi, std::string ext);

	void AddVar(std::string name, std::string val);
	void ReloadQuests(bool reset_timers = true);

	bool HasQuestSub(uint32 npcid, const char *subname);
	bool PlayerHasQuestSub(const char *subname);
	bool SpellHasQuestSub(uint32 spell_id, const char *subname);
	bool ItemHasQuestSub(ItemInst *itm, const char *subname);

	int EventNPC(QuestEventID evt, NPC* npc, Mob *init, std::string data, uint32 extra_data);
	int EventPlayer(QuestEventID evt, Client *client, std::string data, uint32 extra_data);
	int EventItem(QuestEventID evt, Client *client, ItemInst *item, uint32 objid, uint32 extra_data);
	int EventSpell(QuestEventID evt, NPC* npc, Client *client, uint32 spell_id, uint32 extra_data);

private:
	bool HasQuestSubLocal(uint32 npcid, const char *subname);
	bool HasQuestSubGlobal(const char *subname);
	bool PlayerHasQuestSubLocal(const char *subname);
	bool PlayerHasQuestSubGlobal(const char *subname);

	int EventNPCLocal(QuestEventID evt, NPC* npc, Mob *init, std::string data, uint32 extra_data);
	int EventNPCGlobal(QuestEventID evt, NPC* npc, Mob *init, std::string data, uint32 extra_data);
	int EventPlayerLocal(QuestEventID evt, Client *client, std::string data, uint32 extra_data);
	int EventPlayerGlobal(QuestEventID evt, Client *client, std::string data, uint32 extra_data);

	QuestInterface *GetQIByNPCQuest(uint32 npcid, std::string &filename);
	QuestInterface *GetQIByGlobalNPCQuest(std::string &filename);
	QuestInterface *GetQIByPlayerQuest(std::string &filename);
	QuestInterface *GetQIByGlobalPlayerQuest(std::string &filename);
	QuestInterface *GetQIBySpellQuest(uint32 spell_id, std::string &filename);
	QuestInterface *GetQIByItemQuest(std::string item_script, std::string &filename);

	std::map<uint32, QuestInterface*> _interfaces;
	std::map<uint32, std::string> _extensions;
	std::list<QuestInterface*> _load_precedence;

	//0x00 = Unloaded
	//0xFFFFFFFF = Failed to Load
	std::map<uint32, uint32> _npc_quest_status;
	uint32 _global_npc_quest_status;
	uint32 _player_quest_status;
	uint32 _global_player_quest_status;
	std::map<uint32, uint32> _spell_quest_status;
	std::map<std::string, uint32> _item_quest_status;
};

extern QuestParserCollection *parse;

#endif

