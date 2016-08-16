-- Reverse a number n
-- This program requires f = number of digits.
revers :: Int -> Int -> Int
revers n f =
 if n == 0 
  then 0 
  else 
   ((rem n 10) * (10 ^ f)) + revers (quot n 10) (f-1) 

