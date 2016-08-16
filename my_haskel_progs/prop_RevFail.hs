import Test.QuickCheck

prop_RevFail xs = reverse xs == xs
 where 
  types = xs::[Int]
