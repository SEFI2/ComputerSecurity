#!/usr/bin/env python
import sys
# Use a framework to minimize the effort!
import flask

#This is our webapp
app = flask.Flask(__name__)

# This array will keep our postings... Yes it will... until you shutdown the
# app...
postings = []

#This app has minimal header and footer
header = '''
<!doctype html>
<html lang = "en"><body>

'''
footer = '''
</body></html>
'''

# This is our form that users type their posts with.
form_stored = '''
<div id="empty"></div>
<h1>Please leave a message to other visitors</h1>
<form style="width=500px; height=200px" method="POST">
<input type="text" style="width=400px; height=150px" id="new-posting"
name="new-posting" value="Write your post here"
onfocus="document.getElementById('new-posting').value=''">
<input id="button" type="submit" value="submit">
</form>
'''

# This is called whenever our app receives a HTTP request for
# domain (example.org)/stored
# Yes, we'll redirect example.org to here for testing purposes.
# As declared, our app will only handle HTTP "GET" and "POST" requests.
@app.route("/stored", methods=['GET','POST'])
def stored():
    global postings
    # our page always starts with the header
    page = header
    # and then show the form on top.
    page += form_stored
    print(flask.request)
    # our client sends a POST request when a user is adding a post.
    if flask.request.method == 'POST':
        postings.append(flask.request.form['new-posting'])
    # for both the requests, we respond with the updated postings.
    for p in postings:
        page += '<p>' + p + '</p>' + '\n'
    page += footer 
    # This instructs Flask to create a response packet with the page
    resp = flask.Response(page)
    
    # Uncomment this if you want to see the transferred page from the log
    # terminal
    # print(page)

    # Uncomment this if you want to dump the page as a file every time it's
    # served. Please note that you can always check the client-side page
    # from "view source" feature of the chrome browser.
    # open('stored.html','w+').write(page)

    resp.headers['X-XSS-Protection'] = '0'
    resp.set_cookie('secret','secret',path='/')
    return resp


# This is our form to learn who loaded the page.
# We use GET instead of POST: the form contents will be encoded into the URI.
form_reflected = '''
<form style="width=500px; height=200px" method="GET">
<input type="text" style="width=400px; height=150px" id="name"
name="name"
value="What is your name?"
onfocus="document.getElementById('name').value=''">
<input id="button" type="submit" value="submit">
</form>
'''

# our reflected handles GET only
@app.route("/reflected", methods=['GET'])
def reflected():
    page = header
    parsed = flask.request.url.split('?')
    if len(parsed) > 1:
        form_data = parsed[1]
    else:
        form_data = None
    if form_data != None:
        page += '<p> Hello ' + flask.request.args.get('name') + '</p>'
    else:
        page += '<h3> Please let me know your name! </h3>'
        page += form_reflected
    page += footer
    resp = flask.Response(page)
    resp.headers['X-XSS-Protection'] = '0'

    return resp

@app.route("/dom", methods=['GET'])
def dom():
    print("hiiiiii")
    page = open('dom.html','r').read()
    resp = flask.Response(page)
    return resp



if __name__ == "__main__":
    app.run(host='example.com', port="7777")

