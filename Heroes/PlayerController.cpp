#include "stdafx.h"
#include "PlayerController.h"


PlayerController::PlayerController()
{
}


PlayerController::~PlayerController()
{
}

void PlayerController::UpdateActions()
{

}

void PlayerController::SetPlayer(std::shared_ptr<Player> player)
{
	m_player = player;
}

Player* PlayerController::GetPlayer()
{
	return m_player.get();
}