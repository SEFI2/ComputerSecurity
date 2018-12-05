#!/usr/bin/env python
import flask
app  = flask.Flask(__name__)


'''
The attacker's server for lab2.
'''
main_page = '''
<!doctype html><html>Working!</html>
'''
empty_page = '''
<!doctype html>
<html lang="en">
Message received!
</html>
'''

@app.route("/",methods=['GET'])
def resp_main_page():
    resp = flask.Response(main_page)
    return resp

@app.route("/benign",methods=['POST'])
def benign():
    resp = flask.Response(empty_page)
    return resp

@app.route("/malicious",methods=['GET'])
def malicious():
    page = open('malicious.html','r').read()
    return flask.Response(page)


if __name__ == '__main__':
    app.run(host='example.org', port=4444)
