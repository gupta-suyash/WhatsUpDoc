# WhatsUpDoc code.
#
# @author: Evan Desantola
# @author: Suyash Gupta
# @author: Pranjal Daga

from flask import Flask, request, redirect, session
import twilio.twiml
import argparse
import urllib
import re
import operator
import time, timeit
import string
import httplib, json, urllib, urllib2
import cookielib
import os, sys
from py_bing_search import PyBingSearch
import html2text
from newspaper import Article
import unicodedata
import base64
import urllib2
from twilio.rest import TwilioRestClient 

global MORE #used to track the length of text sent back to user
MORE=50
global COUNTER #keeps tracks of the number of errors that the program has had trying to interpret user input
COUNTER=0

#Ignores security warning so that we can access all our APIs.  We have resolved this security problem in through the installation of the proper version of requests
import requests.packages.urllib3
requests.packages.urllib3.disable_warnings()

app = Flask(__name__)


class Payload(object):
    def __init__(self, j):
        self.__dict__ = json.loads(j)
#Summarizes a text stored in the local directory filename to numWords words
def summary(filename, numWords):
    print "\nCalling summary with arguments filename: {} and numWords:".format(filename, str(numWords))
    start = timeit.default_timer()

    f = open(filename, 'r')

    p = f.read()
    sentenceEnders = re.compile('.*?[\?|\.|\!]')
    result = sentenceEnders.findall(p)

    n = len(result)
    Matrix = [[0 for x in range(n)] for x in range(n)]
    Sum = [0 for x in range(n)]

    for i in range(0, n):
        for j in range(0, n):
            if i != j:
                s1 = set(result[i].split(" "))
                s2 = set(result[j].split(" "))
                Matrix[i][j] = len(s1.intersection(s2))
            Sum[i] = Sum[i] + Matrix[i][j]
    TempSum = Sum

    count = 0
    file = open('summaryResults.txt', 'w')

    for y in range(0, 9):
        max_index, max_value = max(enumerate(Sum), key=operator.itemgetter(1))

        n = len(result[max_index].split(" "))

        count = count + n
        if count < numWords:
            file.write(result[max_index])
        else:
            break

        Sum[max_index] = 0

    stop = timeit.default_timer()

    print "\nClosing summary. The Runtime of summary was:", stop - start ,"seconds"
    #......................................................................................................................#
    f.close()

#parses mayoclinic article
def parseMayo(url, symptom):
    print "\nCalling parseMayo with arguments url: {} and symptom:".format(str(url), str(symptom))

    #use newspaper to get meaningful article text... we don't like d'em ads in these here parts
    article=Article(url)
    article.download()
    article.parse()

    #formats article text and removes most unicode
    textOut=((str)(repr(article.text))).replace("\\n","\n")
    textOut2=textOut[2:-2].encode('ascii', 'ignore')
    textOutList=[]
    for each in textOut2:
        if not "\\u" in each:
            textOutList.append(each)
        else:
            textOutList.append(" \n ")
    textOut2="".join(textOutList)
    textOut2=str(unicodedata.normalize("NFKD", unicode(textOut2)).encode('ascii', 'ignore'))

   
    #summarizer built to require file input.  Future versions will optimize this
    textOut2= ''.join([str(i) if ord(i) < 128 else ' ' for i in textOut2])

    for each in textOut2:
        if not each in "\\1234567890":
            textOutList.append(each)
        else:
            textOutList.append(" \n ")
    textOut2="".join(textOutList)
    textOutList=textOut2.split("\n")
    
    textOut2= ''.join([str(each) if not ("e-newsletter" in each or "Mayo" in each) else "\n" for each in textOutList])

    with open("mayoTextOut.txt", "w") as outfile:
         outfile.write(str(textOut2).replace(" u ",""))
    print "\nparseMayo complete"


