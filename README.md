# WhatsUpDoc

This document helps to walk-through the steps for installing the WhatsUpDoc application.

---------------------------------------------------------------------------------------

WhatsUpDoc has been tested on the Microsoft Azure platform and uses a set of APIs from Microsoft Azure and Twilio. This application can be easily installed on any system (Unix, Linux or Mac) and needs a Twilio Account. For the purpose of testing this application we provide a sample Twilio Account. 

Step 1: Set up the Twilio Account
	-- Create a new Twilio Account.
	-- Keep a note of the assigned Twilio Number and Subscription ID. 
	
Step 2: Set up an Microsoft Azure Account and create a new virtual machine. 
	-- For our work we create a Classic Ubuntu Server 14.04 LTS

Step 3: Copy the WhatsUpDoc files (final.py, requirements.txt, installtools.sh) to the server. [In case working from a terminal use "scp" command]
	-- Test command, i.e. copying to the "whatsupdocserver" server.
	* scp final.py whatsupdocserver@whatsupdocserver.cloudapp.net:	
	* scp requirements.txt whatsupdocserver@whatsupdocserver.cloudapp.net:
	* scp installtools.sh whatsupdocserver@whatsupdocserver.cloudapp.net:

Step 4: SSH to the virtual machine
	-- Test command, i.e. logging to the testwhatsup server.

Step 5: Run Installation script - installtools.sh
	* chmod +x installtools.sh
	* ./installtools.sh
	
	Note: in case the shell is not "bash" then the script may need to be run according to specific shell. Additionally script expects "sudo" (root) permission and may prompt for password during installation. In case any package in unsuccessful in installation, please install it manually using the command in the script.

Step 6: Open another terminal and login (ssh) again to whatsupdocserver. Run the ngrok client.
	* ngrok 5000
	-- You can see the ngrok forwarding address. Copy the first till the arrow. For example, if the ngrok forwarding address has following information: 
		http://57177886.ngrok.com -> 127.0.0.1:5000 
	You just need to copy this part: http://57177886.ngrok.com 


Step 7:	Enter your Twilio account and reach the window where the information regarding phone number is there. The general path is:
	-- Click on PRODUCTS at the top left.
	-- Click on PHONE NUMBERS in the drop down menu.
	-- Click on the phone number displayed in red.
	-- Paste the Ngrok forwarding address to 4 places: Request URL and Fallback URL (under Voice) and Request and Fallback URL (under Messaging).
	-- Save it using the option at button left.

Step 8: Switch back to the older terminal and run final.py
	* python finalCleaned.py

Step 9: Make a call and Enjoy !

------------------------------------------------------ xxx ------------------------------------------------------

Additional Note:
-- If you have created a new Twilio account then in the finalCleaned.py you would have to modify the account_sid and auth_token.
-- If you have created a new Azure account then in the finalCleaned.py you would have to modify the clientId and clientSecret.
