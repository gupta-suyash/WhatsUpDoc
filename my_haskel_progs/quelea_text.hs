hd :: [(Int,Int)] -> (Int,Int)
hd (x:xs) = x
hd _ = error "hd error"
--(fst $ hd l, foldl (\x y -> x+y) 0 (map snd l))