"""Bing search, modified version of BingGrag from prior version
BingGrag written during BostonHacks
bingSearch will search bing with what is passed in linkfile and write first result URL to text file.  It also returns it.  Consider cleaning this so that only what is necessary is used
Uses Microsoft Bing APIs"""
def bingSearch(linkfile):
    print "\nCalling bingSearch with arguments linkfile: {}:".format(str(linkfile))
    #Sanitize input
    linkfile = linkfile.replace("^", "|")

    bing=PyBingSearch('MsYC/eW39AiaY9EYFIC8mlX8C7HPRRooagMKRwVZx7Q')
    #Get from bing:
    result_list, next_uri = bing.search(linkfile, limit=5, format='json')
    #result_list, next_uri = bing.search("Python Software Foundation", limit=50, format='json')
    result_list[0].description #first bing result
    file = open( 'bingResults.txt', 'w')
    for res in result_list:
        file.write('"' + res.url + '" ')
        break
    print "\nbingSearch complete"
    return str(result_list[0].url)
"""Uses Microsft Azure keyword extraction through Azure's Machine Learning APIs: Text Analytics
Extracts the keywords from voice module for bing to search with.  This way, if we get a long rambling of text, we can extract the meaningful keywords and search just these """
def keywordExtract(input_text):
   
    print "\nCalling keywordExtract with arguments input_text: {}".format(str(input_text))
    
    #Connect to azure and get keywords
    account_key="YQ/nqcgsmnyK1EMwzib6Tldg9XpTBA9+CSD6LMG2KQ4"
    base_url = 'https://api.datamarket.azure.com/data.ashx/amla/text-analytics/v1'
    creds = base64.b64encode('AccountKey:' + account_key)
    headers = {'Content-Type':'application/json', 'Authorization':('Basic '+ creds)}
    params = { 'Text': input_text}

    key_phrases_url = base_url + '/GetKeyPhrases?' + urllib.urlencode(params)
    req = urllib2.Request(key_phrases_url, None, headers) 
    response = urllib2.urlopen(req)
    result = response.read()
    obj = json.loads(result)
    print "\nkeyphraseExtract complete"
    return (" ".join(obj['KeyPhrases'])) 

"""Downloads twilio recording to server.  We do not need this and have kept this here only for debugging purposes.   """
def down(fileName):
    webFile=urllib.urlopen(fileName)
    output=open("symptoms.wav", "wb")
    output.write(webFile.read())
    output.close()
#Helper function for parse:
#Written by Evan
def find2ndIndex(someString, subString):
    first=someString.find(subString)

    return first+ someString[first+1:].find(subString)
"""parse opens the webmd address returned by bing search and then writes a list of diseases and their corresponding symptoms to a file in json format
Even indices are the diseases, odd numbers are their preceding indices description """
def parse(url, fileout):
    print "\nCalling parse (web-md) with arguments url: {} and fileout: {}".format(str(url),str(fileout))
    hdr = {'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.64 Safari/537.11',
           'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
           'Accept-Charset': 'ISO-8859-1,utf-8;q=0.7,*;q=0.3',
           'Accept-Encoding': 'none',
           'Accept-Language': 'en-US,en;q=0.8',
           'Connection': 'keep-alive'}

    #Open up webMD
    req = urllib2.Request(url, headers=hdr)
    page =urllib2.urlopen(req)
    data=page.readlines()
   
    #Hardcoded formatting:
    returning=[]
    currentLine=0
    while "results_list" not in data[currentLine]:
        currentLine+=1
    currentLine+=1
    while "<li" in data[currentLine] or "</li" in data[currentLine]:
        if "</li" in data[currentLine]:
            currentLine+=1
        else:

            nameIndexStart=find2ndIndex(data[currentLine], ">")+2
            nameIndexEnd=data[currentLine][nameIndexStart:].find("<")+nameIndexStart
            name=data[currentLine][nameIndexStart:nameIndexEnd]

            descriptionIndexStart=nameIndexEnd+find2ndIndex(data[currentLine][nameIndexEnd:],">")+2
            descriptionIndexEnd=descriptionIndexStart+data[currentLine][descriptionIndexStart:].find("</p")
            description=data[currentLine][descriptionIndexStart:descriptionIndexEnd]
            returning.append(name)
            returning.append(description)
            currentLine+=1
    json.dumps(returning)

    #Write the file out
    with open(fileout, "w") as outfile:
        outfile.write(json.dumps(returning))
    print "\nparse complete"
