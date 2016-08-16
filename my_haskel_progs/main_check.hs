testprg = do
 print (1+4)

main = do
 testprg 
 a <- getLine
 putStrLn a

