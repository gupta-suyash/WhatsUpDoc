-- Checks if a number is prime using guards
primeguard n i  
  | n == i  = putStrLn "Prime"
  | (rem n i == 0) = putStrLn "Not Prime"
  | otherwise = primeguard n (i+1) 
