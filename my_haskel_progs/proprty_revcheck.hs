{-# LANGUAGE ScopedTypeVariables, TemplateHaskell #-}
module Main where

import Test.QuickCheck
--import Test.QuickCheck.Text
--import Test.QuickCheck.All


prop_RevRev xs = reverse (reverse xs) == xs
 where 
  types = xs::[Int]

-- main

return []
main = $quickCheckAll
