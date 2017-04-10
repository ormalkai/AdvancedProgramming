
#include "Board.h"


char** Board::toCharMat()
{
	char** ret = new char*[m_rows];

	for (int i = 0; i < m_rows; i++)
	{
		ret[i] = new char[m_cols];

		for (int j = 0; j < m_cols; j++)
		{
			ret[i][j] = m_boardData[i][j].getSign();
		}
	}

	return ret;
}

}