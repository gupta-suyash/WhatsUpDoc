import Test.QuickCheck

ordered :: [Int] -> Bool
ordered xs = and (zipWith (<=) xs (drop 1 xs))

insert x xs = takeWhile (<x) xs++[x]++dropWhile (<x) xs

prop_Insert :: Int -> [Int] -> Property
prop_Insert x xs = ordered xs ==> ordered (insert x xs)
