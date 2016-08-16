myfst :: (a,b) -> a
myfst [] = error "Empty tuple"
myfst (a,b) = a
