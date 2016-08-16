import System.Random
--import System.Random.TF.Gen
import Data.Word

--playnext :: RandomGen g => g -> Word32
--playnext r = fst (next r)

main = do
 --print (playnext 4)
 print (next (mkStdGen 4))
-- print (genRange (mkStdGen 4))
-- print (split (mkStdGen 4))
-- print (5+4)