"""convert the speech to text.  Takes our downloaded file and transcribes it
Uses Microsoft Project Oxford API for the transcription
Errors sometimes happen with poor recording quality but for us Project Oxford in the best transcription software out on the market
"""
def speechToText(filename):
    print "\nCalling speechToText with arguments fiename: {}".format(str(filename))
    clientId = "cd387418-00a5-41be-9dd5-44b9b12ddf2b"
    clientSecret = "37f6f2b046e24ad980692f3b965fc30f"
    ttsHost = "https://speech.platform.bing.com"

    params = urllib.urlencode({'grant_type': 'client_credentials', 'client_id': clientId, 'client_secret': clientSecret, 'scope': ttsHost})

    print ("The body data: %s" %(params))

    headers = {"Content-type": "application/x-www-form-urlencoded"}

    AccessTokenHost = "oxford-speech.cloudapp.net"
    path = "/token/issueToken"

    # Connect to server to get the Oxford Access Token
    conn = httplib.HTTPSConnection(AccessTokenHost)
    conn.request("POST", path, params, headers)
    response = conn.getresponse()
    print(response.status, response.reason)

    data = response.read()
    conn.close()
    accesstoken = data.decode("UTF-8")
    print ("Oxford Access Token: " + accesstoken)

    #decode the object from json
    ddata=json.loads(accesstoken)
    access_token = ddata['access_token']

    # Read the binary from wave file
    f =urllib.urlopen(filename)
    

    try:
        body = f.read();
    finally:
        f.close()

    headers = {"Content-type": "audio/wav; samplerate=8000",
    "Authorization": "Bearer " + access_token}

    #Connect to server to recognize the wave binary
    conn = httplib.HTTPSConnection("speech.platform.bing.com")
    conn.request("POST", "/recognize/query?scenarios=ulm&appid=D4D52672-91D7-4C74-8AD8-42B1D98141A5&locale=en-US&device.os=wp7&version=3.0&format=json&requestid=1d4b6030-9099-11e0-91e4-0800200c9a66&instanceid=1d4b6030-9099-11e0-91e4-0800200c9a66", body, headers)
    response = conn.getresponse()
    print(response.status, response.reason)
    data = response.read()
    conn.close()
    print "\nspeechToText complete"
    return data
"""This function called first.  We are using flask.  This is at the root.
Uses Twilio API"""
@app.route("/", methods=['GET', 'POST'])
def hello_monkey():
    print "\nRunning: hello_monkey"
    account_sid="ACf24960d772c72c2d8316e288bb1de59b"
    auth_token="192a241d9472ca66e865290f02cab554"
    try:
        client=TwilioRestClient(account_sid, auth_token)
        from_number = request.values.get('From')
        message_body=request.values.get("Body")
       
        #if a text was recieved, run search()
        if message_body: 
            resp=twilio.twiml.Response()
            try:
                resp.redirect("/search")
            except:
                print "\nMessaging Complete"
                resp.redirect("/")
        else:  #voice call 
            resp = twilio.twiml.Response()
            # Greet the caller
            resp.say("Hello.  Welcome to WhatsUpDoc.")
        # Gather digits.
            with resp.gather(numDigits=1, action="/handle-key", method="POST") as g:  #calls function handle_key
                g.say("""For the Medical Module, press 1.  For the weather module, press 2.""")
    except: 
        print "Preparing for new caller...\n\n"
    resp.redirect("/")
    return str(resp)

