-- Swaps two numbers and returns a tuple or list depending of defined type.
-- swap :: Int -> Int -> (Int, Int) 	-- If we want to return a tuple
swap :: Int -> Int -> [Int]		-- If we want to return a list
swap x y =
 [(x-(x-y)), (y+(x-y))]			-- Returns a list of 2 numbers
-- ((x-(x-y)), (y+(x-y)))		-- Returns a tuple

