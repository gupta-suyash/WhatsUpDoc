ifprime n i = 
 if n == i 
  then putStrLn "Prime" 
  else 
    if rem n i == 0
      then putStrLn "Not prime"
      else ifprime n (i+1)
