import Test.QuickCheck

data Point = MkPoint Int Int deriving (Eq, Show)

instance Arbitrary Point where
 arbitrary = do
  x <- arbitrary
  y <- arbitrary
  return (MkPoint x y)

swapPoint :: Point -> Point
swapPoint (MkPoint x y) = MkPoint y x

prop_swapInvolution point = swapPoint (swapPoint point) == point
