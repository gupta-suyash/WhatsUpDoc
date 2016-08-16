maxim :: (Ord a) => a -> [a] -> a
maxim m [] = error "empty list yo"
maxim m [x] = x
maxim m (x:y:xs) = if x > y then (maxim x [xs]) else (maxim y [xs])
