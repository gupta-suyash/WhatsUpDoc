import Test.QuickCheck

ordered :: [Int] -> Bool
ordered xs = and (zipWith (<=) xs (drop 1 xs))

insert x xs = takeWhile (<x) xs++[x]++dropWhile (<x) xs

prop_Insert2 x = forAll orderedList $ \xs -> ordered (insert x xs)
  where types = x::Int 

