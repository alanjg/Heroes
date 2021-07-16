#include "stdafx.h"
#include "NetworkClient.h"
#include "WinMain.h"

#include "Renderer.h"
#include "Entity.h"
#include "Camera.h"
#include "Game.h"
#include "InputManager.h"
#include "File.h"
#include "SelectionManager.h"
#include "SkinnedMeshInstance.h"
#include "StaticMeshInstance.h"
#include "ResourceManager.h"
#include "../GameCore/EntityDefinitionManager.h"

NetworkClient::NetworkClient(const std::string& serverIp, const std::string& port)
	:m_serverIp(serverIp), m_port(port)
{

}

bool NetworkClient::InitNetwork()
{
	std::ostringstream connectionTarget;
	connectionTarget << m_serverIp << ":" << m_port;
	grpc::ClientContext context;
	m_grpcChannel = grpc::CreateChannel(connectionTarget.str(), grpc::InsecureChannelCredentials());
	m_grpcStub = heroes::HeroesServer::NewStub(m_grpcChannel);
	heroes::JoinGameRequest request;
	request.set_gameid(555);
	heroes::JoinGameResponse response;
	grpc::Status status = m_grpcStub->JoinGame(&context, request, &response);
	if (!status.ok()) {
		return false;
	}
	m_playerId = response.playerid();
	std::thread networkThread([this]() {
		grpc::ClientContext context2;
		heroes::UpdateStateRequest request;
		request.set_playerid(m_playerId);
		std::unique_ptr<grpc::ClientReader<heroes::ServerState>> reader(m_grpcStub->UpdateState(&context2, request));
		heroes::ServerState serverState;
		while (reader->Read(&serverState))
		{
			m_networkDataAvailable++;
			std::unique_lock<std::mutex> lock(m_networkMutex);
			for (int i = 0; i < serverState.entities_size(); i++)
			{
				const heroes::Entity& entity = serverState.entities(i);

				// Add entity if new
				auto it = g_game->entityMap.find(entity.id());
				if (it == g_game->entityMap.end())
				{
					SkinnedMeshInstance* mesh;
					EntityDefinition* entityDefinition = g_entityDefinitionManager->GetEntity(entity.entitytype());
					mesh = g_renderer->CreateSkinnedMeshInstance(entityDefinition->modelName);
					/*
					if (entity.entitytype() == 0)
					{
						mesh = g_renderer->CreateSkinnedMeshInstance("Footman");
					}
					else if (entity.entitytype() == 1)
					{
						mesh = g_renderer->CreateSkinnedMeshInstance("Demon");
					}
					else
					{
						mesh = g_renderer->CreateSkinnedMeshInstance("BlackDragon");
					}*/
					Entity* unit = new Entity();
					unit->canMove = true;
					mesh->BindEntity(unit);
					std::shared_ptr<Entity> entityPtr(unit);
					unit->Initialize(15, 0, 90, mesh);
					unit->entityId = entity.id();
					g_game->entities.push_back(entityPtr);
					auto ret = g_game->entityMap.insert(make_pair(entity.id(), entityPtr));
					it = ret.first;
				}
				it->second->x = entity.positionx();
				it->second->y = entity.positionz();
				it->second->orientation = entity.orientation();
				if (entity.status() == heroes::Entity_Status_MOVING && !it->second->isWalking)
				{
					it->second->WalkToPoint(entity.targetpointx(), entity.targetpointz());
				}
				if (entity.status() == heroes::Entity_Status_ATTACKED && !it->second->isAttacked)
				{
					it->second->GetHit();
				}
				if (entity.status() == heroes::Entity_Status_DYING && !it->second->isDying)
				{
					it->second->Die();
				}
				if (entity.status() == heroes::Entity_Status_IDLE)
				{
					if (it->second->isWalking || it->second->isAttacking)
					{
						it->second->Stop();
					}
				}
				if (entity.status() == heroes::Entity_Status_ATTACKING && !it->second->isAttacking)
				{
					it->second->Attack();
				}
				
			}
			m_networkDataAvailable--;
			m_networkCV.notify_one();
		}
		grpc::Status status = reader->Finish();
		});
	networkThread.detach();
	return true;
}

bool NetworkClient::SendMoveCommand(int entity, float targetX, float targetY)
{
	grpc::ClientContext context;
	heroes::ClientCommand request;
	request.set_playerid(m_playerId);
	request.set_unitid(entity);
	request.set_command(heroes::ClientCommand_Command_MOVE_TO_POINT);
	request.set_targetpointx(targetX);
	request.set_targetpointy(0);
	request.set_targetpointz(targetY);
	heroes::ClientCommandResult result;
	grpc::Status status = m_grpcStub->ProcessClientCommand(&context, request, &result);
	if (!status.ok()) {
		return false;
	}
	return true;
}

bool NetworkClient::SendAttackUnitCommand(int entity, int target)
{
	grpc::ClientContext context;
	heroes::ClientCommand request;
	request.set_playerid(m_playerId);
	request.set_unitid(entity);
	request.set_command(heroes::ClientCommand_Command_ATTACK_UNIT);
	request.set_targetid(target);
	heroes::ClientCommandResult result;
	grpc::Status status = m_grpcStub->ProcessClientCommand(&context, request, &result);
	if (!status.ok()) {
		return false;
	}
	return true;
}

bool NetworkClient::SendStopCommand(int entity)
{
	grpc::ClientContext context;
	heroes::ClientCommand request;
	request.set_playerid(m_playerId);
	request.set_unitid(entity);
	request.set_command(heroes::ClientCommand_Command_STOP);
	heroes::ClientCommandResult result;
	grpc::Status status = m_grpcStub->ProcessClientCommand(&context, request, &result);
	if (!status.ok()) {
		return false;
	}
	return true;
}

bool NetworkClient::SendHoldCommand(int entity)
{
	grpc::ClientContext context;
	heroes::ClientCommand request;
	request.set_playerid(m_playerId);
	request.set_unitid(entity);
	request.set_command(heroes::ClientCommand_Command_HOLD);
	heroes::ClientCommandResult result;
	grpc::Status status = m_grpcStub->ProcessClientCommand(&context, request, &result);
	if (!status.ok()) {
		return false;
	}
	return true;
}


void NetworkClient::CheckNetworkData()
{
	if (m_networkDataAvailable > 0)
	{
		std::unique_lock<std::mutex> lock(m_networkMutex);
		m_networkCV.wait(lock, [this] { return m_networkDataAvailable == 0; });
	}
}

std::unique_lock<std::mutex> NetworkClient::LockNetworkEntityUpdate()
{
	return std::unique_lock<std::mutex>(m_networkMutex);
}