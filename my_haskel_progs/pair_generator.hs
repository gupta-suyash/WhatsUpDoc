pairgen = do 
 i <- [1,2]
 j <- [1..4]
 return (i,j)

main = do
 print pairgen 
