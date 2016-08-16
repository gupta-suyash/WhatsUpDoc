import Test.QuickCheck
import System.Random

--mygen :: (Random a, Num a) => Gen (a, a)
mygen :: Gen (Int, Int)
mygen = do
 i <- choose (0,10)
 j <- choose (0,10)
 return (i,j)

prop_mysymmetry = forAll mygen $ \(m,n) -> m+n == n+m
