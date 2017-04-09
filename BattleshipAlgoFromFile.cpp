#include "BattleshipAlgoFromFile.h"

BattleshipAlgoFromFile::BattleshipAlgoFromFile(int id)
{
	this->setId(id);
}

void BattleshipAlgoFromFile::setBoard(const char** board, int numRows, int numCols)
{
}

std::pair<int, int> BattleshipAlgoFromFile::attack()
{
	return *(new pair<int, int>);
}

void BattleshipAlgoFromFile::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
}
