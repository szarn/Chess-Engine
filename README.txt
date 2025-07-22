Idea: Programming Engine using C++ & SQL
	C++ -> basic interface with engine calculations
	SQL -> database with historic games, will grab data to replay. Stores data against played games



Chess Engine Design:
	
	1.) Engine can make legal moves
		To Determine legal moves, we must determine the piece -> view squares possible to travel -> check if squares are blocked (if friendly pieces = cant take: if enenmy = capture)
		We must design a board 
			Ideas: 2D matrix
				bitboard approach		

		We must store locations of all pieces on the board
		We must give rules to each piece
		It is one move per player turn
	
	2.) Engine makes "good moves"
		Give values to each piece and corresponding squares on the board
