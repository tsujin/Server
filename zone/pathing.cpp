#include "../common/global_define.h"
#include "../common/event/background_task.h"

#include "client.h"
#include "zone.h"
#include "water_map.h"

extern Zone *zone;

void AdjustRoute(std::list<IPathfinder::IPathNode> &nodes, int flymode, float offset) {
	if (!zone->HasMap() || !zone->HasWaterMap()) {
		return;
	}

	for (auto &node : nodes) {
		if (flymode == GravityBehavior::Ground || !zone->watermap->InLiquid(node.pos)) {
			auto best_z = zone->zonemap->FindBestZ(node.pos, nullptr);
			if (best_z != BEST_Z_INVALID) {
				node.pos.z = best_z + offset;
			}
		}
	}
}

void CullPoints(std::vector<FindPerson_Point> &points) {
	if (!zone->HasMap()) {
		return;
	}

	size_t i = 0;
	for (; i < points.size(); ++i) {
		auto &p = points[i];

		for (;;) {
			if (i + 2 >= points.size()) {
				return;
			}

			if (points.size() < 36) {
				return;
			}

			auto &p1 = points[i + 1];
			auto &p2 = points[i + 2];

			if (zone->zonemap->CheckLoS(glm::vec3(p.x, p.y, p.z), glm::vec3(p2.x, p2.y, p2.z))) {
				points.erase(points.begin() + i + 1);
				Log(Logs::General, Logs::Status, "Culled find path point %u, connecting %u->%u instead.", i + 1, i, i + 2);
			}
			else {
				break;
			}
		}
	}
}

void Client::SendPathPacket(const std::vector<FindPerson_Point> &points) {
	EQ::BackgroundTask task([](EQEmu::Any &data) {
		auto &points = EQEmu::any_cast<std::vector<FindPerson_Point>&>(data);
		CullPoints(points);
	}, [this](EQEmu::Any &data) {
		auto &points = EQEmu::any_cast<std::vector<FindPerson_Point>&>(data);

		if (points.size() < 2) {
			if (Admin() > 10) {
				Message(MT_System, "Too few points");
			}

			EQApplicationPacket outapp(OP_FindPersonReply, 0);
			QueuePacket(&outapp);
			return;
		}

		if (points.size() > 36) {
			if (Admin() > 10) {
				Message(MT_System, "Too many points %u", points.size());
			}

			EQApplicationPacket outapp(OP_FindPersonReply, 0);
			QueuePacket(&outapp);
			return;
		}

		if (Admin() > 10) {
			Message(MT_System, "Total points %u", points.size());
		}

		int len = sizeof(FindPersonResult_Struct) + (points.size() + 1) * sizeof(FindPerson_Point);
		auto outapp = new EQApplicationPacket(OP_FindPersonReply, len);
		FindPersonResult_Struct* fpr = (FindPersonResult_Struct*)outapp->pBuffer;

		std::vector<FindPerson_Point>::iterator cur, end;
		cur = points.begin();
		end = points.end();
		unsigned int r;
		for (r = 0; cur != end; ++cur, r++) {
			fpr->path[r] = *cur;

		}
		//put the last element into the destination field
		--cur;
		fpr->path[r] = *cur;
		fpr->dest = *cur;

		FastQueuePacket(&outapp);
	}, points);
}
