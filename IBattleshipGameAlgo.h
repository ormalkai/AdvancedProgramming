#pragma once
#pragma once

#include <utility>

enum class AttackResult {
	Miss, Hit, Sink
};

class IBattleshipGameAlgo {
public:

	virtual ~IBattleshipGameAlgo() = default;
	virtual void setBoard(const char** board, int numRows, int numCols) = 0;
	virtual std::pair<int, int> attack() = 0;
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) = 0;


};