"""Search uses the recieved text to run a bing search using Microsoft Bing API.  Results are then summarized with our summarizer.
Uses Microsoft Bing API
"""
@app.route("/search", methods=['GET', 'POST'])
def search():
    print "\nRunning search()"
    global MORE  #used to track length of response message 
    account_sid="ACf24960d772c72c2d8316e288bb1de59b"
    auth_token="192a241d9472ca66e865290f02cab554"
    client=TwilioRestClient(account_sid, auth_token)
    from_number = request.values.get('From')  #gets sender of text
    message_body=request.values.get("Body") #extract text body

    print "\n\n\n MESSAGE BODY FROM CALL \n\n\n" + str(message_body)

    resp = twilio.twiml.Response()
    if (message_body and string.lower(message_body)=="more"):
        #send extended summary
        MORE=MORE*2
        summary("summarySMS.txt", MORE)
        with open("summaryResults.txt", "r") as outfile:
            results=outfile.readlines()
        results="".join(results)
        print results
        listOfMessages=serialize(results)
        #unfortunately, the cell carrier of the user dictates the order messages are recieved in
        for i in xrange(len(listOfMessages)-1,-1,-1):
              someStr  =str(unicodedata.normalize("NFKD",unicode(str(("Part "+str(i+1)+ "/"+ str(len(listOfMessages))+" "+ listOfMessages[i])))).encode('ascii', 'ignore'))
              message=client.messages.create(to=from_number, from_="+18564524812", body=someStr)
    
              resp.sms(message)
        
       
    elif message_body: 
        MORE=50 #reset messasge length
        
        #download and extract meaningful text from article
        article=Article(bingSearch(message_body))
        article.download()
        article.parse()
        textOut=((str)(repr(article.text))).replace("\\n","\n")
        textOut2=textOut[2:-2].encode('ascii', 'ignore')
        textOutList=[]
        for each in textOut2:
            if not "\\u" in each:
               textOutList.append(each)
            else:
                textOutList.append(" \n ")
        #format resulting file
        textOut2="".join(textOutList)
        textOut2=(str(unicodedata.normalize("NFKD", unicode(textOut2)).encode('ascii', 'ignore')))
        with open("summarySMS.txt", "w") as outfile:
             outfile.write(textOut2)
        summary("summarySMS.txt", MORE)
        with open("summaryResults.txt", "r") as outfile:
            results=outfile.readlines()  #output file for the summarizer
        
        results="".join(results)
        resp = twilio.twiml.Response()
        listOfMessages=serialize(results)
        #create list of messages.  We cannot control order these are sent in
        for i in xrange(len(listOfMessages)-1,-1,-1):
              someStr  =str(unicodedata.normalize("NFKD",unicode(str(("Part "+str(i+1)+ "/"+ str(len(listOfMessages))+" "+ listOfMessages[i])))).encode('ascii', 'ignore')) 
              message=client.messages.create(to=from_number, from_="+18564524812", body=someStr)
              resp.sms(message)
    return str(resp)

"""Handles a key press from a user.  Used to navigate a user throughout our directory.
"""
@app.route("/handle-key", methods=['GET', 'POST'])
def handle_key():

    digit_pressed = request.values.get('Digits', None)
    if digit_pressed == "1":
       resp = twilio.twiml.Response()
#WhatsUpDoc aims to provide information about common medical concerns telephonically to.  Please do not rely on WhatsUpDoc for diagnoses or definitive medical care.  Please enter your symptoms at the tone.
       resp.say("Hello, welcome to the medical module of WhatsUpDoc. Please state your symptoms after the tone. ")
     
       resp.record(maxLength="30", action="/handle-recording") #runs handle_recording()

       return str(resp)
    elif digit_pressed == "2": #sorry! We haven't gotten around to completing this yet!
       resp = twilio.twiml.Response()
       resp.say("This module is currently under construction.")
       return str(resp)
    # If the caller pressed anything but 1, redirect them to the homepage.
    else:   return redirect("/")

#Create the list of text messages of appropriate character length
#written by Evan
def serialize(someStr):
   return [someStr[i:i+140] for i in range(0,len(someStr), 140)] 

