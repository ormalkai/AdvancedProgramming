set(user1 ormalkai)
set(user2 hilatamam)
set(user3 galsimhi)
set(id1 "203569264")
set(id2 "203521638")
set(id3 "201082708")
set (SOURCE_FILES
	BattleshipAlgoFromFile.h
	BattleshipAlgoFromFile.cpp
	Board.h
	Board.cpp
	Cell.h
	Cell.cpp
	Debug.h
	Debug.cpp
	Destroyer.h
	Destroyer.cpp
	Game.h
	Game.cpp
	main.cpp
	IBattleshipGameAlgo.h
	PlayerAlgo.h
	PlayerAlgoFactory.h
	PlayerAlgoFactory.cpp
	RocketShip.h
	RocketShip.cpp
	RubberShip.h
	RubberShip.cpp
	Ship.h
	Ship.cpp
	ShipFactory.h
	ShipFactory.cpp
	Submarine.h
	Submarine.cpp
	Utils.h
	Utils.cpp
)

### DO NOT CHANGE FROM HERE ON ###

message("user: ${user1}, id: ${id1}")
message("user: ${user2}, id: ${id2}")
message("user: ${user3}, id: ${id3}")

set(EXE_NAME "ex1.${user1}")
add_executable( ${EXE_NAME}
	${SOURCE_FILES}
)
#BUILD_BINARY_OUTPUT_DIRECTORY
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "ex1/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")