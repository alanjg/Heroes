#pragma once

#include "../GameCore/entity.grpc.pb.h"

class NetworkClient
{
private:
	int m_playerId;
	std::string m_serverIp;
	std::string m_port;

	std::shared_ptr<grpc::Channel> m_grpcChannel = nullptr;
	std::unique_ptr<heroes::HeroesServer::Stub> m_grpcStub = nullptr;
	grpc::ClientContext* g_grpcContext = nullptr;
	std::mutex m_networkMutex;
	std::thread m_networkThread;
	std::condition_variable m_networkCV;
	std::atomic<int> m_networkDataAvailable = 0;
public:
	NetworkClient(const std::string& serverIp, const std::string& port);
	bool InitNetwork();
	bool SendMoveCommand(int entity, float targetX, float targetY);
	bool SendAttackUnitCommand(int entity, int target);
	bool SendStopCommand(int entity);
	bool SendHoldCommand(int entity);
	void CheckNetworkData();
	std::unique_lock<std::mutex> LockNetworkEntityUpdate();
};