"""Handles the voice call.  Recieves a recording and runs WhatsUpDoc's Medical Voice Module.  See presentation for program flow.
"""
@app.route("/handle-recording", methods=['GET', 'POST'])  #this is what is doing everything
def handle_recording():
    print "\nRunning handle_recording()"
    global COUNTER #keeps track of the number of errors a user has had
    if (COUNTER>=5): #if we have failed 5 times in a row:
         resp = twilio.twiml.Response()
         resp.say("I am unable to understand your voice information.  Please try calling back later")
         return str(resp)
    try:  #otherwise, try to get their symptoms. 
        #Get the url of twilio voice recording
        recording_url = request.values.get("RecordingUrl", None)
        resp = twilio.twiml.Response()
        jsonResponse= speechToText(recording_url)
        speechDict=json.loads(jsonResponse)
        symptoms=speechDict["header"]["name"]
       
        #What Project Oxford transcribed the user's voice data to be 
        print "\nProject Oxford API has transcribed symptoms as: " + symptoms
        
        #Extract the keywords of the spoken symptoms
        if len(symptoms)<30:
             symptoms=keywordExtract(symptoms)

        #Use bing to get the URL of the first webMD result matching keywords
        chiefResult=bingSearch(symptoms + " \"multiple-symptoms\" site:symptomchecker.webmd.com")
        print "\nChief result: " + chiefResult

        #Open that address and clean up the resulting file.  We write all these results into a json file so that we can audit our program to ensure correctness of queries
        parse(str(chiefResult) ,"webMDOut.json")
         
        with open("webMDOut.json") as infile:  #load from webMD file
            webMDData= json.load(infile)

        resp.say("You may have " + str(webMDData[0]))
        resp.say(str(webMDData[1]))  #read webMD definition of nature of illness

        mayoTextURL=bingSearch(str(webMDData[0]) +" site:mayoclinic.org")
        webMDData=webMDData[0]
        webMDData=str(unicodedata.normalize("NFKD", webMDData).encode('ascii', 'ignore')) #clean d'at ugly unicode

        #What is the first symptom?  Search the mayo article for this to help make sure we are ignoring garbage text
        formattedWord=str(webMDData)[0:1].upper()+str(webMDData)[1:].lower()
        formattedWord=formattedWord.split()[0]
        parseMayo(mayoTextURL, formattedWord)

        #summarize the resulting output
        summary("mayoTextOut.txt", 85)
        #write it to summaryResults so that 
        with open("summaryResults.txt", "r") as outfile:
            results=outfile.readlines()
        results="".join(results)
        resp.say(results)
        summary("mayoTextOut.txt", 500)
        resp.redirect("/redirector")
    except:  #we didn't understand them
        COUNTER+=1
        resp.say("I'm sorry, something went wrong.  Could you please repeat your symptoms after the tone?")
        resp.record(maxLength="30", action="/handle-recording") #try recording again
    print "\nhandle_recording() complete"
    return str(resp)

#calls redirector_helper
#called after medical diagnoses has been supplied
#resets counter 
@app.route("/redirector", methods=['GET', 'POST'])
def handle_redirector():
     print "\nRunning handle_redirector()"
     global COUNTER
     COUNTER=0
     resp = twilio.twiml.Response()
     with resp.gather(numDigits=1, action="/redirectorHelper", method="POST") as g:  #calls function handle_key
        g.say("""To hear more about this condition, press 1.  To make a new medical query, press 2.  Press any key or stay on the line to return to the main menu""")
     print "\nhandle_redirector() complete"
     return str(resp)

#Gathers digits and provides services based on input
@app.route("/redirectorHelper", methods=['GET', 'POST'])
def handle_redirector_helper():
     print "\nrunning handle_redirector_helper()"
     digit_pressed = request.values.get('Digits', None)
     resp=twilio.twiml.Response()
     
     if digit_pressed == "1":  #if the digit is "1", provide further detail
         with open("summaryResults.txt", "r") as outfile:
             results=outfile.readlines()
         results="".join(results)
         resp.say(results)
         print "\nhandle_redirector_helper completed with increase in summary"
         return str(resp)
     elif digit_pressed == "2":  #if digit is 2, loop back to medical modue
         resp = twilio.twiml.Response()
#WhatsUpDoc aims to provide information about common medical concerns telephonically to.  Please do not rely on WhatsUpDoc for diagnoses or definitive medical care.  Please enter your symptoms at the tone.
         resp.say("As before, state your symptoms after the tone")

         resp.record(maxLength="30", action="/handle-recording")
         print "\nhandle_redirector_helper completed with redirection to start of medical module"
         
         return str(resp)
     else: #redirect to start
         resp.redirect("/")
         print "\nhandle_redirector_helper completed with redirection to root"
         return str(resp)
     

if __name__ == "__main__":
    app.run(debug=True)
