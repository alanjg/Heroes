#include "stdafx.h"

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "../GameCore/entity.grpc.pb.h"
#include "../GameCore/Game.h"
#pragma comment(lib,"WS2_32")
#ifdef _DEBUG
#pragma comment(lib, "zlibd")
#pragma comment(lib,"libprotobufd")
#pragma comment(lib,"libprotobuf-lited")
#else
#pragma comment(lib, "zlib")
#pragma comment(lib,"libprotobuf")
#pragma comment(lib,"libprotobuf-lite")
#endif // DEBUG

#ifdef _WIN64
#pragma comment(lib,"grpc++_reflection")
#pragma comment(lib,"grpcpp_channelz")
#endif

#pragma comment(lib, "address_sorting")
#pragma comment(lib, "upb")
#pragma comment(lib, "upb_fastdecode")
#pragma comment(lib, "upb_handlers")
#pragma comment(lib, "upb_json")
#pragma comment(lib, "upb_pb")
#pragma comment(lib, "upb_reflection")
#pragma comment(lib, "upb_textformat")
#pragma comment(lib, "re2")
#pragma comment(lib, "cares")
#pragma comment(lib,"abseil_dll")
#pragma comment(lib,"gpr")
#pragma comment(lib,"grpc")
#pragma comment(lib,"grpc_csharp_ext")
#pragma comment(lib,"grpc_plugin_support")
#pragma comment(lib,"grpc_unsecure")
#pragma comment(lib,"grpc_upbdefs")
#pragma comment(lib,"grpc++")
#pragma comment(lib,"grpc++_alts")
#pragma comment(lib,"grpc++_error_details")
#pragma comment(lib,"grpc++_unsecure")
#pragma comment(lib, "libssl")
#pragma comment(lib, "libcrypto")

class HeroesServerImpl final : public heroes::HeroesServer::Service 
{
public:
    explicit HeroesServerImpl() :
		game(new Game())
	{
		game->LoadEntityDefinitions();

		clientsToProcess = 0;
		m_clientCount = 0;
		targetPlayerCount = 2;
    }

    grpc::Status UpdateState(grpc::ServerContext* context, const heroes::UpdateStateRequest* request, grpc::ServerWriter<heroes::ServerState>* writer) override 
	{		
		std::cout << "UpdateState called\n";
		int playerId = request->playerid();

        while (true) 
		{			
			heroes::ServerState state;
			{
				// wait for frame from game loop
				std::unique_lock<std::mutex> lk(frameDataReadyMutex);
				frameDataReadyCV.wait(lk, [this, playerId] { return this->playersToProcessNewState[playerId]; });

				//std::cout << "Updating state for player " << playerId << std::endl;
				playersToProcessNewState[playerId] = false;
				
				state.CopyFrom(serverState);
				clientsToProcess--;
			}
			
			//std::cout << "UpdateState complete" << std::endl;
			frameDataReadyCV.notify_all();
			bool result = writer->Write(state);
			//std::cout << "UpdateState wrote to stream writer\n";
			if (false)
			{
				break;
			}
        }

        return grpc::Status::OK;
    }

	grpc::Status ProcessClientCommand(grpc::ServerContext* context, const heroes::ClientCommand* command, heroes::ClientCommandResult* response) override 
	{
		std::cout << "Got client command\n";
		std::lock_guard<std::mutex> lock(commandMutex);
		std::cout << "Queued client command\n";
		pendingCommands.push_back(*command);
		return grpc::Status::OK;
	}

	grpc::Status CreateGame(grpc::ServerContext* context, const heroes::CreateGameRequest* request, heroes::CreateGameResponse* response) override 
	{
		return grpc::Status::OK;
	}

	grpc::Status JoinGame(grpc::ServerContext* context, const heroes::JoinGameRequest* request, heroes::JoinGameResponse* response) override 
	{
		std::cout << "JoinGame called\n";
		std::unique_lock<std::mutex> lk(joinGameMutex);
		int gameId = request->gameid();

		int playerId = game->AddPlayer(request->player());
		int teamId = game->CreateTeam(playerId);
		response->set_playerid(playerId);
		playersToProcessNewState[playerId] = false;
		m_clientCount++;
		std::cout << "JoinGame complete\n";
		gameStartCV.notify_one();
		std::cout << "JoinGame cv notified\n";
		return grpc::Status::OK;
	}

