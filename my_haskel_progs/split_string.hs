import Test.QuickCheck
import Data.List (intersperse)

split c [] = []
split c xs = xs' : if null xs'' then [] else split c (tail xs'')
 where xs' = takeWhile (/=c) xs
       xs''= dropWhile (/=c) xs


unsplit :: Char -> [String] -> String
unsplit c = concat . intersperse [c]

-- show
-- examples = [('@', "pbv@dcc.fc.up.pt"),('/',"/usr/include")]
prop_split_inv c xs = unsplit c (split c xs) == xs

--test (c,xs) = unwords ["split", show c, show xs, "=", show ys]
-- where ys = split c xs

--main = mapM_ (putStrLn.test) examples
main = quickCheck prop_split_inv
