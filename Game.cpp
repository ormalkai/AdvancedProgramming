#include <codecvt>
#include <windows.h>
#include "Game.h"
#include "Utils.h"
#include "Debug.h"
#include "Board.h"


using namespace std;


Game::Game(Board& board, unique_ptr<IBattleshipGameAlgo> algoA, unique_ptr<IBattleshipGameAlgo> algoB) : m_isGameOver(false),
                                                                                                         m_isQuiet(false), m_currentPlayerIndex(MAX_PLAYER), m_otherPlayerIndex(MAX_PLAYER), m_winner(TIE_WINNER_ID), m_depth(-1), m_rows(-1), m_cols(-1)
{
	// invoke copy constructor!
	m_board = board;
	// Init players
	m_players.push_back(move(algoA));
	m_players.push_back(move(algoB));
}

Game::~Game()
{
}

void Game::init()
{
	Board boardA, boardB;
	m_board.splitToPlayersBoards(boardA, boardB);

	m_players[PLAYER_A]->setBoard(boardA);
	m_players[PLAYER_B]->setBoard(boardB);

	m_players[PLAYER_A]->setPlayer(PLAYER_A);
	m_players[PLAYER_B]->setPlayer(PLAYER_B);
}


AttackRequestCode Game::requestAttack(Coordinate& req)
{
	if (ARC_FINISH_REQ == req.depth && ARC_FINISH_REQ == req.row && ARC_FINISH_REQ == req.col)
		if (true == m_finishedAttackPlayer[m_otherPlayerIndex])
			return ARC_GAME_OVER;
		else
		{
			m_finishedAttackPlayer[m_currentPlayerIndex] = true;
			return ARC_FINISH_REQ;
		}
	else if (req.depth < 0 || req.depth > m_board.depth() ||
		req.row < 0 || req.row > m_board.rows() ||
		req.col < 0 || req.col > m_board.cols())
		return ARC_ERROR;
	else
		return ARC_SUCCESS;
}


void Game::startGame()
{
	init();
	auto& shipsPerPlayer = m_board.shipsPerPlayer();

	m_board.printBoard();

	m_currentPlayerIndex = PLAYER_A;
	m_otherPlayerIndex = PLAYER_B;

	// Game loop
	while (!m_isGameOver)
	{
		Coordinate attackReq = m_players[m_currentPlayerIndex]->attack();
		DBG(Debug::DBG_DEBUG, "Player [%d] attack (%d, %d, %d)", m_currentPlayerIndex, attackReq.depth, attackReq.row, attackReq.col);

		// Check attack request 
		AttackRequestCode arc = requestAttack(attackReq);
		switch (arc)
		{
		case ARC_GAME_OVER:
			DBG(Debug::DBG_INFO, "Game Over For both players - No more attack requests");
			m_isGameOver = true;
			m_winner = TIE_WINNER_ID;
			continue;
		case ARC_FINISH_REQ:
			proceedToNextPlayer();
			continue;
		case ARC_ERROR:
			DBG(Debug::DBG_INFO, "Attack failed ! values: %d-%d-%d. Skipping.. arc[%d]", attackReq.depth, attackReq.row, attackReq.col, arc);
			break;
		default:
			break;
		}

		shared_ptr<Cell> attackedCell = m_board.get(attackReq);
		attackedCell->hitCell();

		AttackResult attackResult;

		// Check attack result
		switch (attackedCell->getStatus())
		{
		case Cell::ALIVE:
			{
				attackedCell->setStatus(Cell::DEAD);

				// Notify the user his ridiculous mistake
				if (attackedCell->getPlayerIndexOwner() == m_currentPlayerIndex)
				{
					DBG(Debug::DBG_INFO, "You bombed youself, U R probably an idiot..");
				}

				shared_ptr<Ship> pShip = attackedCell->getShip();
				pShip->executeAttack();

				if (pShip->isShipAlive())
				{
					attackResult = AttackResult::Hit;
				}
				else
				{
					attackResult = AttackResult::Sink;

					// Hit myself
					if (attackedCell->getPlayerIndexOwner() == m_currentPlayerIndex)
					{
						m_playerScore[m_otherPlayerIndex] += pShip->getValue();
						shipsPerPlayer[m_currentPlayerIndex]--;
					}
					else
					{
						// Update score for player
						m_playerScore[m_currentPlayerIndex] += pShip->getValue();

						// Update number of alive ships
						shipsPerPlayer[m_otherPlayerIndex]--;
					}
				}
			}
			break;
		case Cell::DEAD:
			{
				DBG(Debug::DBG_INFO, "This cell was already bombed, go to sleep bro...");
				if (attackedCell->getShip()->isShipAlive())
					attackResult = AttackResult::Hit;
				else
					attackResult = AttackResult::Miss;
			}
			break;
		case Cell::FREE:
		default:
			attackResult = AttackResult::Miss;
		}

		m_board.printAttack(m_currentPlayerIndex, attackReq.row, attackReq.col, attackReq.depth, attackResult);

		// Notify for all players
		for (int i = 0; i < NUM_OF_PLAYERS; i++)
		{
			m_players[i]->notifyOnAttackResult(m_currentPlayerIndex, attackReq, attackResult);
			DBG(Debug::DBG_DEBUG, "Notify Player [%d] attack (%d, %d, %d) [%d]", i, attackReq.depth, attackReq.row, attackReq.col, attackResult);
		}

		// If game over break
		if (Utils::isExistInVec(shipsPerPlayer, 0))
		{
			m_isGameOver = true;
			m_winner = m_currentPlayerIndex;
			DBG(Debug::DBG_INFO, "Game over - 0 ships remaining");
		}
		// If attack failed - iter.next
		// If iter == last do iter = begin

		// Update next turn
		if (AttackResult::Miss == attackResult ||
			attackedCell->getPlayerIndexOwner() == m_currentPlayerIndex)
		{
			proceedToNextPlayer();
		}
	} // Game Loop
}

bool Game::isGameOver() const
{
	return m_isGameOver;
}

Game::GameResult Game::getGameResult()
{
	return GameResult(m_winner, m_playerScore[PLAYER_A], m_playerScore[PLAYER_B]);
}
