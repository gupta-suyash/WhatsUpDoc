#!/bin/bash

sudo apt-get install python-pip
sudo apt-get update
sudo apt-get install python-pip
sudo pip install twilio
sudo apt-get install ngrok-client
sudo pip install virtualenv
mkdir new_app
cd new_app
virtualenv -p /usr/bin/python2.7 venv
source venv/bin/activate
cd venv
cp ~/requirements.txt .
pip install -r requirements.txt
pip install py-bing-search
pip install html2text
sudo apt-get install python-dev
sudo apt-get install libxml2-dev libxslt-dev
sudo apt-get install libjpeg-dev zlib1g-dev libpng12-dev
pip install newspaper
curl https://raw.githubusercontent.com/codelucas/newspaper/master/download_corpora.py | python
sudo pip uninstall requests
pip install requests==2.3.0
pip install --upgrade httpie
