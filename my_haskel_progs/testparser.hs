module Main where
import Control.Monad
import Text.Parsec
import Text.Parsec.Char
import Text.Parsec.String


-- Parsing a whitespace
-- whitespace :: Parser ()
-- whitespace = void $ many $ char 'a'

-- Parsing a character.
-- whitespace :: Parser Char
-- whitespace = char 'a'

whitespace :: Parser String
whitespace = string "stella"

-- runEvalWith :: (String -> Char) -> String -> IO ()
runEvalWith :: String -> IO ()
-- runEvalWith parseExpr input = do
runEvalWith input = do
	case (parse whitespace "simple parser" input) of
		Left err -> print err
		Right xs -> print "yes"
	--let ast = parseExpr input
	putStrLn $ "AST: " 
	-- ++ (show ast)

main :: IO ()
main = do
	putStrLn "Hello World"
	name <- getLine 
	putStrLn ("hello " ++ name)
	runEvalWith name
	return ()