	void RunGame()
	{
		std::cout << "RunGame started\n";
		std::unique_lock<std::mutex> lk(joinGameMutex);
		gameStartCV.wait(lk, [this] { return this->m_clientCount == targetPlayerCount; });
		std::cout << "RunGame at target player count\n";
		
		game->CreateGame1();

		const double serverFrameDuration = 1 / 60.0; // 60 FPS
		const int serverFrameDurationMs = (int)(1000 * serverFrameDuration);
		auto lastTime = std::chrono::steady_clock::now();
		auto startTime = lastTime;
		for (;;)
		{
			auto currentTime = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsedTime = currentTime - lastTime;
			float elapsedSeconds = elapsedTime.count();
			std::chrono::duration<float> totalElapsedTime = currentTime - startTime;
			double totalElapsedSeconds = totalElapsedTime.count();

			while (elapsedSeconds < serverFrameDuration)
			{
				ProcessCommands();
				Sleep(serverFrameDurationMs / 3);
				currentTime = std::chrono::steady_clock::now();
				elapsedTime = currentTime - lastTime;
				elapsedSeconds = elapsedTime.count();
				totalElapsedTime = currentTime - startTime;
				totalElapsedSeconds = totalElapsedTime.count();
			}

			lastTime = currentTime;
			{
				ProcessCommands();
				// wait for frame from game loop
				std::unique_lock<std::mutex> lk(frameDataReadyMutex);

				//std::cout << "Updating game state\n";
				game->UpdateGame(elapsedSeconds);
				game->WriteServerState(serverState);
				// frame is done, update clients.
				clientsToProcess = m_clientCount;
				for (auto it = game->players.begin(); it != game->players.end(); it++)
				{
					playersToProcessNewState[(*it)->id] = true;
				}
				
				frameDataReadyCV.notify_all();
				//std::cout << "Updating game state cv notified\n";
				frameDataReadyCV.wait(lk, [this] { return this->clientsToProcess == 0; });
				//std::cout << "Updating game state complete\n";
			}
		}
	}

	void ProcessCommands()
	{
		std::unique_lock<std::mutex> lk(commandMutex);
		for (auto command = pendingCommands.begin(); command != pendingCommands.end(); command++)
		{
			std::cout << "got command " << command->command() << " " << command->unitid() << " " << command->targetpointx() << " " << command->targetpointz() << std::endl;
			//process command
			int unitId = command->unitid();
			EntityCommand ecommand;
			switch (command->command())
			{
			case heroes::ClientCommand_Command_ATTACK_UNIT:
				ecommand = EntityCommand::ATTACK_UNIT;
				break;
			case heroes::ClientCommand_Command_ATTACK_POINT:
				ecommand = EntityCommand::ATTACK_POINT;
				break;
			case heroes::ClientCommand_Command_MOVE_TO_POINT:
				ecommand = EntityCommand::MOVE_TO_POINT;
				break;
			case heroes::ClientCommand_Command_STOP:
				ecommand = EntityCommand::STOP;
				break;
			default:
				ecommand = EntityCommand::NONE;
				break;
			}
			EntityCommandData commandData;
			commandData.targetEntityId = command->targetid();
			commandData.targetX = command->targetpointx();
			commandData.targetY = command->targetpointy();
			commandData.targetZ = command->targetpointz();
			
			game->ProcessCommand(unitId, ecommand, commandData);
		}
		pendingCommands.clear();
	}

private:
	int clientsToProcess;
    std::mutex frameDataReadyMutex;
	std::mutex commandMutex;
	std::mutex joinGameMutex;
	std::condition_variable gameStartCV;
	std::condition_variable frameDataReadyCV;
	heroes::ServerState serverState;
	std::unordered_map<int, std::string> m_players;
	std::unordered_map<int, bool> playersToProcessNewState;
	
	int targetPlayerCount;
	int m_clientCount;
	std::vector<heroes::ClientCommand> pendingCommands;
	std::unique_ptr<Game> game;
};

int main()
{
	std::string server_address("0.0.0.0:50051");
	HeroesServerImpl service;

	grpc::ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	std::thread gameThread(&HeroesServerImpl::RunGame, &service);
	server->Wait();
}