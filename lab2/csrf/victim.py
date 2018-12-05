#!/usr/bin/env python
import sys, random, datetime
# Use a framework to minimize the effort!
import flask

#This is our webapp
app = flask.Flask(__name__)
app.config['WTF_CSRF_METHODS'] = []
app.config['TESTING'] = True

postingHeader = '''
<html><body>
<script>
function logout() {
document.cookie = "id_victim=x; expires=Thu, 01 Jan 1970 00:00:00 UTC";
window.location.href = "http://example.com:7777/victim/login";
}
</script>
<h3> Welcome to Victim! </h3>
<button
onclick="logout();">
Logout
</button>
</form>
'''
postingFooter = '''
</body></html>
'''
postingForm = '''
<form style="width=800px; height=400px" method="POST">
  <input type="text" style="width=600px; height=200px"
  id="post_victim"
  name="post_victim"
  value="Post here!"
  onfocus="document.getElementById('post_victim').value='';" />
  <input id="button" type="submit" value="post" />
</form>
'''

postings = []
postings.append({'ID':'user1','content':"I'm user 1!!!"})
postings.append({'ID':'user2','content':"I'm user 2!!!"})
postings.append({'ID':'user3','content':"I'm user 3!!!"})

valid_cookies = {}
def handleNewPost(ID,post):
    postings.append({'ID':ID,'content':post})
    pass

def handleLogin(ID):
    global valid_cookies
    token = ''
    for i in range(0,128):
        token += format(random.randrange(0,15),'X')
    valid_cookies[ID] = token
    return ID + '::' + token

def checkLogin():
    cookie = flask.request.cookies.get('id_victim')
    if not cookie or '::' not in cookie:
        ID = None
        token = None
    else:
        c = cookie.split('::')
        ID = c[0]
        token = c[1]
    if not ID or ID not in valid_cookies or valid_cookies[ID] != token:
        return None
    else:
        return ID

def getPostingPage(ID):
    page  = postingHeader
    page += '<h5> Please post something! ' + ID + '</h5>'
    page += postingForm
    for p in postings:
        page += '<p><b>' + p['ID'] + '</b>: ' + p['content'] + '</p>'
    return flask.Response(page)

def getfile(name):
    return open(name,'r').read()

@app.route("/victim/login",methods=['GET','POST'])
def login():
    if flask.request.method == 'GET':
        ID = checkLogin()
        if not ID:
            page = getfile('login.html')
            resp = flask.Response(page)
        else:
            resp = flask.redirect("/victim/posts")
    elif flask.request.method == 'POST':
        ID = flask.request.form['id_victim']
        #resp = getPostingPage(ID)
        resp = flask.redirect("/victim/posts")
        cookie = handleLogin(ID)
        resp.set_cookie('id_victim',cookie,path='/victim',expires=datetime.datetime.now()
                + datetime.timedelta(days=30))
        pass
    else:
        raise Exception
    resp.set_cookie('stamp','x',path='/victim')
    return resp

@app.route("/victim/posts",methods=['GET','POST'])
def posts():
    if flask.request.method == 'GET':
        ID = checkLogin()
        if not ID:
            resp = flask.redirect("/victim/login")
        else:
            if not flask.request.form.get('post_victim'):
                pass
            resp = getPostingPage(ID)
    elif flask.request.method == 'POST':
	print('we are here')	
        ID = checkLogin()
        if not ID:
	    print ('not ID :(')
            resp = flask.redirect("/victim/login")
        else:
	    print ('ID is good')
            handleNewPost(ID,flask.request.form['post_victim'])
            resp = getPostingPage(ID)
    resp.set_cookie('stamp','x',path='/victim')
    return resp

@app.route("/victim/clean-cache", methods=['GET','POST'])
def clean_cache():
    page = open('clean-cookie.html','r').read()
    page += str(flask.request.cookies)
    page += '\n</body></html>'
    resp = flask.Response(page)
    resp.set_cookie('id_victim','x',path='/victim/posts',expires='0')
    return resp

if __name__ == "__main__":
    app.run(host='example.net', port="8888")

